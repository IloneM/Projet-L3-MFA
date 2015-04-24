#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

//#include "abstractrpcma.h"
#include <cmaes.h>
#include "rpcmasmall.h"

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno>
class RPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>//AbstractRPCMA<TCovarianceUpdate,TGenoPheno>
{
protected:

//int _sdim;

	RPCMASmall<TCovarianceUpdate,TGenoPheno>* _sdimstrat;//small dimension strategy

	dMat _candidates;

	HyperParameters _hp;

public:

	inline dMat candidates() {
		return _candidates;
	}

	inline HyperParameters hp() {
		return _hp;
	}

    /**
     * \brief dummy constructor
     */
    RPCMABig() : CMAStrategy<TCovarianceUpdate,TGenoPheno>() {}//, _sdim(0) {}
  
    /**
     * \brief constructor.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     */
    RPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &bparams)//,
		//_randProjection(grp(SMALL_DIM, params.dim()))//, _sdim(SMALL_DIM)
	{
		bparams.set_str_algo("sepacmaes");
		/*
		 * As algorithm isn't changed by initialize_parameters, we don't call the procedure. Must evoluate as library does
		 */
//		bparams.initialize_parameters();
		CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, bparams);
		
		_sdimstrat = new RPCMASmall<TCovarianceUpdate,TGenoPheno>(_hp.d(), this);
	}

    /**
     * \brief constructor for starting from an existing solution.
     * @param func objective function to minimize
     * @param parameters stochastic search parameters
     * @param cmasols solution object to start from
     */
    RPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &params,
	  const CMASolutions &cmasols) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, params, cmasols) {}//, _sdim(SMALL_DIM) {}

	~RPCMABig() {
		delete _sdimstrat;
	}

	dMat ask() {
		int lambda = CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda();
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.set_lambda(_hp.k());

		dMat pop(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), lambda);
		dMat candidates(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), _hp.k());

		for(int i=0; i< lambda; i++) {
			candidates = CMAStrategy<TCovarianceUpdate,TGenoPheno>::ask();
			pop.col(i) = _sdimstrat->bestCandidate(candidates);
		}

		CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.set_lambda(lambda);
		return pop;
	}
};

//template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
//class RPCMA : public RPCMABig<TCovarianceUpdate,TGenoPheno> {};

#endif

