#ifndef RP_CMA_SMALL_H
#define RP_CMA_SMALL_H

//#include "abstractrpcma.h"
#include <cmaes.h>
#include "rpcmabig.h"

#define PRECISION 10e-6

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>> class RPCMABig;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMASmall : public CMAStrategy<TCovarianceUpdate,TGenoPheno>//AbstractRPCMA<TCovarianceUpdate,TGenoPheno>
{
protected:

	RPCMABig<TCovarianceUpdate,TGenoPheno>* _bdimstrat;

	dMat _randProjection;

	dMat _covarianceInverse;

public:

	inline dMat randProjection() {
		return _randProjection;
	}

	inline double propFitness(dVec X) {//X belongs to big space
		return _covarianceInverse * (_randPorjection * X - _esolver.mean())
	}

	void setCovarianceInverse(dMat newMatrix) {
		_covarianceUpdate = ;
	}

      /**
       * \brief dummy constructor
       */
      RPCMASmall(RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat,
					  dMat randProjection) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(), _bdimstrat(bdimstrat), _randProjection(randProjection) {}
    
      /**
       * \brief constructor.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       */
      RPCMASmall(RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat,
			dMat randProjection,
			FitFunc &func,
			CMAParameters<TGenoPheno> &parameters) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, parameters), _bdimstrat(bdimstrat), _randProjection(randProjection) {}

      /**
       * \brief constructor for starting from an existing solution.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       * @param cmasols solution object to start from
       */
      RPCMASmall(RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat,
		  dMat randProjection,
		  FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters,
		  const CMASolutions &cmasols) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, parameters, cmasols), _bdimstrat(bdimstrat), _randProjection(randProjection) {}

	dMat ask() {
		return _randProjection *  _bdimstrat->candidates();
	}

/*    
	  ~RPCMASmall() {
	  }
*/
};

#endif

