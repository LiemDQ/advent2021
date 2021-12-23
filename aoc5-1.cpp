#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <regex>
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
	std::regex coordinate_regex("([0-9]*),([0-9]*)\\s->\\s([0-9]*),([0-9]*)");
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
	if (line.begin.x == line.end.x){
		//line is vertical
		//print_line(line);
		int starting_y = line.begin.y < line.end.y ? line.begin.y : line.end.y;
		int ending_y = line.begin.y > line.end.y ? line.begin.y : line.end.y;
		for (int i = starting_y; i <= ending_y; i++){
			grid[i][line.begin.x].value++;
		}

	} else if (line.begin.y == line.end.y) {
		//line is horizontal
		//print_line(line);
		int starting_x = line.begin.x < line.end.x ? line.begin.x : line.end.x;
		int ending_x = line.begin.x > line.end.x ? line.begin.x : line.end.x;
		for (int i = starting_x; i <= ending_x; i++){
			grid[line.begin.y][i].value++;
		}
	} else {
		//line is diagonal
		//do nothing
	}
}

int count_marked_threshold(const Grid& grid, int threshold){
	int count = 0;
	for (auto&& row:grid){
		for (auto&& value:row){
			if (value.value >= threshold)
				count++;
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
	std::vector<Line> lines;
	std::string line_string;
	while(std::getline(file, line_string)){
		lines.push_back(parse_line_input(line_string));
	}


	for (auto& line:lines){
		mark_line_on_grid(grid, line);
	}
	int values_above_2 = count_marked_threshold(grid, 2);
	std::cout << "Number of overlaps: " << values_above_2 << std::endl;
}