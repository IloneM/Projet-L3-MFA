#include "../rpcmabig.h"
#include <iostream>
#include <opti_err.h>
#include <omp.h>                // include OpenMP
#include <cmath>
#include <string>
#include <fstream>
#include <json/json.h>
#include <limits>
#include "../functions.h"
#include "ultraparameters-special-bis.h"

using namespace libcmaes;

int main(int argc, char* argv[]) {

	int dim = 1000; // problem dimensions.
	std::vector<double> x0(dim,0.0);
	double sigma = 0.1;

	Json::FastWriter json_writer;//prevents to write a big file with a lot of newlines
	std::ofstream json_file;
	const std::string data_path("datas/");

//#pragma omp parallel for schedule(dynamic)
for(int k=MIN_K; k<= MAX_K; k+=STEP_K) {
//#pragma omp parallel for schedule(dynamic)
  for(int d=MIN_d; d<= MAX_d; d+=STEP_d) {
	HyperParameters::_k = k;
	HyperParameters::_d = d;

	double time		= 0.0;
	double fvalue	= 1.0;
	int niter		= 0.0;
	int fevals		= 0.0;

	double min=std::numeric_limits<double>::max();
	double max=std::numeric_limits<double>::min();
	
	Json::Value root;

#pragma omp parallel for schedule(dynamic) reduction(+:time,niter,fevals) reduction(*:fvalue)
	for(int i=0; i< NB_TEST; i++) {
		CMAParameters<> cmaparams(dim,&x0.front(),sigma);

		ESOptimizer<RPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);

		if(instance.optimize() == OPTI_SUCCESS) {
			time	+= instance.get_solutions().elapsed_time();
			fvalue	*= instance.get_solutions().best_candidate().get_fvalue();
			niter	+= instance.get_solutions().niter();
			fevals	+= instance.get_solutions().nevals();
			Json::Value result;
			result["fvalue"]	= instance.get_solutions().best_candidate().get_fvalue();
			result["fevals"]	= instance.get_solutions().nevals();
			result["sigma"]		= instance.get_solutions().sigma();
			result["iter"]		= instance.get_solutions().niter();
			result["elaps"]		= instance.get_solutions().elapsed_time();

			dVec X = instance.get_parameters().get_gp().pheno(instance.get_solutions().best_candidate().get_x_dvec());
			Json::Value X_json;
			for(int j=0; j< X.size(); j++) {
				X_json[j] = X(j);
			}
			result["X"] = X_json;

#pragma omp critical                        // because result is a shared zone
				{
					root[std::to_string(i)] = result;//use string key for parallelization purpose

					if(min > instance.get_solutions().best_candidate().get_fvalue()) {
						min = instance.get_solutions().best_candidate().get_fvalue();
					}

					if(max < instance.get_solutions().best_candidate().get_fvalue()) {
						max = instance.get_solutions().best_candidate().get_fvalue();
					}

					std::cout << "K: " << k << " d: " << d << " sample: " << i << "\n";
				}
		} else {
#pragma omp critical                        // because i is a shared zone
		  {
			std::cerr << "optimization failed for sample " << i << " with parameters K=" << k << ", d=" << d << "\n";
			i--;
		  }
		}
	}
//#pragma omp critical                        // because stdout is a shared zone
	Json::Value global_result;
	global_result["elasped"] = time;
	global_result["niter"] = niter;
	global_result["fvalue"] = fvalue;
	global_result["fevals"] = fevals;
	global_result["min"] = min;
	global_result["max"] = max;

	root["global"] = global_result;

	json_file.open(data_path + "k" + std::to_string(k) + "_d" + std::to_string(d) + ".json");
	if(json_file.is_open()) {
		json_file << json_writer.write(root);
	} else {
		std::cerr << "error: cannot open file data.json\n";
	}
	json_file.close();
  }
}

	return 0;
}

