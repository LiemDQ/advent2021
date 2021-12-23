#include <cstdint>
#include <cstring>
#include <iterator>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <limits>
#include <unordered_set>
#include <cassert>
#include <numeric>

struct Node {
	int risk;
	std::size_t x;
	std::size_t y;
	bool visited = false;
	int total_risk_from_start = std::numeric_limits<int>::max();
};

using Row = std::vector<Node>;
using Grid = std::vector<Row>;

void update_total_node_risk(const Node& current, Node& neighbor){
	int risk = current.total_risk_from_start + neighbor.risk;
	if (risk < neighbor.total_risk_from_start){
		neighbor.total_risk_from_start = risk;
	} 
}

void push_if_not_visited(std::vector<Node*>& v, Node* n){
	if (n->visited){
		return;
	} else {
		v.push_back(n);
	}
}

std::vector<Node*> get_neighbors(Grid& grid, int x, int y){
	std::vector<Node*> neighbors;
	if (y > 0){
		push_if_not_visited(neighbors, &grid[y-1][x]);
	}
	if (y < grid.size() - 1){
		push_if_not_visited(neighbors, &grid[y+1][x]);
	}

	if (x > 0){
		push_if_not_visited(neighbors, &grid[y][x-1]);
	} 
	if (x < grid[0].size() - 1){
		push_if_not_visited(neighbors, &grid[y][x+1]);		
	}
	return neighbors;
}

int djikstra(Grid& grid){
	int x = 0, y = 0;
	Node* current_node = &grid[y][x];
	current_node->total_risk_from_start = 0;
	current_node->visited = true;
	std::unordered_set<Node*> spt_set;
	std::vector<Node*> neighbors; 

	const std::size_t max_y = grid.size()-1;
	const std::size_t max_x = grid[0].size() -1;
	//std::cout << max_x << std::endl;
	//std::cout << max_y << std::endl;

	Node* end_node = &grid[max_y][max_x];
	
	while(current_node != end_node){
		neighbors = get_neighbors(grid, x, y);
		for (Node* ng:neighbors){
			update_total_node_risk(*current_node, *ng);
			spt_set.insert(ng);
		}

		Node* min_risk_node = end_node;
		for (Node* ng:spt_set){
			if(ng->total_risk_from_start < min_risk_node->total_risk_from_start){
				min_risk_node = ng;
			}
		}

		current_node = min_risk_node;
		current_node->visited = true;
		x = current_node->x;
		y = current_node->y;
		spt_set.erase(current_node);
	}
	return end_node->total_risk_from_start;
}

Grid increment_grid(Grid& grid){
	
	auto increment_vec = [](Node& node){
		Node result = node;
		if (node.risk == 9){
			result.risk = 1;
		} else {
			result.risk++;
		}
		return result;
	};

	Grid result; 
	std::transform(	grid.begin(),grid.end(),std::back_inserter(result),
		[increment_vec](auto& vec){
			std::vector<Node> result_vec;
			std::transform(vec.begin(), vec.end(), std::back_inserter(result_vec), increment_vec);
			return result_vec;
		}
	);

	return result;
}

void append_grid_right(Grid& g1, Grid& g2){
	assert(g1.size() == g2.size());
	for (int i = 0; i < g1.size(); i++){
		const std::size_t sz = g1[i].size();
		std::transform(g2[i].begin(), g2[i].end(), g2[i].begin(), [sz](Node node){node.x += sz; return node;});
		g1[i].insert(g1[i].end(), g2[i].begin(), g2[i].end());
	}
}

void print_grid(const Grid& grid){
	for (auto& v:grid){
		for(const Node& n:v){
			std::cout << n.risk;
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	std::string inputs;

	Grid grid;
	while(std::getline(file, inputs)){
		Row row;
		for (char c:inputs){
			int digit = c - '0';
			row.push_back({digit, row.size(), grid.size()});
		}
		grid.push_back(row);
	}

	std::vector<Grid> incremented_grids;
	incremented_grids.push_back(grid);
	for (int i =0; i < 4; i++){
		incremented_grids.push_back(increment_grid(incremented_grids[i]));
	}
	Grid big_grid = grid;
	for (int i = 1; i < 5; i++){
		append_grid_right(big_grid, incremented_grids[i]);
	}
	
	Grid incremented_wide_grid = big_grid;
	const std::size_t sz = incremented_wide_grid.size();
	for (int j = 0; j < 4; j++){
		incremented_wide_grid = increment_grid(incremented_wide_grid);
		std::transform(incremented_wide_grid.begin(), incremented_wide_grid.end(), incremented_wide_grid.begin(),
			[sz](auto v){
				std::transform(v.begin(),v.end(), v.begin(), [sz](Node node){node.y += sz; return node;});
				return v;
			});
		big_grid.insert(big_grid.end(), incremented_wide_grid.begin(), incremented_wide_grid.end());
	}
	//print_grid(grid);
	//print_grid(big_grid);
	//start
	int result = djikstra(big_grid);
	std::cout << "Total risk to end: " << result << std::endl;

	return 0;
}