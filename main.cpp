#include "rpcmabig.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <opti_err.h>

using namespace libcmaes;

/*
double testFunc(Eigen::RowVectorXd v) {
	return v.norm();
}
*/

FitFunc fsphere = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += x[i]*x[i];
  return val;
};

int main(int argc, char* argv[]) {

	srand((unsigned int) time(NULL));

	int dim = 1000; // problem dimensions.
	std::vector<double> x0(dim,10.0);
	double sigma = 0.1;
	//int lambda = 100; // offsprings at each generation.
	CMAParameters<> cmaparams(dim,&x0.front(),sigma);

	RPCMABig<CovarianceUpdate> instance(fsphere, cmaparams);

	if(instance.optimize() == OPTI_SUCCESS) {
		std::cout << instance.get_solutions() << "\n";
	} else {
		std::cout << "errors occurs...\n";
	}

	return 0;
}

