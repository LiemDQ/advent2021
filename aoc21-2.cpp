#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include <array>
#include <algorithm>
#include <utility>

constexpr int WINNING_SCORE = 21;
constexpr int BOARD_POSITIONS = 10;

struct Player {
    int id;
    int position;
};

using ScoreRow = std::array<long, WINNING_SCORE>;
using ScoreGrid = std::array<ScoreRow, WINNING_SCORE>;
using GameState = std::array<std::array<ScoreGrid,BOARD_POSITIONS>,BOARD_POSITIONS>; //literal 4D chess

int get_new_position(int position, int roll_result){
    int result = (position + roll_result) % 10;
    return result;
}

int get_new_score(int position, int current_score){
    if (!position){
        current_score += 10;
    } else {
        current_score += position;
    }
    if (current_score >= WINNING_SCORE){
        current_score = WINNING_SCORE;
    }
    return current_score;
}

bool isGameFinished(const GameState& gm){
    for (auto& i:gm){
        for (auto& j:i){
            for (auto& k:j){
                for (long val: k){
                    if (val != 0)
                        return false;
                }
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]){
    std::fstream file(argv[1]);
    if(!file.is_open()){
        std::cerr << "Failed to open file " << argv[1] << std::endl;
        return 1;
    }
    
    std::string input;
    int id, position;
    std::vector<Player> players;
    while(std::getline(file, input)){
        std::sscanf(input.c_str(), "Player %d starting position: %d", &id, &position);
        players.push_back(Player{id, position});
        std::cout << "Player " << id << " starts in position " << position << "\n";
    };

    ScoreGrid grid;
    for (auto& a:grid){
        a.fill(0);
    }

    GameState game_state;
    for (auto& g:game_state){
        g.fill(grid);
    }

    //(amount, frequency)
    constexpr std::array<std::pair<long, long>, 7> psbl_roll_results = {{{3,1},{4,3},{5,6},{6,7},{7,6},{8,3},{9,1}}};
    long p1_win = 0;
    long p2_win = 0;

    //indices: player 1 position | player 2 position | player 1 score | player 2 score
    game_state[players[0].position][players[1].position][0][0] = 1;
    GameState gs2(game_state);
    
    bool p1_turn = true;
    while(!isGameFinished(game_state)){
        for (int i = 0; i < game_state.size(); i++){ //p1 positions
            for (int j = 0; j < game_state[0].size(); j++){ //p2 positions
                for (int k = 0; k < game_state[0][0].size(); k++){ //p1 score
                    for (int l = 0; l < game_state[0][0][0].size(); l++){ //p2 score
                        long n_states = game_state[i][j][k][l];
                        if (n_states){
                            for (auto [result, frequency]: psbl_roll_results){
                                if (p1_turn){
                                    int new_position = get_new_position(i, result);
                                    // std::cout << "New position for P1: " << new_position << std::endl; 
                                    int new_score = get_new_score(new_position, k);
                                    // std::cout << "New score for P1: " << new_score << std::endl;
                                    if (new_score == WINNING_SCORE){
                                        p1_win += frequency*n_states;
                                    } else {
                                        gs2[new_position][j][new_score][l] += frequency*n_states;
                                    }
                                } else {
                                    int new_position = get_new_position(j, result);
                                    int new_score = get_new_score(new_position, l);
                                    // std::cout << "New position for P2: " << new_position << std::endl;
                                    if (new_score == WINNING_SCORE){
                                        p2_win += frequency*n_states;
                                    } else {
                                        gs2[i][new_position][k][new_score] += frequency*n_states;
                                    }
                                }
                            }
                            gs2[i][j][k][l] -= n_states;
                        }
                    }
                }
            }
        }
        game_state = gs2;
        p1_turn = !p1_turn;
    }

    std::cout << "Number of P1 wins: " << p1_win << std::endl;
    std::cout << "Number of P2 wins: " << p2_win << std::endl;

    return 0;
}