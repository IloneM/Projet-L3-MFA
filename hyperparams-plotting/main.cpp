#include "ultraparameters.h"
#include <json/json.h>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>

#define TYPES {"elasped", "mean-elasped"}//"niter", "fvalue", "fevals", "mean-elapsed", "mean-niter", "mean-fvalue", "mean-fevals", "max", "min"}
#define NB_TYPES 2
#define MEAN_ID_LEN 5 //length of mean-

int main() {
		const std::string data_path("../test-hyperparams/datas/");
		const std::string plots_path("plots/");

		const std::string data_types[NB_TYPES] = TYPES;

		std::ofstream gnuplot_files[NB_TYPES];
		for(int i=0; i< NB_TYPES; i++) {
			gnuplot_files[i].open(plots_path + data_types[i] + ".dat");
			if(!gnuplot_files[i].is_open()) {
				std::cerr << "error: cannot open file " << data_types[i] << ".dat";
				return 0;
			}
			gnuplot_files[i] << "//X,Y,Z\n";
		}
			
		std::ifstream json_file;
		double res;

		for(int k=MIN_K; k<= MAX_K; k+=STEP_K) {
			for(int d=MIN_d; d<= MAX_d; d+=STEP_d) {
				json_file.open(data_path + "k" + std::to_string(k) + "_d" + std::to_string(d) + ".json");
				if(json_file.is_open()) {
					std::cout << "parsing k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";

					Json::Value result;
					json_file >> result;
		
					//_Z[(k-MIN_K)/STEP_K][(d-MIN_d)/STEP_d] = 1.0;//result["global"]["min"].asDouble();
					for(int i=0; i< NB_TYPES; i++) {
						std::string fc = data_types[i].substr(0,MEAN_ID_LEN);
						
						if(fc == "mean-") {
							fc = data_types[i].substr(MEAN_ID_LEN);

							res = result["global"][fc].asDouble();

							if(fc == "fvalue")
								res = std::pow(res, 1.0/NB_TEST);
							else 
								res /= NB_TEST;

						} else {
							res = result["global"][data_types[i]].asDouble();
						}


						//res = result["global"][data_types[i]].asDouble();

						gnuplot_files[i] << (k-MIN_K)/STEP_K + 1 << " " << (d-MIN_d)/STEP_d + 1 << " " << res << '\n';
					}
		/*
						Json::CharReaderBuilder rbuilder;
						std::string errs;
						bool parsingSuccessful = Json::parseFromStream(rbuilder, json_file, &result, &errs);
						if (!parsingSuccessful) {
						// report to the user the failure and their locations in the document.
							std::cout  << "Failed to parse configuration\n" << errs;
						} else {
								
						}
		*/
					json_file.close();
				} else {
					std::cerr << "error: cannot open file k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";
				}
			}
			for(int i=0; i< NB_TYPES; i++) {
				gnuplot_files[i] << '\n';
			}
		}
		for(int i=0; i< NB_TYPES; i++) {
			gnuplot_files[i].close();
		}
    return 0;
}

