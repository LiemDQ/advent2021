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
	const std::size_t x;
	const std::size_t y;
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
	//start
	int result = djikstra(grid);
	std::cout << "Total risk to end: " << result << std::endl;

	return 0;
}