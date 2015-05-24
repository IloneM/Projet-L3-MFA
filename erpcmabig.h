#ifndef ERP_CMA_BIG_H
#define ERP_CMA_BIG_H

#include <cmaes.h>
#include <vector>
#include <memory>
#include <random>
#include "erpcmasmall.h"

/*
 * TODO:
 * Important:
 * -Make ERPCMABig and RPCMABIG having common ancester
 * -Rebuild tell in (E)RPCMASmall in way to prevent sorting candiates again and delete build_fvalue in library
 * -Implement chooseSDimStrat and update_weight (and see if this last must be implemented in ERPCMABig or Small)
 *
 * Secondary:
 * -See wich call orders are the best in tell, ask, eval
 * -Add warnings for calls to sdimstrat methods orders (see above)
 */

namespace libcmaes
{

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class ERPCMABig : public CMAStrategy<TCovarianceUpdate,TGenoPheno>
{
protected:

	//small dimension strategy
	std::vector<std::shared_ptr<ERPCMASmall<ERPCMABig<TCovarianceUpdate,TGenoPheno>,TCovarianceUpdate,TGenoPheno>>> _sdimstrats;

	//The sum of all the weights of sdim_strats
	double _totalWeights;

	//the generator used for selecting the small strategy to use
	std::mt19937 _gen;

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

	unsigned int setupGenerator() {
		std::random_device rd;
		return rd();
	}

    /**
     * \brief Build a new Small dimension strategy with dimension of d and sparams parameters
     * @param original parameters usually set by user
     * @param the new dimension
     */
	std::shared_ptr<ERPCMASmall<ERPCMABig<TCovarianceUpdate,TGenoPheno>,TCovarianceUpdate,TGenoPheno>> setupSDimStrat(CMAParameters<TGenoPheno>& sparams, const int& d, const double weight) {
		return std::make_shared<ERPCMASmall<ERPCMABig<TCovarianceUpdate,TGenoPheno>,TCovarianceUpdate,TGenoPheno>>(setupDim(sparams, d), this, weight);
	}

	std::shared_ptr<ERPCMASmall<ERPCMABig<TCovarianceUpdate,TGenoPheno>,TCovarianceUpdate,TGenoPheno>> chooseSDimStrat() {
		std::uniform_real_distribution <double> uni(0.0,_totalWeights);
		double res;

		//For the very unprobable res == _totalWeights case
		do {		
			res = uni(_gen);
		} while(res >= _totalWeights);

		for(int i=0; i< _sdimstrats.size(); i++) {
			res -= _sdimstrats[i]->weight();
			if(res < 0) {
				std::cout << "choosed is " << i << '\n';
				return _sdimstrats[i];
			}
		}
		return _sdimstrats[_sdimstrats.size()-1];//if not returned is cause of approximations errors
/*
		//in the very unprobable case (i.e. res == _totalWeights case) which none has been selected, select it randmoly without considering weights
		std::uniform_int_distribution<int> uni_int(0, _sdimstrats.size()-1);
		return _sdimstrats[uni_int(gen)];
*/
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
    ERPCMABig(FitFunc &func,
	  CMAParameters<TGenoPheno> &bparams) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, setupParameters(bparams)), _sdimstrats(_hp.M()), _totalWeights(1.0), _gen(setupGenerator()) {
		
		for(int i=0; i< _sdimstrats.size(); i++) {
			_sdimstrats[i] = setupSDimStrat(*_backupParams, _hp.d(), 1.0/_hp.M());
		}
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
	~ERPCMABig() {
		delete _backupParams;
	}

	virtual dMat ask() {
		for(int i=0; i< _sdimstrats.size(); i++) {
			_sdimstrats[i]->ask();
		}
		return CMAStrategy<TCovarianceUpdate,TGenoPheno>::ask();
	}

	virtual void build_pop(dMat& pop) {
		std::shared_ptr<ERPCMASmall<ERPCMABig<TCovarianceUpdate,TGenoPheno>,TCovarianceUpdate,TGenoPheno>> sdimstrat = chooseSDimStrat();

		// sample for multivariate normal distribution, produces one candidate per column.
		pop = dMat(CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.dim(), CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda());
    
		for(int i=0; i< CMAStrategy<TCovarianceUpdate,TGenoPheno>::_parameters.lambda(); i++) {
			pop.col(i) = sdimstrat->bestCandidate(
				CMAStrategy<TCovarianceUpdate,TGenoPheno>::_esolver.samples_ind(
					_hp.K(),
					CMAStrategy<TCovarianceUpdate,TGenoPheno>::_solutions._sigma
				)
			);
		}
	}

	//rewrite erpcmasmall tell for preventing the resorting of solutions
	virtual void tell() {
		CMAStrategy<TCovarianceUpdate,TGenoPheno>::tell();
		for(int i=0; i< _sdimstrats.size(); i++) {
			_sdimstrats[i]->eval();
			_sdimstrats[i]->tell();
			_totalWeights += _sdimstrats[i]->update_weight();
		}
	}

	virtual bool stop() {
		bool res_big = CMAStrategy<TCovarianceUpdate,TGenoPheno>::stop();
		bool res_small;
//		std::cout << "Big is " << res_big << '\n';
		for(int i=0; i< _sdimstrats.size(); i++) {
			res_small = _sdimstrats[i]->stop();
//			std::cout << "Small " << i << " is " << res_small << '\n';
			res_big &= res_small;
		}
//		std::cout << CMAStrategy<TCovarianceUpdate,TGenoPheno>::get_solutions().get_best_seen_candidate().get_fvalue() << '\n';
		return res_big;
	}
};
}

#endif

