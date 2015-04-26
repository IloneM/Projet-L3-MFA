#include <cmaes.h>
#include <iostream>
#include <opti_err.h>

using namespace libcmaes;

FitFunc fsphere = [](const double *x, const int N)
{
  double val = 0.0;
  for (int i=0;i<N;i++)
    val += x[i]*x[i];
  return val;
};

int main(int argc, char* argv[]) {

	int dim = 1000; // problem dimensions.
	std::vector<double> x0(dim,10.0);
	double sigma = 0.1;
	//int lambda = 100; // offsprings at each generation.
	CMAParameters<> cmaparams(dim,&x0.front(),sigma);

	cmaparams.set_str_algo("sepacmaes");

	CMASolutions cmasols = cmaes<>(fsphere,cmaparams);

	std::cout << cmasols << "\n";

	return 0;
}

