#include "rp_cma.hpp"
#include <iostream>

double testFunc(Eigen::RowVectorXd v) {
	return v.norm();
}

int main(int argc, char* argv[]) {

	RP_CMA instance(10,2, testFunc);

	instance.run();

	return 0;
}

