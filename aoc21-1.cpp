#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include <array>

constexpr int WINNING_SCORE = 1000;

struct Dice {
    int counter = 0;
    int current_val = 1;

    int roll(){
        int result = 0;
        for (int i = 0; i < 3; i++){
            result += current_val;
            current_val++;
            current_val %= 100;
            current_val = current_val == 0 ? 100 : current_val;
        }
        counter += 3;
        return result;
    }
};

struct Player {
    int id;
    int position;
    int score = 0;
    bool hasWon = false;
    bool play(Dice& dice){
        int result = dice.roll();
        int new_position = (position + result) % 10;
        if (new_position == 0){
            new_position = 10;
        }
        position = new_position;
        score += position;
        hasWon = score >= WINNING_SCORE;
        return hasWon;
    }
};

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

    Dice die;

    int i = 0;
    while(true){
        if (players[i].play(die))
            break;
        i++;
        if (i == players.size())
            i = 0;
    }

    for (auto& player: players){
        if (!player.hasWon){
            std::cout << "The losing player score is " << player.score << "\n";
            std::cout << "Number of die rolls: " << die.counter << "\n";
            std::cout << "Product of the two: " << die.counter*player.score << "\n";
        } else {
            std::cout << "Winning player score is " << player.score << "\n";
        }
    }
    
    return 0;
}