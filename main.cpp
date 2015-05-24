#include "rpcmabig.h"
//#include "erpcmabig.h"
#include <iostream>
#include <opti_err.h>
#include "functions.h"

using namespace libcmaes;

int main(int argc, char* argv[]) {

	int dim = 1000; // problem dimensions.
	std::vector<double> x0(dim,0.0);
	double sigma = 0.1;


	//int lambda = 100; // offsprings at each generation.
	CMAParameters<> cmaparams(dim,&x0.front(),sigma);

	//ESOptimizer<ERPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);
	ESOptimizer<RPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);

	if(instance.optimize() == OPTI_SUCCESS) {
		std::cout << instance.get_solutions() << '\n';
	} else {
		std::cout << "errors\n";
	}

	return 0;
}

