#include <ostream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <cstring>
#include <string>

constexpr std::size_t bingo_size = 5;
constexpr std::size_t board_size = bingo_size*bingo_size;

struct BingoVal {
	int val;
	bool marked = false;
};

struct BingoBoard {
	using Row = std::array<BingoVal ,bingo_size>;
	std::array<Row, bingo_size> rows;
	//first index: row
	//second index: column 
};

bool check_if_board_wins(const BingoBoard& board){
	//check rows
	for (auto&& row:board.rows){
		bool row_all_marked = true;
		for (auto&&val:row){
			if (!val.marked) {
				row_all_marked = false;
				break;
			}
		}
		if (row_all_marked) return true;
	}
	
	//check columns
	for (int i = 0; i < bingo_size; i++){
		bool column_all_marked = true;
		for (int j = 0; j < bingo_size; j++){
			if (!board.rows[j][i].marked){
				column_all_marked = false;
				break;
			}
		}
		if (column_all_marked) return true;
	}
	return false;
}

int sum_unmarked_values(const BingoBoard& board){
	int sum = 0;
	for (auto&& row:board.rows){
		for (auto&& value:row){
			if (!value.marked){
				sum += value.val;
			}
		}
	}
	return sum;
}

BingoBoard create_from_array(const std::array<int,board_size>& arr){
	BingoBoard board;
	for (int i = 0; i < bingo_size; i++){
		for (int j = 0; j < bingo_size; j++){
			// int index;
			// if (i == 0) {
			// 	index = j;
			// } else {
			// 	index = 5*i+j
			// }
			board.rows[i][j] = {arr[5*i +j]};
		}
	}
	return board; 
}

bool mark_board_with_val(BingoBoard& board, int value){
	for (auto&& row:board.rows){
		for (auto& bingo_val:row){
			if (bingo_val.val == value){
				bingo_val.marked = true;
				return true; //assuming unique values on board
			}
		}
	}
	return false;
}

std::vector<int> split_into_int(const std::string& s, char delim){
	std::vector<int> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)){
		result.push_back(std::stoi(item));
	}

	return result;
}

int check_for_winner(const std::vector<BingoBoard>& boards){
	for (int i = 0; i < boards.size(); i++){
		if(check_if_board_wins(boards[i])){
			return i;
		}
	}
	return -1;
}

void print_board(const BingoBoard& board){
	for (auto&& row:board.rows){
		for (auto&& value:row){
			std::cout << value.val << " ";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}


int main(int argc, char* argv[]){
	std::fstream file(argv[1]);

	if (!file.is_open()){
		std::cerr << "Could not open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string inputs;
	std::getline(file, inputs);
	std::vector<int> moves = split_into_int(inputs, ',');
	
	std::array<int, board_size> flattened_board;
	std::vector<BingoBoard> boards;
	int val, count = 0;
	while (file >> val){
		flattened_board[count] = val;
		count++;
		if (count == board_size){
			boards.push_back(create_from_array(flattened_board));
			count = 0;
		}
	}
	std::cout << "Total number of boards: " << boards.size() << std::endl;
	std::cout << "Total number of moves: " << moves.size() << std::endl;
	print_board(boards.front());
	print_board(boards.back());


	int num_moves = 0;
	for (auto&& move:moves){
		num_moves++;
		for (auto&& board:boards){
			mark_board_with_val(board, move);
		}
		int result = check_for_winner(boards);
		if (result != -1){
			int score = sum_unmarked_values(boards[result]);
			std::cout << "Winning board was " << result << ". Moves taken: " <<  num_moves << "\n";
			std::cout << "Last number called: " << move << std::endl;
			std::cout << "Summed value of unmarked squares: " << score << std::endl;
			std::cout << "Final score: " << score*move << std::endl;
			break;
		}
	}
	return 0;
}