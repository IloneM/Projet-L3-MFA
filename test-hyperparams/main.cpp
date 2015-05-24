#include "../rpcmabig.h"
#include <iostream>
#include <opti_err.h>
#include <omp.h>                // include OpenMP
#include <string>
#include <fstream>
#include <json/json.h>
#include <limits>
#include "../functions.h"
//#include "ultraparameters-small.h"
#include "ultraparameters-big-ni.h"
//#include "ultraparameters-big-gui.h"

using namespace libcmaes;

int main(int argc, char* argv[]) {

	int dim = 1000; // problem dimensions.
	std::vector<double> x0(dim,0.0);
	double sigma = 0.1;

	Json::FastWriter json_writer;//prevents to write a big file with a lot of newlines
	std::ofstream json_file;
	std::fstream conf_file;
	const std::string data_path("datas/");

	bool init = true;

for(int spc=MIN_spc; spc<= MAX_spc; spc+=STEP_spc) {
 for(int k=MIN_K; k<= MAX_K; k+=STEP_K) {
  for(int d=MIN_d; d<= MAX_d; d+=STEP_d) {

	HyperParameters::_K = k;
	HyperParameters::_d = d;
	HyperParameters::_spc = spc/100.0;
	
	Json::Value config;
	
	if(init) {
		conf_file.open(data_path + "progress.json", std::fstream::in);
		if(conf_file.is_open()) {
			conf_file >> config;
			k = config["k"].asInt();
			d = config["d"].asInt();
			spc = config["spc"].asInt();

			HyperParameters::_K = k;
			HyperParameters::_d = d;
			HyperParameters::_spc = spc/100.0;
		}
		conf_file.close();
		init = false;
	} else {	
		config["k"] = k;
		config["d"] = d;
		config["spc"] = spc;

		conf_file.open(data_path + "progress.json", std::fstream::out | std::fstream::trunc);
		if(conf_file.is_open()) {
			conf_file << json_writer.write(config);
		} else {
			std::cerr << "error: cannot open file progress.json\n";
		}
		conf_file.close();
	}

	Json::Value root;

#pragma omp parallel for schedule(dynamic)
	for(int i=0; i< NB_TEST; i++) {
		CMAParameters<> cmaparams(dim,&x0.front(),sigma);

		ESOptimizer<RPCMABig<CovarianceUpdate>,CMAParameters<>,CMASolutions> instance(rosenbrock,cmaparams);

		if(instance.optimize() == OPTI_SUCCESS) {
			Json::Value result;
			result["fvalue"]	= instance.get_solutions().get_best_seen_candidate().get_fvalue();
			result["fevals"]	= instance.get_solutions().nevals();
			result["sigma"]		= instance.get_solutions().sigma();
			result["iter"]		= instance.get_solutions().niter();
			result["elaps"]		= instance.get_solutions().elapsed_time();

			dVec X = instance.get_parameters().get_gp().pheno(instance.get_solutions().get_best_seen_candidate().get_x_dvec());
			Json::Value X_json;
			for(int j=0; j< X.size(); j++) {
				X_json[j] = X(j);
			}
			result["X"] = X_json;

#pragma omp critical                        // because root is a shared zone
				{
					root[std::to_string(i)] = result;//use string key for parallelization purpose

					std::cout << "spc: " << spc << " K: " << k << " d: " << d << " sample: " << i << "\n";
				}
		} else {
#pragma omp critical                        // because i is a shared zone
		  {
			std::cerr << "optimization failed for sample " << i << " with parameters spc=" << spc << ", K=" << k << ", d=" << d << "\n";
			i--;
		  }
		}
	}

	json_file.open(data_path + "spc" + std::to_string(spc) + "_k" + std::to_string(k) + "_d" + std::to_string(d) + ".json");
	if(json_file.is_open()) {
		json_file << json_writer.write(root);
	} else {
		std::cerr << "error: cannot open file spc" << spc << "_k" << k << "_d" << d << ".json\n";
	}
	json_file.close();
  }
 }
}

	return 0;
}

