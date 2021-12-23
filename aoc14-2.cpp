#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstring>
#include <cassert>
#include <iostream>
#include <utility>
#include <algorithm>
#include <numeric>

template <typename T1, typename T2>
void print_pair(std::pair<T1,T2> p){
	std::cout << "(" << p.first << "," << p.second << ")\n";
}

using Rules = std::unordered_map<std::string, char>;
using PolymerBin = std::unordered_map<std::string, long>;
using PolymerCount = std::unordered_map<char, long>;
std::string insert_polymers(const std::string& polymer, Rules& rules){
	std::string result (polymer); //copy original string
	std::string pair; 
	int inserted = 0;
	for (int i = 0; i < polymer.length()-1; i++){
		//std::cout << i << "," << i+1 << std::endl;
		pair = polymer.substr(i, 2);
		//std::cout << pair << std::endl;
		if(rules.count(pair)){
			//std::cout << "inserted char: " << rules[pair] << " at index " << i+1 << std::endl;
			result.insert(i+1+inserted, 1, rules[pair]);
			inserted++;
		}
	}
	return result;
}



PolymerBin insert_polymers2(const PolymerBin& polymers, const Rules& rules, PolymerCount& occurrences){
	PolymerBin result = polymers;
	for (auto&& item:polymers ){
		if (item.second > 0 && rules.count(item.first)){
			char first = item.first[0];
			char second = item.first[1];
			char insertion = rules.at(item.first);
			result[{first, insertion}] += item.second;
			result[{insertion, second}] += item.second;
			result[item.first] -= item.second;
			occurrences[insertion] += item.second;
		}
	}
	return result;
}

PolymerBin get_pair_permutations(const std::string& polymer, Rules& rules){
	std::unordered_set<char> chars;
	PolymerBin pairs;
	//get all possible chars
	for (char c:polymer){
		chars.insert(c);
	}
	for (auto&& item:rules){
		for (char c:item.first){
			chars.insert(c);
		}
		chars.insert(item.second);
	}
	//get cartesian product of all chars
	for (char c:chars){
		for (char b:chars){
			pairs.insert({{c,b},0});
		}
	}
	return pairs;
}

PolymerBin get_string_pairs(const PolymerBin& polymers, const std::string& s){
	PolymerBin result = polymers;
	for (int i = 0; i < s.length()-1; i++){
		result[s.substr(i, 2)]++;
	}
	return result;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);

	std::string inputs, pair, poly_template;
	char insert;
	Rules rules; 
	std::getline(file, poly_template);
	std::getline(file, inputs);
	while(std::getline(file, inputs)){
		pair = inputs.substr(0,inputs.find(" -> "));
		insert = inputs.substr(inputs.find(" -> ")+4)[0];
		rules.insert({pair, insert});

	}

	std::cout << poly_template << "\n";
	for (auto&& item:rules){
		print_pair(item);
	}

	std::string polymer = poly_template;
	//std::cout << "First two chars: " << poly_template.substr(0,2) << std::endl;

	constexpr int NUM_STEPS = 10;
	int n;
	if (argc < 3){
		n = NUM_STEPS;
	} else {
		n = std::atoi(argv[2]);
	}

	auto bins = get_pair_permutations(polymer, rules);
	bins = get_string_pairs(bins, polymer);
	PolymerCount occurrences; 
	for (char c:polymer){
		if(occurrences.count(c)){
			occurrences[c]++;
		} else {
			occurrences[c] = 1;
		}
	}

	for (int i = 0; i < n; i++){
		bins = insert_polymers2(bins, rules, occurrences);
	}

	auto sum = std::accumulate(occurrences.begin(), occurrences.end(), (long) 0, [](long sum, auto& p){ return sum + p.second;});

	std::cout << "Length of polymer: " << sum << std::endl;
	//std::cout << polymer << std::endl;

	auto compare_second = [](const auto& l, const auto& r){ return l.second < r.second;};
	auto min_it = std::min_element(occurrences.begin(), occurrences.end(), compare_second);
	auto max_it = std::max_element(occurrences.begin(), occurrences.end(), compare_second);
	
	std::cout << "Min occurrences: " << min_it->first << "," << min_it->second << std::endl;
	std::cout << "Max occurrences: " << max_it->first << "," << max_it->second << std::endl;
	std::cout << "Difference: " << max_it->second - min_it->second << std::endl;
	return 0;
}
