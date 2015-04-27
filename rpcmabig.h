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

	dMat ask() {
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

