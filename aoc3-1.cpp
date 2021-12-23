#include <cstdlib>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

constexpr int num_bits = 12; 

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	int n_lines = 0;

	int count[num_bits] = {};

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string line;
	while (std::getline(file, line)){
		//std::cout << line << std::endl;
		for (int i = 0; i < num_bits; i++){
			if (line.at(i) == '0'){
				continue;
			} else if (line.at(i) == '1'){
				//std::cout << line << " - " << i << std::endl;
				count[i]++;
			} else {
				std::cout << "Warning: this path should not be reachable" << std::endl;
			}
		}
		n_lines++;
	}
	

	int gamma_rate, eps_rate = 0;
	for (int j = 0; j < num_bits; j++ ){
		std::cout << count[j] << std::endl;
		if (count[j] > n_lines/2){
			int bit = 1 << (num_bits-1) >> j;
			gamma_rate += bit;
		}
	}

	int mask = 0xFFF;
	eps_rate = gamma_rate^mask;

	std::cout << "Gamma rate: " << gamma_rate << std::endl;
	std::cout << "Epsilon rate: " << eps_rate << std::endl;
	std::cout << "Power consumption: " << gamma_rate*eps_rate << std::endl;

	return 0;

}