#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <array>
#include <numeric>

struct DigitMap {
	int val;
	std::string str;
};

struct Digits {
	std::array<std::string, 10> map;

	std::vector<std::string> output;
};

template <typename V>
void print_vector(const V& vec){
	std::cout << "Vector contents: ";
	for (auto&& item:vec){
		std::cout << item << " | ";
	}
	std::cout << std::endl;
}

int solve_easy(Digits& digits){
	int count = 0;
	for (auto&& s:digits.output){
		if (s == digits.map[1] || s == digits.map[4] | s == digits.map[7] | s == digits.map[8]){
			count++;
		}
	}
	return count;
}

int solve_hard(Digits& digits){
	int result = 0;
	int multiplier = 1000;
	for (auto&& s:digits.output){
		for (int i = 0; i < digits.map.size(); i++){
			if (s == digits.map[i]){
				result += i*multiplier;
				break;
			}
		}
		multiplier /= 10;
	}
	return result;
}

bool str_contains(const std::string& main, const std::string& sub){
	//check if main string contains all chars in sub string
	for (auto&& s:sub){
		if (main.find(s) == main.npos){
			return false;
		}
	}
	return true;
}

Digits get_digit_combinations(const std::vector<std::string>& cmbs){
	using namespace std;
	Digits digits; 
	auto find_digit = [](const vector<string>& vec, auto&& predicate){
		return *find_if(vec.begin(), vec.end(), std::forward<decltype(predicate)>(predicate)); };
	//easy digits
	digits.map[1] = find_digit(cmbs, [](const string& s){return s.size() ==2;});
	digits.map[4] = find_digit(cmbs, [](const string& s){return s.size() ==4;});
	digits.map[7] = find_digit(cmbs, [](const string& s){return s.size() ==3;});
	digits.map[8] = find_digit(cmbs, [](const string& s){return s.size() ==7;});
	//std::cout << "Easy segments mapped." << std::endl;

	//six segment digits
	vector<string> six_digits;
	std::copy_if(cmbs.begin(), cmbs.end(), back_inserter(six_digits), [](const std::string& s){return s.size() == 6;});
	for (int i =0; i < six_digits.size(); i++){
		if (!str_contains(six_digits[i], digits.map[1])){
			digits.map[6] = six_digits[i];
		}
	}
	//std::cout << "Six done.";
	digits.map[0] = find_digit(six_digits, [digits](const string& s){
		return !str_contains(s, digits.map[4]) && s != digits.map[6];
	});
	digits.map[9] = find_digit(six_digits,
	 [digits](const string& s){ return !(s == digits.map[0] || s == digits.map[6]); });
	
	//std::cout << "Six digit segments mapped: 6 - " << digits.map[6] << "| 0 - " << digits.map[0] << "| 9 - " << digits.map[9] << std::endl;

	
	//five segment digits
	vector<string> five_digits;
	std::copy_if(cmbs.begin(),cmbs.end(), back_inserter(five_digits), [](const std::string& s){return s.size() == 5;});
	digits.map[3] = find_digit(five_digits, [digits](const string& s){ return str_contains(s, digits.map[1]); });
	digits.map[5] = find_digit(five_digits, [digits](const string& s){ return str_contains(digits.map[6], s); });
	digits.map[2] = find_digit(five_digits, 
	 [digits](const string& s){ return !(s == digits.map[3] || s == digits.map[5]); });
	
	//std::cout << "Five digit segments mapped." << std::endl;
	
	return digits;
}

template <typename V>
void print_digit_combinations(const V& vec){
	std::cout << "Digits: ";
	for (int i =  0; i < vec.size(); i++){
		std::cout << i << " - " << vec[i] << " | ";
	}
	std::cout << std::endl;
}

Digits parse_input(const std::string& s){
	std::stringstream ss(s);
	std::string item;
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

	Digits digits = get_digit_combinations(inputs);
	digits.output = outputs;
	// std::cout << "----------------------------" << std::endl;
	// std::cout << "Inputs: ";
	// print_vector(inputs);
	// std::cout << "Outputs: ";
	// print_vector(outputs);
	// print_digit_combinations(digits.map);


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
	std::cout << "Inputs parsed." << std::endl;

	int sum = std::accumulate(digit_combos.begin(), digit_combos.end(), 0, 
		[](int accum, Digits& dig){int sol = solve_hard(dig); std::cout << sol << std::endl; return accum + sol;});
	
	std::cout << "Sum of total numbers displayed: " << sum << std::endl;
	
	return 0;
}