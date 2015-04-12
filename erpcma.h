#ifndef RP_CMA_MCP_H
#define RP_CMA_MCP_H

#include "abstractrpcma.h"

#define SMALL_DIM 75

/*
 * Importants points procedures seems to be eval for N_D and ask for n_d.
 * A overall class must be implemented
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class RPCMAMcp : public AbstractRPCMA<TCovarianceUpdate,TGenoPheno>
{

//general space properties

protected:

//unsigned int m_D;//size of the big space ## EDIT: in parameters
int _sdim;//size of the "small space" (i.e. d<<D)
//unsigned int m_K;//number of samples

public:

RP_CMA(FitFunc &func, CMAParameters<> &parameters) : AbstractRPCMA(func,parameters) {}

      /**
       * \brief dummy constructor
       */
      RPCMAMcp() : AbstractRPCMA<TCovarianceUpdate,TGenoPheno>(), _sdim(0) {}
    
      /**
       * \brief constructor.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       */
      RPCMAMcp(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters) : AbstractRPCMA<TCovarianceUpdate,TGenoPheno>(func, parameters), _sdim(SMALL_DIM) {}

      /**
       * \brief constructor for starting from an existing solution.
       * @param func objective function to minimize
       * @param parameters stochastic search parameters
       * @param cmasols solution object to start from
       */
      RPCMAMcp(FitFunc &func,
		  CMAParameters<TGenoPheno> &parameters,
		  const CMASolutions &cmasols) : AbstractRPCMA<TCovarianceUpdate,TGenoPheno>(func, parameters, cmasols), _sdim(SMALL_DIM) {}
    
	  ~RPCMAMcp() {
			  ~AbstractRPCMA<TCovarianceUpdate,TGenoPheno>();
	  };

};

#endif
