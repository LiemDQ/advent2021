#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	if (!file.is_open()){
		std::cerr << "Could not open file: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	char direction[7];
	int distance;
	
	int depth, position, aim = 0;

	std::string line;
	while (std::getline(file, line)){
		std::stringstream ss(line);
		ss >> direction;
		ss >> distance;
		
		if (std::strcmp(direction, "forward") == 0){
			position += distance;
			depth += aim*distance;
		} else if (std::strcmp(direction, "up") == 0){
			aim -= distance;
		} else if (std::strcmp(direction, "down") == 0){
			aim += distance;
		} else {}
	}
	file.close();
	
	std::cout << "Final depth: " << depth << " | Final position: " << position << std::endl;
	std::cout << "Product of depth and position: " << depth*position << std::endl;

	return 0;
}