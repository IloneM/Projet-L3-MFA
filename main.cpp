#include "rpcmabig.h"
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
	cmaparams.set_str_algo("acmaes");
	//cmaparams.set_str_algo("sepacmaes");
	//cmaparams.set_fplot("./plot.txt");

	ESOptimizer<RPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);
	//instance.set_progress_func(CMAStrategy<CovarianceUpdate>::_defaultPFunc);
	//instance.set_plot_func(CMAStrategy<CovarianceUpdate>::_defaultFPFunc);

	if(instance.optimize() == OPTI_SUCCESS) {
		std::cout << instance.get_solutions() << "\n";
	} else {
		std::cout << "errors\n";
	}

	return 0;
}

