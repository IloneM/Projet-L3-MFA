#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

#include <cmaes.h>
#include "rpcmasmall.h"

namespace libcmaes
{

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>
{
protected:

	//small dimension strategy
	RPCMASmall<RPCMABig<TCovarianceUpdate,TGenoPheno>, TCovarianceUpdate,TGenoPheno>* _sdimstrat;

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
		//params.set_str_algo("sepacmaes");
		params.set_str_algo("sepcmaes");
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
	RPCMASmall<RPCMABig<TCovarianceUpdate,TGenoPheno>, TCovarianceUpdate,TGenoPheno>* setupSDimStrat(CMAParameters<TGenoPheno>& sparams, const int& d) {
		return new RPCMASmall<RPCMABig<TCovarianceUpdate,TGenoPheno>, TCovarianceUpdate,TGenoPheno>(setupDim(sparams, d), this);
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

	virtual dMat ask() {
		//CAUTION: The ask function of big dim strat uses parameters initialized by small dim ask function.
		//Therefore is mandatory to call small dim ask function before big dim one. See build_pop for more infomartions.
		_sdimstrat->ask();
		return CMAStrategy<TCovarianceUpdate,TGenoPheno>::ask();
	}

	virtual void tell() {
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::tell();
		_sdimstrat->eval();
		_sdimstrat->tell();
	}
/*
	virtual bool stop() {

		bool res_big = CMAStrategy<TCovarianceUpdate,TGenoPheno>::stop();
		bool res_small = _sdimstrat->stop();
		std::cout << CMAStrategy<TCovarianceUpdate,TGenoPheno>::get_solutions().get_best_seen_candidate().get_x_dvec().transpose() << '\n';
		std::cout << "Big is " << res_big << " Small is " << res_small << '\n';
		std::cout << CMAStrategy<TCovarianceUpdate,TGenoPheno>::get_solutions().get_best_seen_candidate().get_fvalue() << '\n';
		if(res_big) {
			//_sdimstrat->re_grp();
			delete _sdimstrat;
			_sdimstrat = setupSDimStrat(*_backupParams, _hp.d());
		}

		//return (res_big && res_small);
		return res_big;

		//return (CMAStrategy<TCovarianceUpdate,TGenoPheno>::stop() && _sdimstrat->stop());
	}
*/
	virtual void build_pop(dMat& pop) {
		// sample for multivariate normal distribution, produces one candidate per column.
		pop = dMat(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda());
    
		for(int i=0; i< CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda(); i++) {
			pop.col(i) = _sdimstrat->bestCandidate(
				CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver.samples_ind(
					_hp.K(),
					CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma
				)
			);
		}
	}
};
}

#endif

