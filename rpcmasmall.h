#ifndef RP_CMA_SMALL_H
#define RP_CMA_SMALL_H

//#include "abstractrpcma.h"
#include <cmaes.h>
#include "rpcmabig.h"

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

public:

      /**
       * \brief dummy constructor
       */
      RPCMASmall(RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(), _bdimstrat(bdimstrat) {}
    
      /**
       * \brief constructor.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       */
      RPCMASmall(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters,
		  RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, parameters), _bdimstrat(bdimstrat) {}

      /**
       * \brief constructor for starting from an existing solution.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       * @param cmasols solution object to start from
       */
      RPCMASmall(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters,
		  const CMASolutions &cmasols,
		  RPCMABig<TCovarianceUpdate,TGenoPheno>* bdimstrat) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, parameters, cmasols), _bdimstrat(bdimstrat) {}

	dMat ask() {
//		std::cout << "big Mat: -cols: " << _bdimstrat->candidates().cols() << "\n-rows: " << _bdimstrat->candidates().rows() << "\nproj: -cols: " << _bdimstrat->randProjection().cols() << "-rows: " << _bdimstrat->randProjection().rows();
		return _bdimstrat->randProjection() *  _bdimstrat->candidates();
	}

/*    
	  ~RPCMASmall() {
	  }
*/
};

#endif

