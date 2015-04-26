#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

#include <cmaes.h>
#include "rpcmasmall.h"

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

namespace libcmaes
{

template <class TCovarianceUpdate,class TGenoPheno>
class RPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>
{
protected:

	RPCMASmall<TCovarianceUpdate,TGenoPheno>* _sdimstrat;//small dimension strategy

	//this pointer is just intended to save the parameters set by the user for rpcmasmall purpose before forwading it to parent
	CMAParameters<TGenoPheno> _sparams;

//	dMat _candidates;

	HyperParameters _hp;

public:

	CMAParameters<TGenoPheno>& setupParameters(CMAParameters<TGenoPheno>& params) {
		//save the parameters for rpcmasmall purpose
		//_sparams = new CMAParameters<TGenoPheno>(params);
		//should be initialize here as prevent compilation if done in rpcmasmall (not considered as referencce)
		params.set_str_algo("sepacmaes");
		return params;
	}

	void/*CMAParameters<TGenoPheno>&*/ setupSmallParameters(CMAParameters<TGenoPheno>& params) {
		params.set_uh(false);
		//return params;
	}
/*
	dMat setupRPCMASmall(int d, int d, CMAParameters<TGenoPheno>& params) {
		dMat randProjection(RPCMASmall::grp(d, D));
		params.set_x0(randProjection * _bdimstrat->get_parameters().get_x0min(),
					randProjection * _bdimstrat->get_parameters().get_x0max());
		return params;
	}
*/
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
	  CMAParameters<TGenoPheno> &bparams) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, setupParameters(bparams)),
			_sparams(_hp.d(), new double[_hp.d()], bparams.get_sigma_init(), -1, bparams.get_seed(), bparams.get_gp()) {
//		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.set_str_algo("sepacmaes");
		//CMAParameters<TGenoPheno> sparams(_hp.d(), new double[_hp.d()], bparams.get_sigma_init());//should be initialize here as prevent compilation if done in rpcmasmall (not considered as referencce)
		setupSmallParameters(_sparams);
		_sdimstrat = new RPCMASmall<TCovarianceUpdate,TGenoPheno>(_sparams, this);

		/*
		 * for multi RPCMA maybe use a different var for sparams
		 */

//		std::cout << ask();
	}

    /**
     * \brief constructor for starting from an existing solution.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     * @param cmasols solution object to start from
     */
/*
    RPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &params,
	  const CMASolutions &cmasols) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, params, cmasols) {}
*/
	~RPCMABig() {
		delete _sdimstrat;
//		delete _sparams;
	}
/*
	inline dMat candidates() const {
		return _candidates;
	}

	inline HyperParameters hp() const {
		return _hp;
	}
*/
	dMat ask() {
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

};

}

#endif

