#ifndef ABSTRACT_RP_CMA_H
#define ABSTRACT_RP_CMA_H

//#include <Eigen/Eigen>
//#include <eigenmvn.h>
#include <cmaes.h>

/*
 * DISCLAMER: This class may be useless and is implemented only in case of potential uses in the future
 */

using namespace libcmaes;

template <class TCovarianceUpdate,class TGenoPheno=GenoPheno<NoBoundStrategy>>
class AbstractRPCMA : public CMAStrategy<TCovarianceUpdate,TGenoPheno> {};

#endif

