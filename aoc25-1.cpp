#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <optional>
enum class CellStatus {
    RIGHT,
    DOWN,
    EMPTY
};

using std::vector;
struct Grid {
    vector<vector<CellStatus>> grid;

    CellStatus get(size_t x, size_t y) const {
        return grid[y][x];
    }

    void set(size_t x, size_t y, CellStatus val){
        grid[y][x] = val;
    }

    std::optional<std::pair<int, int>> get_destination(size_t x, size_t y) const {
        auto state = get(x,y);
        switch (state){
            case CellStatus::RIGHT: {
                if (x == grid[0].size()-1){
                    return {{0 , y}};
                } else {
                    return {{x+1, y}};
                }
            }
            case CellStatus::DOWN: {
                if (y == grid.size() - 1){
                    return {{x, 0}};
                } else {
                    return {{x , y+1}};
                }
            }
            default:
                return std::nullopt;                
        }
    }

    bool can_move(size_t x, size_t y, CellStatus type) const {
        auto dest = get_destination(x, y);
        if (dest && get(x, y) == type){
            return get(dest->first, dest->second) == CellStatus::EMPTY;
        } else {
            return false;
        }
    }

    void move(size_t x, size_t y){
        auto val = get(x,y);
        auto dest = get_destination(x, y);
        assert(dest && "Invalid move was requested");
        set(dest->first, dest->second, val);
        set(x, y, CellStatus::EMPTY);
    }

    bool move_herds(){
        int num_moves = 0;
        //as long as we move from left to right then the behavior should be as specified
        for (size_t i = 0; i < grid.size(); i++){
        //edge case: must check the sides first
            int edge = grid[0].size() -1;
            bool can_edge_move = can_move(edge, i, CellStatus::RIGHT);
            for (size_t j = 0; j < grid[0].size()-1; j++){
                if (can_move(j, i, CellStatus::RIGHT)){
                    move(j, i);
                    num_moves++;
                    j++;
                }
            }
            if (can_edge_move){
                move(edge, i);
                num_moves++;
            }
        }
        //now for the downward facing cucumbers
        //as longas we move from town to bottom then behavior should be as specified
        for (size_t k = 0; k < grid[0].size(); k++){
            int edge = grid.size()-1;
            bool can_edge_move = can_move(k, edge, CellStatus::DOWN);
            for (size_t l = 0; l < grid.size()-1; l++){
                if (can_move(k, l, CellStatus::DOWN)){
                    move(k, l);
                    num_moves++;
                    l++;
                }
            }
            if (can_edge_move){
                move(k, edge);
                num_moves++;
            }
        }

        return num_moves != 0;
    }
};

std::ostream& operator<<(std::ostream& o, const Grid& g){
    for (auto& row:g.grid){
        for (CellStatus cell:row){
            switch(cell){
                case CellStatus::EMPTY: {
                    o << '.';
                    break;
                }
                case CellStatus::DOWN: {
                    o << 'v';
                    break;
                }
                case CellStatus::RIGHT: {
                    o << '>';
                    break;
                }
            }
        }
        o << "\n";
    }
    o << "####################################\n";
    return o;
}

int main(int argc, char* argv[]){
    std::fstream file(argv[1]);

    if (!file.is_open()){
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::string input;
    Grid map;
    std::vector<CellStatus> row;
    while(std::getline(file, input)){
        for (char c:input){
            switch(c){
                case '.': {
                    row.push_back(CellStatus::EMPTY);
                    break;
                }
                case '>': {
                    row.push_back(CellStatus::RIGHT);
                    break;
                }
                case 'v': {
                    row.push_back(CellStatus::DOWN);
                    break;
                }
            }
        }
        map.grid.push_back(row);
        row.clear();
    }

    size_t iter = 0;
    //std::cout << map;
    constexpr size_t max_iter = 1000000;
    while (map.move_herds() && iter < max_iter){
        iter++;
        //std::cout << iter << "\n";
        // std::cout << map;
    }

    std::cout << "Number of iterations: " << ++iter << std::endl;

    return 0;
}