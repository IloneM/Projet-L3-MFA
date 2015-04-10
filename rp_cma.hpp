#ifndef RP_CMA_H
#define RP_CMA_H

#include <Eigen/Eigen>
#include <eigenmvn.h>
#include <cmaes.h>

/*
 * DISCLAMER: This class may be useless and is implemented only in case of potential uses in the future
 */

using namespace libcmaes;

class RP_CMA : public CMAStrategy<CovarianceUpdate> {};

#endif
