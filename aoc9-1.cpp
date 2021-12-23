#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <numeric>

using Row = std::vector<int>;
struct Grid {
	std::vector<Row> rows; 
	bool check_if_low_point(int x, int y){
		int val = rows[y][x];
		bool x_result, y_result;
		if (x != 0 && x != rows[0].size()-1){
			bool left = rows[y][x-1] > val;
			bool right = rows[y][x+1] > val;
			x_result = left && right;
		} else if (x == rows[0].size()-1){
			bool left = rows[y][x-1] > val;
			x_result = left;
		} else {
			bool right = rows[y][x+1] > val;
			x_result = right;
		}

		if (y != 0 && y != rows.size()-1){
			bool top = rows[y-1][x] > val;
			bool bot = rows[y+1][x] > val;
			y_result = top && bot;

		} else if (y == rows.size() - 1){
			bool top = rows[y-1][x] > val;
			y_result = top;
		} else {
			bool bot = rows[y+1][x] > val;
			y_result = bot;
		}
		return y_result && x_result;
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
	std::vector<int> risk_levels;
	
	while(std::getline(file, line)){
		for (auto& character:line){
			int i = character - '0';
			input_row.push_back(i);
		}
		grid.rows.push_back(input_row);
		input_row.erase(input_row.begin(), input_row.end());
	}

	for (int i = 0; i < grid.rows.size(); i++){
		for (int j = 0; j < grid.rows[0].size(); j++){
			if (grid.check_if_low_point(j, i)){
				risk_levels.push_back(grid.rows[i][j]);
			}
		}
	}

	int sum = std::accumulate(risk_levels.begin(), risk_levels.end(), 0, [](int accum, int risk_lv){ return accum + risk_lv + 1;});
	std::cout << "Total risk level: " << sum << std::endl;

	return 0;
}