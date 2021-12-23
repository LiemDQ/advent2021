#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cstring>

struct Octopus {
	int energy = 0;
	bool has_flashed = false;
};

struct OctopusGrid{
	using Rows = std::vector<Octopus>;
	std::vector<Rows> grid; 
};

void print_grid(const OctopusGrid& grid){
	for (auto& row:grid.grid){
		for(auto&& value:row){
			std::cout << value.energy;
		}
		std::cout << std::endl;
	}
}

void flash(OctopusGrid& grid, int x, int y);

void increment_energy(OctopusGrid& grid, int y, int x){
	if(x < 0 || x >= grid.grid[0].size()){
		return;
	}

	if(y < 0 || y >= grid.grid.size()){
		return;
	}
	
	if (!grid.grid[y][x].has_flashed){
		if(++grid.grid[y][x].energy > 9){
			flash(grid, x, y);
		}
	}
}

void flash(OctopusGrid& grid, int x, int y){
	Octopus& oct = grid.grid[y][x];

	oct.energy = 0;
	oct.has_flashed = true;
	
	increment_energy(grid, y-1, x-1);
	increment_energy(grid,y-1,x);
	increment_energy(grid,y-1,x+1);
	increment_energy(grid,y+1,x-1);
	increment_energy(grid,y+1,x);
	increment_energy(grid,y+1,x+1);
	increment_energy(grid,y,x+1);
	increment_energy(grid,y,x-1);
}

int reset_flash_and_count(OctopusGrid& grid){
	int n = 0;
	for (auto&& row:grid.grid){
		for (Octopus& oct:row){
			if(oct.has_flashed)
				n++;
			oct.has_flashed = false; 
		}
	}
	return n;
}

std::vector<Octopus> str2row(const std::string& str){
	std::vector<Octopus> vec;
	for(char c:str){
		int i = c - '0';
		vec.push_back({i, false});
	}

	return vec; 
}

int count_flashes(OctopusGrid& grid, int num_steps){
	int n = 0;
	for (int i = 0; i < num_steps; i++){
		for (int j = 0; j < grid.grid.size(); j++){
			for (int k = 0; k < grid.grid[0].size(); k++){
				increment_energy(grid, j, k);
			}
		}
		n += reset_flash_and_count(grid);
		//print_grid(grid);
		//std::cout << "///\n"; 
	}
	return n;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	std::string input;
	OctopusGrid octopus_grid;
	while(std::getline(file, input)){
		octopus_grid.grid.push_back(str2row(input));
	}
	std::cout << "Starting grid:\n"; 
	print_grid(octopus_grid);
	std::cout << "---\n"; 

	constexpr int N_STEPS = 100;
	int num_flashes = count_flashes(octopus_grid, N_STEPS);
	std::cout << "Number of flashes: " << num_flashes << std::endl;
	return 0;
}