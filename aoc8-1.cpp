#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>


struct Digits {
	std::string one;
	std::string two;
	std::string three;
	std::string four;
	std::string five;
	std::string six;
	std::string seven;
	std::string eight;
	std::string nine;

	std::vector<std::string> output;
};

int solve_easy(Digits& digits){
	int count = 0;
	for (auto&& s:digits.output){
		if (s == digits.one || s == digits.four || s == digits.seven || s == digits.eight){
			count++;
		}
	}
	return count;
}

Digits parse_input(const std::string& s){
	std::stringstream ss(s);
	std::string item;
	Digits digits;
	std::stringstream combinations(s.substr(0, 59));
	std::vector<std::string> inputs;

	std::stringstream output_stream(s.substr(61));
	std::vector<std::string> outputs;

	while(std::getline(combinations, item, ' ')){
		std::sort(item.begin(), item.end());
		inputs.push_back(item);
	}

	while (std::getline(output_stream, item, ' ')){
		std::sort(item.begin(), item.end());
		outputs.push_back(item);
	}
	digits.output = outputs;
	digits.one = *std::find_if(inputs.begin(), inputs.end(), [](std::string& s){return s.size() == 2; });
	digits.four = *std::find_if(inputs.begin(), inputs.end(), [](std::string& s){return s.size() == 4; });
	digits.seven = *std::find_if(inputs.begin(), inputs.end(), [](std::string& s){return s.size() == 3; });
	digits.eight = *std::find_if(inputs.begin(), inputs.end(), [](std::string& s){return s.size() == 7; });


	return digits;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string inputs;
	std::vector<Digits> digit_combos;
	while(std::getline(file, inputs)){
		digit_combos.push_back(parse_input(inputs));
	}

	int easy_count = std::accumulate(digit_combos.begin(), digit_combos.end(), 0, 
		[](int accum, Digits& dig){return accum + solve_easy(dig);});
	
	std::cout << "Number of 1's, 4's, 7's and 8's: " << easy_count << std::endl;
	
	return 0;
}