#include <cmaes.h>
#include <iostream>
#include <opti_err.h>
#include "../functions.h"

using namespace libcmaes;

int main(int argc, char* argv[]) {

	int dim = 50; // problem dimensions.
	std::vector<double> x0(dim,10.0);
	double sigma = 0.1;
	//int lambda = 100; // offsprings at each generation.
	CMAParameters<> cmaparams(dim,&x0.front(),sigma);

	cmaparams.set_str_algo("acmaes");

	CMASolutions cmasols = cmaes<>(rosenbrock,cmaparams);

	std::cout << cmasols << "\n";

	return 0;
}

