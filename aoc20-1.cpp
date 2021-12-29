#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <numeric>

using std::vector;
using Grid = vector<vector<bool>>;
using Algorithm = std::bitset<512>;

struct ImageGrid {
    Grid m;
    size_t end_col = 0;
    size_t end_row = 0;
    bool parity = false;

    void initialize_end_col(){
        end_col = m.front().size()-2;
    }

    void addRow(vector<bool>& row){
        m.push_back(row);
        end_row++;
    }

    void padLeft(){
        for (auto& v:m){
            v.insert(v.begin(), parity);
        }
        end_col++;
    }

    void padRight(){
        for (auto& v:m){
            v.push_back(parity);
        }
        end_col++;
    }

    void padTop(){
        std::size_t sz = m.front().size();
        m.emplace(m.cbegin(), sz, parity);
        end_row++;
    }

    void padBottom(){
        std::size_t sz = m.back().size();
        m.emplace_back(sz, parity);
        end_row++;
    }

    void padAllSides(){
        padLeft();
        padRight();
        padTop();
        padBottom();
    }

    std::string getGridString(size_t x, size_t y) const {
        std::string output;
        for (int i = y-1; i <= y+1; i++){
            for (int j = x-1; j <= x+1; j++){
                output += (m[i][j] ? '1' : '0');
            }
        }
        return output;
    }

    void flipBorder(){
        std::fill(m.front().begin(), m.front().end(), parity);
        std::fill(m.back().begin(), m.back().end(), parity);
        for (auto& v:m){
            v.front() = parity;
            v.back() = parity;
        }
    }


    size_t getAlgoIndexFromGrid(size_t x, size_t y) const {
        std::string str = getGridString(x, y);
        return std::stoul(str, nullptr, 2);
    }

    template <typename Func>
    Grid iterateAcrossGrid(Func&& f){
        Grid grid(m);
        for (int i = 1; i < grid.size()-1; i++){ //y
            for (int j = 1; j < grid.front().size()-1; j++){ //x
                f(grid, j, i);
            }
        }
        return grid;
    }

    void update(const Algorithm& algo, Grid& grid, size_t x, size_t y){
        size_t idx = getAlgoIndexFromGrid(x, y);
        bool val = algo[idx];
        grid[y][x] = val;        
    }

    void updateAll(const Algorithm& algo){
        auto func = [this, &algo](Grid& grid, size_t x, size_t y){this->update(algo, grid, x, y);};
        Grid g = iterateAcrossGrid(func);
        std::swap(m, g);
        parity = parity ^ algo[0];
        flipBorder();
        padAllSides();
    }
};

std::ostream& operator<<(std::ostream& o, const ImageGrid& grid){
    std::cout << "Image grid: \n\n";
    for (auto& row:grid.m){
        for (auto b:row){
            o << (b ? '#' : '.');
        }
        o << "\n";
    }
    return o;
}

int parse_char(char c){
    if (c == '#'){
        return 1;
    } else if (c == '.'){
        return 0;
    } else {
        assert(0 && "Invalid character found");
    }
}

int getNumLights(const ImageGrid& grid){
    auto sum = [](int total, const vector<bool>& v){return total + std::accumulate(v.begin(), v.end(), 0);};

    return std::accumulate(grid.m.begin(), grid.m.end(), 0, sum);
}

int main(int argc, char* argv[]){
    std::fstream file(argv[1]);
    
    if (!file.is_open()){
        std::cerr << "Failed to open file " << argv[1] << std::endl;
        return 1;
    }

    Algorithm algo;
    std::string algo_str;
    std::getline(file, algo_str);
    assert(algo_str.length() == 512);
    for (int i = 0; i < algo_str.length(); i++){
        algo[i] = parse_char(algo_str[i]);
    }

    std::string input;
    std::getline(file, input);
    assert(input.empty() && "Line should be empty");
    ImageGrid img;
    std::vector<bool> v;
    while(std::getline(file, input)){
        for (auto c:input){
            v.push_back(parse_char(c));
        }
        img.m.push_back(v);
        v.clear();
    }

    img.padAllSides();
    img.padAllSides();
    img.padAllSides();
    img.initialize_end_col();

    // std::cout << algo.to_string() << std::endl;
    // std::cout << img;
    // std::cout << img.getGridString(5,5) << std::endl;
    // std::cout << img.getAlgoIndexFromGrid(5,5) << std::endl;

    img.updateAll(algo);
    // std::cout << img;
    img.updateAll(algo);
    
    int n = getNumLights(img);

    std::cout << img;   
    std::cout << "Number of lights: " << n << std::endl;

    return 0;   
}