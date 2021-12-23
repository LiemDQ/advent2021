#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <string>

std::vector<int> split_into_int(const std::string& s, char delim){
	std::vector<int> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)){
		result.push_back(std::stoi(item));
	}
	return result;
}

constexpr int NUM_DAYS = 80;

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string inputs;
	std::getline(file, inputs);
	std::vector<int> lanternfishes = split_into_int(inputs, ',');
	std::vector<int> new_lanternfishes;
	lanternfishes.reserve(10000);
	
	for (int i = NUM_DAYS; i > 0; i--){
		//std::cout << "Num days left: " << NUM_DAYS << std::endl;
		//std::cout << "Number of fishies: " << lanternfishes.size() << std::endl;
		for (auto& fish:lanternfishes){
			if (--fish < 0){
				fish = 6;
				new_lanternfishes.push_back(8);
			}
		}
		lanternfishes.insert(lanternfishes.end(), new_lanternfishes.begin(), new_lanternfishes.end());
		new_lanternfishes.erase(new_lanternfishes.begin(), new_lanternfishes.end());
	}

	std::cout << "NMumber of lanternfishes is: " << lanternfishes.size() << std::endl;

	return 0;
}