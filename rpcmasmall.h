#ifndef RP_CMA_SMALL_H
#define RP_CMA_SMALL_H

//#include "abstractrpcma.h"
#include <cmaes.h>

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMASmall : public CMAStrategy<TCovarianceUpdate,TGenoPheno>//AbstractRPCMA<TCovarianceUpdate,TGenoPheno>
{
public:

      /**
       * \brief dummy constructor
       */
      RPCMASmall() : CMAStrategy<TCovarianceUpdate,TGenoPheno>() {}
    
      /**
       * \brief constructor.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       */
      RPCMASmall(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, parameters) {}

      /**
       * \brief constructor for starting from an existing solution.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       * @param cmasols solution object to start from
       */
      RPCMASmall(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters,
		  const CMASolutions &cmasols) : CMAStrategy<TCovarianceUpdate,TGenoPheno>(func, parameters, cmasols) {}
/*    
	  ~RPCMASmall() {
	  }
*/
};

#endif

