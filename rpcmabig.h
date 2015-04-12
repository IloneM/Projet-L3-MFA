#ifndef RP_CMA_BIG_H
#define RP_CMA_BIG_H

#include "abstractrpcma.h"

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMABig : public AbstractRPCMA<TCovarianceUpdate,TGenoPheno>
{
protected:

int _sdim;

public:

      /**
       * \brief dummy constructor
       */
      RPCMABig() : AbstractRPCMA<TCovarianceUpdate,TGenoPheno>(), _sdim(0) {}
    
      /**
       * \brief constructor.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       */
      RPCMABig(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters) : AbstractRPCMA<TCovarianceUpdate,TGenoPheno>(func, parameters), _sdim(SMALL_DIM) {}

      /**
       * \brief constructor for starting from an existing solution.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       * @param cmasols solution object to start from
       */
      RPCMABig(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters,
		  const CMASolutions &cmasols) : AbstractRPCMA<TCovarianceUpdate,TGenoPheno>(func, parameters, cmasols), _sdim(SMALL_DIM) {}
    
	  ~RPCMABig() {
			  ~AbstractRPCMA<TCovarianceUpdate,TGenoPheno>();
	  }
};

#endif

