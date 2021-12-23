#include <cstdlib>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <bitset>
#include <cstring>

constexpr int num_bits = 12; 
using Diagnostic = std::bitset<num_bits>;

bool find_most_common_bit(std::vector<Diagnostic>& vec, int index){
	int count = 0;
	const int size = vec.size();

	for (auto&& bits:vec){
		if (bits[index]){
			count++;
		}
	}
	if (size %2 == 1){
		return count > size/2;
	}
	else return count >= size/2;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	int n_lines = 0;
	std::vector<Diagnostic> bits;

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	Diagnostic line;
	while (file >> line){
		//std::cout << line << std::endl;
		n_lines++;
		bits.push_back(line);
		//std::cout << line << std::endl;
	}
	

	std::vector<Diagnostic> o2_vec = bits, co2_vec = bits;
	std::vector<Diagnostic> temp;
	int j = 0;
	for (int j = num_bits-1; j >= 0; j--){
		//std::cout << "O2" << std::endl;
		bool most_common_bit = find_most_common_bit(o2_vec, j);
		std::cout << "Most common bit " << most_common_bit << " at position: " << j << std::endl;
		std::copy_if(o2_vec.begin(), o2_vec.end(), std::back_inserter(temp), [j, most_common_bit](Diagnostic& s){ return s[j] == most_common_bit;});
		o2_vec.swap(temp);
		temp.erase(temp.begin(), temp.end());
		std::cout << "O2 size: " << o2_vec.size() << std::endl;
		if (o2_vec.size() == 1){
			std::cout << "Final result: " << o2_vec[0] << std::endl;
			break;
		}
	}

	j = 0;
	for(int k = num_bits-1; k >= 0; k--){
		bool least_common_bit = !find_most_common_bit(co2_vec, k);
		std::cout << "Least common bit " << least_common_bit << " at position: " << k << std::endl;

		std::copy_if(co2_vec.begin(), co2_vec.end(), std::back_inserter(temp), [k, least_common_bit](Diagnostic& s){ return s[k] == least_common_bit;});
		co2_vec.swap(temp);
		temp.erase(temp.begin(), temp.end());
		std::cout << "CO2 size: " << co2_vec.size() << std::endl;
		if (co2_vec.size() == 1){
			std::cout << "Final result: " << co2_vec[0] << std::endl;
			break;
		}
	}

	auto oxygen_rating = o2_vec[0].to_ulong();
	auto co2_rating = co2_vec[0].to_ulong();



	std::cout << "Oxygen rating: " << oxygen_rating << std::endl;
	std::cout << "CO2 rating: " << co2_rating << std::endl;
	std::cout << "Power consumption: " << oxygen_rating*co2_rating << std::endl;

	return 0;

}