#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

#include <cmaes.h>
#include "rpcmasmall.h"

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno>
class RPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>
{
protected:

	RPCMASmall<TCovarianceUpdate,TGenoPheno>* _sdimstrat;//small dimension strategy

	dMat _candidates;

	HyperParameters _hp;

public:

	CMAParameters<TGenoPheno>& setupParameters(CMAParameters<TGenoPheno>& params) {
		params.set_str_algo("sepacmaes");
		return params;
	}

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
	  CMAParameters<TGenoPheno> &bparams) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, setupParameters(bparams))
	{
//		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.set_str_algo("sepacmaes");
		CMAParameters<TGenoPheno> sparams(_hp.d(), new double[_hp.d()], bparams.get_sigma_init());//should be initialize here as prevent compilation if done in rpcmasmall (not considered as referencce)
		_sdimstrat = new RPCMASmall<TCovarianceUpdate,TGenoPheno>(sparams, this);

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
	}

	inline dMat candidates() {
		return _candidates;
	}

	inline HyperParameters hp() {
		return _hp;
	}

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
};

#endif

