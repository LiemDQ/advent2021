#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <limits>
#include <fstream>
#include <iostream>

std::vector<int> split_into_int(const std::string& s, char delim){
	std::vector<int> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)){
		result.push_back(std::stoi(item));
	}

	return result;
}


int consecutive_sum(int n){
	return n*(n+1)/2;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string inputs;
	std::getline(file, inputs);
	std::vector<int> crabs = split_into_int(inputs, ',');

	int min_pos = *std::min_element(crabs.begin(), crabs.end());
	int max_pos = *std::max_element(crabs.begin(), crabs.end());
	std::cout << "Minimum position: " << min_pos << " | Maximum positiion: " << max_pos << std::endl;
	int min_moves = std::numeric_limits<int>::max();
	for (int i = min_pos; i < max_pos; i++){
		int num_moves = std::accumulate(crabs.begin(), crabs.end(), 0, 
			[i](int nmoves, int crab_pos){ return nmoves + consecutive_sum(std::abs(crab_pos - i));});
		min_moves = std::min(min_moves, num_moves);
	}

	std::cout << "Min fuel spent is: " << min_moves << std::endl; 
	
	return 0;
}