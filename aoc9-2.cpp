#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <numeric>


struct Cave {
	int height;
	bool filled = false; 
};

using Row = std::vector<Cave>;
struct Grid {
	std::vector<Row> rows; 
	bool check_if_low_point(int x, int y){
		int val = rows[y][x].height;
		bool x_result, y_result;
		if (x != 0 && x != rows[0].size()-1){
			bool left = rows[y][x-1].height > val;
			bool right = rows[y][x+1].height > val;
			x_result = left && right;
		} else if (x == rows[0].size()-1){
			bool left = rows[y][x-1].height > val;
			x_result = left;
		} else {
			bool right = rows[y][x+1].height > val;
			x_result = right;
		}

		if (y != 0 && y != rows.size()-1){
			bool top = rows[y-1][x].height > val;
			bool bot = rows[y+1][x].height > val;
			y_result = top && bot;

		} else if (y == rows.size() - 1){
			bool top = rows[y-1][x].height > val;
			y_result = top;
		} else {
			bool bot = rows[y+1][x].height > val;
			y_result = bot;
		}
		return y_result && x_result;
	}

	int flood_region(int x, int y){
		//std::cout << "x: " << x << " y: " << y << std::endl;
		int count = 0;
		if (y == rows.size() || x == rows[0].size() || y == -1 || x == -1){
			return 0;
		}
		Cave node = rows[y][x];
		if (node.height == 9 || node.filled){
			return 0;
		}

		rows[y][x].filled = true;
		count++;
		count += flood_region(x, y+1);
		count += flood_region(x, y-1);
		count += flood_region(x+1, y);
		count += flood_region(x-1, y);
		return count;
	}
};

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	Grid grid;

	std::string line;
	Row input_row; 
	std::vector<Cave> risk_levels;
	std::vector<int> basin_sizes;
	
	while(std::getline(file, line)){
		for (auto& character:line){
			int i = character - '0';
			input_row.push_back({i});
		}
		grid.rows.push_back(input_row);
		input_row.erase(input_row.begin(), input_row.end());
	}

	for (int i = 0; i < grid.rows.size(); i++){
		for (int j = 0; j < grid.rows[0].size(); j++){
			if (grid.check_if_low_point(j, i)){
				basin_sizes.push_back(grid.flood_region(j, i));
			}
		}
	}

	//int sum = std::accumulate(risk_levels.begin(), risk_levels.end(), 0, [](int accum, int risk_lv){ return accum + risk_lv + 1;});
	std::sort(basin_sizes.begin(), basin_sizes.end(), [](int a, int b){ return a >b;});
	std::cout << "Basin sizes: ";
	for (auto size:basin_sizes){
		std::cout << size << " | ";
	}
	std::cout << std::endl;

	std::cout << "Product of biggest basins: " << basin_sizes[0]*basin_sizes[1]*basin_sizes[2] << std::endl;
 
	return 0;
}