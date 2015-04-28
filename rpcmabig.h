#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

#include <cmaes.h>
#include "rpcmasmall.h"

/*
 * TODO: See why elapsed time isn't present
 */

namespace libcmaes
{

template <class TCovarianceUpdate,class TGenoPheno>
class RPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>
{
protected:

	//small dimension strategy
	RPCMASmall<TCovarianceUpdate,TGenoPheno>* _sdimstrat;

	/* 
	 * Using pointer because of a twisted issue if not.. See parameters-tests branch for more info
	 */
	//this pointer is just intended to save the parameters set by the user for rpcmasmall purpose before forwading it to parent
	CMAParameters<TGenoPheno>* _backupParams;

	HyperParameters _hp;

    /**
     * \brief Setup parameters for big dimension strategy
     * @param original parameters usually set by user
     */
	CMAParameters<TGenoPheno>& setupParameters(CMAParameters<TGenoPheno>& params) {
		//save the parameters for rpcmasmall purpose
		_backupParams = new CMAParameters<TGenoPheno>(params);
		//set it up for future uses
		params.set_str_algo("sepacmaes");
		if (!params.is_sep())
			params.set_sep();
		params.initialize_parameters();
		return params;
	}

    /**
     * \brief Set dimension value in params according to d. This is usually called before before being forwarded to small dimension strategy
     * @param original parameters usually set by user
     * @param the new dimension
     */
	CMAParameters<TGenoPheno>& setupDim(CMAParameters<TGenoPheno>& params, const int& d) const {
		params._dim = d;
		//prevents x0 conflicting with dim
		params.set_x0(0.0);
		params.initialize_parameters();
		return params;
	}

    /**
     * \brief Build a new Small dimension strategy with dimension of d and sparams parameters
     * @param original parameters usually set by user
     * @param the new dimension
     */
	RPCMASmall<TCovarianceUpdate,TGenoPheno>* setupSDimStrat(CMAParameters<TGenoPheno>& sparams, const int& d) {
		return new RPCMASmall<TCovarianceUpdate,TGenoPheno>(setupDim(sparams, d), this);
	}

public:

