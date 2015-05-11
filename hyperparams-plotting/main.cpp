#include "ultraparameters.h"
#include <json/json.h>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>

#define TYPES {"mean-fvalue"}
#define NB_TYPES 2
#define MEAN_ID_LEN 5 //length of mean-

int main() {
		const std::string data_path("datas/");
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
			
//		Json::FastWriter json_writer;//prevents to write a big file with a lot of newlines
		std::fstream json_file;

		for(int k=MIN_K; k<= MAX_K; k+=STEP_K) {
			for(int d=MIN_d; d<= MAX_d; d+=STEP_d) {
				json_file.open(data_path + "k" + std::to_string(k) + "_d" + std::to_string(d) + ".json", std::ios_base::in);
				if(json_file.is_open()) {
					std::cout << "parsing k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";

					Json::Value result;
					json_file >> result;
		
					double res = 1;

					for(int i=0; i< NB_TEST/10; i++) {
						double tmp = result[std::to_string(i*10)]["fvalue"].asDouble();
						for(int j=1; j< 10; j++) {
							tmp *= result[std::to_string(i*10+j)]["fvalue"].asDouble();
						}
						res *= std::pow(tmp, 1.0/10);
					}
					res = std::pow(res, 10.0/NB_TEST);

					gnuplot_files[0] << (k-MIN_K)/STEP_K + 1 << " " << (d-MIN_d)/STEP_d + 1 << " " << res << '\n';

					json_file.close();
/*
					json_file.open(data_path + "k" + std::to_string(k) + "_d" + std::to_string(d) + ".json", std::ios_base::out | std::ios_base::trunc);
					if(json_file.is_open()) {
						result["global"]["fvalue"] = res;
						json_file << json_writer.write(result);
						json_file.close();
					} else {
						std::cerr << "error: cannot open file k" << std::to_string(k) << "_d" << std::to_string(d) << ".json for writing\n";
					}
*/
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

