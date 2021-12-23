#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <regex>
#include <algorithm>
#include <string>

constexpr int GRID_SIZE = 1000;

struct GridValue {
	int value = 0;
};

using GridRow = std::array<GridValue, GRID_SIZE>;
using Grid = std::array<GridRow, GRID_SIZE>;

struct Point {
	int x = 0;
	int y = 0;
};

struct Line {
	Point begin;
	Point end;
};

void print_line(const Line& line){
	std::cout << "(" << line.begin.x << "," << line.begin.y << ") -> (" << line.end.x << "," << line.end.y << ")" << std::endl;
}

Line parse_line_input(const std::string& s){
	Line line;
	std::regex coordinate_regex("(\\d+),(\\d+) -> (\\d+),(\\d+)");
	std::smatch coordinates; 
	bool result = std::regex_match(s, coordinates, coordinate_regex);
	if (result && coordinates.size() == 5){
		line.begin = {std::stoi(coordinates[1]), std::stoi(coordinates[2])};
		line.end = {std::stoi(coordinates[3]), std::stoi(coordinates[4])};
	} else {
		std::cout << "Ill-formed regex. String was " << s << std::endl;
		std::cout << "Size of match: " << coordinates.size() << std::endl;
	}

	return line;
}

void mark_line_on_grid(Grid& grid, const Line& line){
	//lines can either be horizontal or vertical
	//print_line(line);
	if (line.begin.x == line.end.x){
		//line is vertical
		// std::cout << "Vertical line: ";
		// print_line(line);
		int starting_y = line.begin.y < line.end.y ? line.begin.y : line.end.y;
		int ending_y = line.begin.y > line.end.y ? line.begin.y : line.end.y;
		for (int i = starting_y; i <= ending_y; i++){
			grid[i][line.begin.x].value++;
		}

	} else if (line.begin.y == line.end.y) {
		//line is horizontal
		// std::cout << "Horizontal line: ";
		// print_line(line);
		int starting_x = line.begin.x < line.end.x ? line.begin.x : line.end.x;
		int ending_x = line.begin.x > line.end.x ? line.begin.x : line.end.x;
		for (int i = starting_x; i <= ending_x; i++){
			grid[line.begin.y][i].value++;
		}
	} else {
		//line is diagonal
		// std::cout << "Diagonal line: ";
		// print_line(line);
		//set direction to iterate in
		int x_increment = line.begin.x < line.end.x ? 1 : -1;
		int y_increment = line.begin.y < line.end.y ? 1 : -1;
		
		int i = line.begin.x;
		int j = line.begin.y;
		while (i != line.end.x + x_increment){
			grid[j][i].value++;
			i += x_increment;
			j += y_increment;
		}
	}
}

int count_marked_threshold(const Grid& grid, int threshold){
	int count = 0;
	for (int i = 0; i < grid.size(); i++){
		//count += std::count_if(grid[i].begin(), grid[i].end(), [threshold](GridValue value){return value.value >= threshold; });
		for (int j = 0; j < grid[0].size(); j++){
			if (grid[j][i].value >= threshold){
				count++;
				//std::cout << grid[j][i].value << " overlaps at (" << i << "," << j << ")" << std::endl;
			}
		}
	}
	return count;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	Grid grid{0};
	//std::vector<Line> lines;
	std::string line_string;
	while(std::getline(file, line_string)){
		mark_line_on_grid(grid, parse_line_input(line_string));
	}


	int values_above_2 = count_marked_threshold(grid, 2);
	std::cout << "Number of overlaps: " << values_above_2 << std::endl;
}