#include <cassert>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <cctype>
#include <list>
#include <unordered_set>
#include <utility>

using Coordinate = std::pair<int, int>;
struct coord_hash {
	template <typename T1, typename T2>
	std::size_t operator() (const std::pair<T1,T2> &pair) const {
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};
using CoordinateSet = std::unordered_set<Coordinate, coord_hash>;

void print_coord(const Coordinate& coord){
	std::cout << "(" << coord.first << "," << coord.second << ")" << std::endl;
}

template <typename Container, typename PrintFunction>
void print_vector(const Container& vec, PrintFunction pf){
	for (auto&& item:vec){
		pf(item);
	}
}

enum class Fold {
	x,
	y
};

struct FoldCoord {
	Fold fold;
	int val;
};

int reflect_value(int value, int fold_coordinate){
	if (value < fold_coordinate){
		return value; //do nothing
	} else if (value > fold_coordinate) {
		int diff = value - fold_coordinate;
		int result = fold_coordinate - diff;
		return result;
	} else {
		assert(0 && "Dot is on fold line. Invalid state");
		return 0;
	}
}

CoordinateSet fold(const CoordinateSet& set, FoldCoord fold){
	CoordinateSet result;
	int x,y;
	
	if (fold.fold == Fold::x){
		for(const Coordinate& coord:set){
			x = reflect_value(coord.first, fold.val);
			y = coord.second;
			result.insert({x,y});
		}
	} else {
		for(const Coordinate& coord:set){
			x = coord.first;
			y = reflect_value(coord.second, fold.val);
			result.insert({x,y});
		}
	}
	return result;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	CoordinateSet coordinates; 
	std::vector<FoldCoord> folds;

	std::string inputs, parsed;
	while(std::getline(file, inputs)){
		if (std::isdigit(inputs[0])){
			std::stringstream ss(inputs);
			std::getline(ss,parsed,',');
			int x = std::stoi(parsed);
			std::getline(ss, parsed);
			int y = std::stoi(parsed);
			coordinates.insert(
				{x,y}
			);
		} else if (inputs[0] == 'f') {
			auto index = inputs.find('y');
			Fold coordinate = index == inputs.npos ? Fold::x : Fold::y; 
			auto digit_index = inputs.find_first_of("0123456789");
			int value = std::stoi(inputs.substr(digit_index));
			folds.push_back({
				coordinate,
				value
			});
		} else if (inputs.size() == 0) {
			//parse error
			continue;
		} else {
			assert(0 && "Parse error");
		}
	}
	//print_vector(coordinates, print_coord);
	//print_vector(folds, [](const FoldCoord& fc){ char c = fc.fold == Fold::x ? 'x' : 'y'; std::cout << c << ',' << fc.val << std::endl; });

	coordinates = fold(coordinates, folds[0]);

	std::cout << "Num points: " << coordinates.size() << std::endl;

	return 0;
}