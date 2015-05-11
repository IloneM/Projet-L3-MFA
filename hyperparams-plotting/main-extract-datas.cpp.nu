#include "ultraparameters.h"
#include <json/json.h>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {

	Json::FastWriter json_writer;//prevents to write a big file with a lot of newlines
	std::ifstream json_file;
	std::ofstream json_file_w;
	const std::string data_path("../test-hyperparams/datas/");
	const std::string data_path_w("datas/");

for(int k=MIN_K; k<= MAX_K; k+=STEP_K) {
  for(int d=MIN_d; d<= MAX_d; d+=STEP_d) {

	Json::Value root;
	Json::Value new_root;

	json_file.open(data_path + "k" + std::to_string(k) + "_d" + std::to_string(d) + ".json");
	if(json_file.is_open()) {
//		std::cout << "parsing k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";
		json_file >> root;
		json_file.close();
	} else {
		std::cerr << "error 1: cannot open file k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";
	}

	for(int i=0; i< NB_TEST; i++) {
		Json::Value result;
		result["fvalue"] = root[std::to_string(i)]["fvalue"];
		result["fevals"] = root[std::to_string(i)]["fevals"];
		result["sigma"] = root[std::to_string(i)]["sigma"];
		result["iter"]	= root[std::to_string(i)]["iter"];
		result["elaps"]	= root[std::to_string(i)]["elaps"];

		new_root[std::to_string(i)] = result;
	}

	new_root["global"] = root["global"];

	json_file_w.open(data_path_w + "k" + std::to_string(k) + "_d" + std::to_string(d) + ".json");
	if(json_file_w.is_open()) {
		std::cout << "parsing file k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";
		json_file_w << json_writer.write(new_root);
	} else {
		std::cerr << "error 2: cannot open file k" << std::to_string(k) << "_d" << std::to_string(d) << ".json\n";
	}
	json_file_w.close();
  }
}

	return 0;
}

