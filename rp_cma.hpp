#ifndef RP_CMA_H
#define RP_CMA_H

#include <Eigen/Eigen>
//#include <functional>
#include <eigenmvn.h>
#include <cmaes.h>

using namespace libcmaes;

class RP_CMA : public CMAStrategy<CovarianceUpdate>
{

//general space properties

protected:

unsigned int m_D;//size of the big space
unsigned int m_d;//size of the "small space" (i.e. d<<D)
unsigned int m_K;//number of samples

public:

RP_CMA(FitFunc &func, CMAParameters<> &parameters, unsigned int D, unsigned int d) : CMAStrategy<CovarianceUpdate>(func,parameters),
		m_d(d), m_D(D), m_K(10)/*to set*/
{
	
}

};

#endif