    /**
     * \brief dummy constructor
     */
/*
    RPCMABig() : CMAStrategy<TCovarianceUpdate,TGenoPheno>() {}
*/
    /**
     * \brief constructor.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     */
    RPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &bparams) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, setupParameters(bparams)) {
		_sdimstrat = setupSDimStrat(*_backupParams, _hp.d());
		/*
		 * for multi RPCMA maybe create another var for sparams
		 */
	}

    /**
     * \brief constructor for starting from an existing solution.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     * @param cmasols solution object to start from
     */

	~RPCMABig() {
		delete _sdimstrat;
		delete _backupParams;
	}

  dMat ask()
  {
#ifdef HAVE_DEBUG
    std::chrono::time_point<std::chrono::system_clock> tstart = std::chrono::system_clock::now();
#endif
    
    // compute eigenvalues and eigenvectors.
    CMAStrategy<TCovarianceUpdate,TGenoPheno>::initialize_esolver_and_solutions();

    //debug
    //std::cout << "transform: " << _esolver._transform << std::endl;
    //debug
    
	_sdimstrat->ask();
    // sample for multivariate normal distribution, produces one candidate per column.
	dMat pop(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda());
	dMat candidates(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), _hp.k());
    
	for(int i=0; i< CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda(); i++) {
		candidates = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver.samples_ind(_hp.k(),CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma);
		pop.col(i) = _sdimstrat->bestCandidate(candidates);
	}

    // gradient if available.
    if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._with_gradient)
      {
	dVec grad_at_mean = CMAStrategy<TCovarianceUpdate,TGenoPheno>::gradf(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._gp.pheno(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean));
	dVec gradgp_at_mean = CMAStrategy<TCovarianceUpdate,TGenoPheno>::gradgp(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean); // for geno / pheno transform.
	grad_at_mean = grad_at_mean.cwiseProduct(gradgp_at_mean);
	if (grad_at_mean != dVec::Zero(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._dim))
	  {
	    dVec nx;
	    if (!CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._sep && !CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._vd)
	      {
		dMat sqrtcov = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver._eigenSolver.operatorSqrt();
		dVec q = sqrtcov * grad_at_mean;
		double normq = q.squaredNorm();
		nx = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean - CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma * (sqrt(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._dim / normq)) * CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._cov * grad_at_mean;
	      }
	    else nx = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean - CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma * (sqrt(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._dim) / ((CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sepcov.cwiseSqrt().cwiseProduct(grad_at_mean)).norm())) * CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sepcov.cwiseProduct(grad_at_mean);
	    pop.col(0) = nx;
	  }
      }

    // tpa: fill up two first (or second in case of gradient) points with candidates usable for tpa computation
    if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._tpa == 2  && CMAStrategy<TCovarianceUpdate,TGenoPheno>::_niter > 0)
      {
	dVec mean_shift = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean - CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean_prev;
	double mean_shift_norm = 1.0;
	if (!CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._sep && !CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._vd)
	  mean_shift_norm = (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver._eigenSolver.eigenvalues().cwiseSqrt().cwiseInverse().cwiseProduct(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver._eigenSolver.eigenvectors().transpose()*mean_shift)).norm() / CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma;
	else mean_shift_norm = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sepcov.cwiseSqrt().cwiseInverse().cwiseProduct(mean_shift).norm() / CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma;
	//std::cout << "mean_shift_norm=" << mean_shift_norm << " / sqrt(N)=" << std::sqrt(std::sqrt(eostrat<TGenoPheno>::_parameters._dim)) << std::endl;

	dMat rz = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver.samples_ind(1);
	double mfactor = rz.norm();
	dVec z = mfactor * (mean_shift / mean_shift_norm);
	CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_x1 = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean + z;
	CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_x2 = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._xmean - z;
	
	// if gradient is in col 0, move tpa vectors to pos 1 & 2
	if (CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._with_gradient)
	  {
	    CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_p1 = 1;
	    CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_p2 = 2;
	  }
	pop.col(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_p1) = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_x1;
	pop.col(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_p2) = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._tpa_x2;
      }
    
    // if some parameters are fixed, reset them.
    if (!CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._fixed_p.empty())
      {
	for (auto it=CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._fixed_p.begin();
	     it!=CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._fixed_p.end();++it)
	  {
	    pop.block((*it).first,0,1,pop.cols()) = dVec::Constant(pop.cols(),(*it).second).transpose();
	  }
      }
    
    //debug
    //DLOG(INFO) << "ask: produced " << pop.cols() << " candidates\n";
    //std::cerr << pop << std::endl;
    //debug

#ifdef HAVE_DEBUG
    std::chrono::time_point<std::chrono::system_clock> tstop = std::chrono::system_clock::now();
    CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._elapsed_ask = std::chrono::duration_cast<std::chrono::milliseconds>(tstop-tstart).count();
#endif
    
    return pop;
  }

/*
	dMat ask() {
		//_sdimstrat->initialize_esolver_and_solutions();
		_sdimstrat->ask();
		int lambda = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda();
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._lambda = _hp.k();

		dMat pop(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), lambda);
		dMat candidates(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), _hp.k());

		for(int i=0; i< lambda; i++) {
			candidates = CMAStrategy<TCovarianceUpdate,TGenoPheno>::ask();
			pop.col(i) = _sdimstrat->bestCandidate(candidates);
		}

		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters._lambda = lambda;
		return pop;
	}
*/
	void eval(const dMat &candidates, const dMat &phenocandidates=dMat(0,0)) {
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::eval(candidates, phenocandidates);
		_sdimstrat->eval(_sdimstrat->randProjection() * candidates);
	}

	void tell() {
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::tell();
		_sdimstrat->tell();
	}

      /**
       * \brief Finds the minimum of the objective function. It makes
       *        alternate calls to ask(), tell() and stop() until 
       *        one of the termination criteria triggers.
       * @return success or error code, as defined in opti_err.h
       * Note: the termination criteria code is held by _solutions._run_status
       */
    int optimize()
    {
      return CMAStrategy<TCovarianceUpdate,TGenoPheno>::optimize(
		      std::bind(&RPCMABig<TCovarianceUpdate,TGenoPheno>::eval,this,std::placeholders::_1,std::placeholders::_2),
		      std::bind(&RPCMABig<TCovarianceUpdate,TGenoPheno>::ask,this),
		      std::bind(&RPCMABig<TCovarianceUpdate,TGenoPheno>::tell,this));
    }

};

}

#endif

