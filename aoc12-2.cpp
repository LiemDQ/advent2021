#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <initializer_list>

enum class CaveType {
	small,
	big,
	start,
	end
};

struct Cave {
	std::string name;
	CaveType type;
	std::unordered_set<std::string> connections; 
};

CaveType get_cave_type(const std::string& name){
	if (name == "start"){
		return CaveType::start;
	} else if (name == "end"){
		return CaveType::end;
	} else if (std::islower(name[0])){
		return CaveType::small;
	} else {
		return CaveType::big;
	}
}


int visit(std::string src, std::unordered_map<std::string,Cave>& caves, std::unordered_map<std::string, int>& visited, bool visit_twice){
	//note: the big caves do not connect to each other, which simplifies the problem considerably as we do not have to check for cycles
	//std::cout << src << "	| " << visit_twice << std::endl;
	if (src == "end"){
		return 1;
	}
	int n_paths = 0;	

	visited[src]++;
	
	for (auto& connection:caves[src].connections){
		if (visited[connection] && (caves[connection].type == CaveType::small || caves[connection].type == CaveType::start)){
			if (!visit_twice && caves[connection].type == CaveType::small){
				n_paths += visit(connection, caves, visited, true);
			} else {
				continue;
			}
		} else {
			n_paths += visit(connection, caves, visited, visit_twice);
		} 
	}
	//std::cout << "Exiting " << src << std::endl;
	visited[src]--; 
	return n_paths;
}

int traverse(std::unordered_map<std::string, Cave>& caves){
	assert(caves["start"].type == CaveType::start && "Must begin with start cave");
	std::unordered_map<std::string, int> visited; 
	for (auto& cave:caves){
		//initialize visited map
		visited[cave.first] = 0;
	}
	
	Cave start = caves["start"];
	visited[start.name] = true;
	int n_paths = visit("start", caves, visited, false);
	return n_paths;
}

template <typename C>
void print_caves(C container){
	for (auto&& item:container){
		std::cout << "============================\n";
		std::cout << "Cave name: " << item.second.name << "\n"; 
		std::cout << "Connections: ";
		for (auto&& c:item.second.connections){
			std::cout << c << ", ";
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);	
	std::unordered_map<std::string, Cave> caves;
	std::string input;
	const std::string delimiter = "-";
	while(std::getline(file, input)){
		//set up caves
		std::string start = input.substr(0, input.find(delimiter));
		std::string end = input.substr(input.find(delimiter)+1, input.size()-1);		
		if (caves.find(start) == caves.end()){
			caves[start] = Cave{ start, get_cave_type(start), {end} };
		} else {
			caves[start].connections.emplace(end);
		}

		if (caves.find(end) == caves.end()){
			caves[end] = Cave{ end, get_cave_type(end), {start} };
		} else {
			caves[end].connections.emplace(start);
		}
	}

	int n = traverse(caves);
	std::cout << "Num paths: " << n << std::endl;
	

	return 0;
}