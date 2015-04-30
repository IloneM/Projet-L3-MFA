#include "rpcmabig.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <opti_err.h>
#include <limits>
#include <omp.h>                // include OpenMP
#include <cmath>
#include "functions.h"
#define NB_TEST 10

using namespace libcmaes;

int main(int argc, char* argv[]) {

	//srand((unsigned int) time(NULL));

	int dim = 1000; // problem dimensions.
	std::vector<double> x0(dim,0.0);
	double sigma = 0.1;


	//int lambda = 100; // offsprings at each generation.
	CMAParameters<> cmaparams(dim,&x0.front(),sigma);
	cmaparams.set_str_algo("acmaes");
	//cmaparams.set_str_algo("sepacmaes");
	//cmaparams.set_fplot("./plot.txt");

	ESOptimizer<RPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);
	//ESOptimizer<CMAStrategy<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);
	//instance.set_progress_func(CMAStrategy<CovarianceUpdate>::_defaultPFunc);
	//instance.set_plot_func(CMAStrategy<CovarianceUpdate>::_defaultFPFunc);

	if(instance.optimize() == OPTI_SUCCESS) {
		//std::cout << instance.get_solutions() << "\n";
		std::cout
		<< "best solution => f-value=" << instance.get_solutions().best_candidate().get_fvalue()
		<< " / fevals=" << instance.get_solutions().nevals()
		<< " / sigma=" << instance.get_solutions().sigma()
		<< " / iter=" << instance.get_solutions().niter()
		<< " / elaps=" << instance.get_solutions().elapsed_time() << "ms\n";
	} else {
		std::cout << "errors\n";
	}
/*
	double time;
	double f_value;
	int n_iter;

	double min=std::numeric_limits<double>::max();

#pragma omp parallel for schedule(dynamic) reduction(+:time,n_iter) reduction(*:f_value)
	for(int i=0; i< NB_TEST; i++) {
//#pragma omp critical                        // because stdout is a shared zone
//		{
//			std::cout << i << "\n";
//		}

		CMAParameters<> cmaparams(dim,&x0.front(),sigma);
		//cmaparams.set_str_algo("sepacmaes");
		//if (!cmaparams.is_sep())
		//	cmaparams.set_sep();
		//ESOptimizer<CMAStrategy<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(fsphere,cmaparams);
		ESOptimizer<RPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(fsphere,cmaparams);
		instance.set_progress_func(CMAStrategy<CovarianceUpdate>::_defaultPFunc);
		instance.set_plot_func(CMAStrategy<CovarianceUpdate>::_defaultFPFunc);

		if(instance.optimize() == OPTI_SUCCESS) {
			time += instance.get_solutions().elapsed_time();
			f_value *= pow(instance.get_solutions().best_candidate().get_fvalue(),1.0/NB_TEST);
			n_iter += instance.get_solutions().niter();
#pragma omp critical                        // because stdout is a shared zone
				{
					std::cout //<< i << "\n"
					//<< "optim result: " << instance.optimize() << "\n"
					<< "best solution => f-value=" << instance.get_solutions().best_candidate().get_fvalue()
					<< " / fevals=" << instance.get_solutions().nevals()
					<< " / sigma=" << instance.get_solutions().sigma()
					<< " / iter=" << instance.get_solutions().niter()
					<< " / elaps=" << instance.get_solutions().elapsed_time() << "ms\n";
					if(min > instance.get_solutions().best_candidate().get_fvalue())
						min = instance.get_solutions().best_candidate().get_fvalue();
				}
		} else {
			//i--;
		}

	}

	std::cout	<< "Elapsed time:\n\t-total: " << time << "\n\t-mean: " << time / NB_TEST << "\n"
				<< "Nb iter:\n\t-total: " << n_iter << "\n\t-mean: " << (double)n_iter / NB_TEST << "\n"
				<< "Func value:\n\t-min: " << min << "\n\t-mean (geom): " << f_value << "\n";
*/
	return 0;
}

