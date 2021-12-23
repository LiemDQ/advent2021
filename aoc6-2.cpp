#include <fstream>
#include <numeric>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <string>

std::array<long long, 9> partition_fishes(const std::string& s, char delim){
	std::array<long long, 9> result{0};
	std::stringstream ss(s);
	std::string item;
	while(std::getline(ss, item, delim)){
		result[std::stoi(item)]++;
	}
	return result;
}

constexpr int NUM_DAYS = 256;

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string inputs;
	std::getline(file, inputs);
	
	std::array<long long, 9> lf_bins = partition_fishes(inputs, ',');
	for (int i = NUM_DAYS; i > 0; i--){
		long long new_fishes = lf_bins[0];
		for (int j = 0; j < lf_bins.size() - 1; j++){
			lf_bins[j] = lf_bins[j+1];
		}
		lf_bins[6] += new_fishes;
		lf_bins[8] = new_fishes;
	}

	long long total_fishes = std::accumulate(lf_bins.begin(), lf_bins.end(), (long long) 0);
	std::cout << "Number of lanternfishes is: " << total_fishes << std::endl;

	return 0;
}