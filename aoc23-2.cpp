#include <cassert>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <array>
#include <algorithm>
#include <optional>
#include <exception>


const inline std::array<int, 4> COST = {1, 10, 100, 1000};

struct Room {
    Room(char who, std::array<char, 4> data): owner(who), data{data} {
        for(char c:data){
            if (c == owner){
                fixed++;
            } else {
                break;
            }
        }
    }
    char top() const {
        if (index == 0){
            throw std::runtime_error("Room is empty");
        }
        return data[index-1];
    }
    size_t push(char who){
        if (!can_accept()){
            throw std::runtime_error("Room cannot accept an amphipod");
        }
        if (who != owner){
            throw std::runtime_error("Room cannot accept this type of amphipod");
        }
        size_t cost = COST.at(owner- 'A')*(data.size() - index);
        data[index++] = who;
        fixed++;
        return cost; 
    }
    size_t pop(){
        if (index == 0){
            throw std::runtime_error("Room is empty");
        }
        size_t cost = COST.at(data[index-1]-'A')*(data.size() - index + 1);
        data[--index] = '.';
        return cost; 
    }

    bool is_finished() const {
        return fixed == data.size();
    }

    bool can_accept() const {
        return index < data.size() && !has_visitors();
    }

    bool has_visitors() const {
        return index > fixed;
    }

        std::array<char, 4> data;
    private:
        char owner;
        size_t index = data.size();
        size_t fixed = 0;
};

using Rooms = std::array<Room, 4>;

size_t distance(size_t src, size_t dest){
    if (src > dest) {
        return src - dest;
    } else {
        return dest - src;
    }
}

struct Hallway {
    Hallway(Rooms rms): rooms{rms} {
        hallway.fill('.');
    }
    Hallway(const Hallway&) = default;
    Rooms rooms;
    std::array<char, 11> hallway;
    size_t total_cost = 0;

    size_t hallway_to_room() {
        for (size_t i = 0; i < hallway.size(); i++){
            char entity = hallway[i];
            if (entity == '.')
                continue;
            
            if (!rooms[entity - 'A'].can_accept()) {
                continue;
            }
            
            size_t target = 2 + 2*(entity - 'A'); //index of hallway 
            if (occupied_spaces(i, target) > 1){
                continue;
            }
                
            size_t cost = rooms[entity - 'A'].push(entity);
            hallway[i] = '.';
            return cost + COST.at(entity-'A')*distance(i, target);            
        }
        return 0;
    }
    size_t room_to_room() {
        for (size_t i = 0; i < rooms.size(); i++){
            if (!rooms[i].has_visitors())
                continue;
            char entity = rooms[i].top();
            if (!rooms[entity - 'A'].can_accept())
                continue;

            size_t origin = 2 + 2*i;
            size_t target = 2 + 2*(entity-'A');
            if (occupied_spaces(origin, target) > 0)
                continue;
            
            size_t room_leave_cost = rooms[i].pop();
            size_t room_enter_cost = rooms[entity - 'A'].push(entity);
            return room_leave_cost + room_enter_cost + distance(origin, target)*COST.at(entity-'A');
        }
        return 0;
    }
    size_t room_to_hallway(size_t n_room, size_t target) {
        if (!rooms[n_room].has_visitors()){
            return 0;
        } 
        if (target % 2 == 0 && target != 0 && target != 10){
            return 0;
        }
        size_t source = n_room*2 + 2;
        if (occupied_spaces(source, target) > 0)
            return 0;
        char entity = rooms[n_room].top();
        size_t cost = rooms[n_room].pop() + distance(source, target)*COST.at(entity-'A');
        hallway[target] = entity;
        
        return cost; 
    }
    size_t occupied_spaces(size_t src, size_t dest){
        size_t begin = src < dest ? src : dest;
        size_t end = src > dest ? src : dest;
        size_t count = 0;
        for (size_t i = begin; i <= end; i++){
            if (hallway[i] != '.'){
                count++;
            }
        }
        return count;
    }
    bool is_finished() const {
        for (auto& room: rooms){
            if (!room.is_finished())
                return false;
        }
        return true;
    }

    
    size_t num_finished() const {
        size_t count = 0;
        for (auto& rm:rooms){
            if (rm.is_finished())
                count++;
        }
        return count;
    }

    size_t num_visitors() const {
        size_t count = 0;
        for (auto& rm:rooms){
            if (rm.has_visitors())
                count++;
        }
        return count;
    }

    bool is_congested() const {
        return hallway[3] != '.' && hallway[5] != '.' && hallway[7] != '.';
    }
};

std::ostream& operator<<(std::ostream& o, const Hallway& hw){
    for (char c:hw.hallway){
        o << c;
    }
    for (int i = hw.rooms[0].data.size() - 1; i >= 0 ; i--){
        o << "\n";
        o << "  ";
        for (auto& rm:hw.rooms){
            o << rm.data[i] << " ";
        }
    }   
    o << "\n///////////////////////////\n"; 
    return o;
}

struct LtHallway {
    constexpr bool operator()(const Hallway& lhs, const Hallway& rhs) const{
        constexpr int FINISHED_PENALTY = 100;
        return lhs.total_cost 
            < rhs.total_cost;
    }
};

using HallwaySet = std::multiset<Hallway, LtHallway>;

size_t djikstra(Rooms rms){
    Hallway init{rms};
    //std::cout << init;
    HallwaySet to_visit{init};
    bool done = false;
    int n = 0;
    const int max_count = 10000000;
    while (n < max_count){
        Hallway hw = *to_visit.begin();
        if (hw.is_finished()){
            std::cout << "Total iterations: " << n << std::endl;
            return hw.total_cost;
        }

        Hallway hw2(hw), hw3(hw);

        size_t h2r = hw3.hallway_to_room();
        if (h2r){
            hw3.total_cost += h2r;
            to_visit.insert(hw3);
        }
        size_t r2r = hw2.room_to_room();
        if (r2r){
            hw2.total_cost += r2r;
            to_visit.insert(hw2);
        }
        if (!(r2r || h2r)){
            for (size_t room = 0; room < hw.rooms.size(); room++){
                for (size_t point = 0; point < hw.hallway.size(); point++){
                    Hallway hw_copy(hw);
                    auto cost = hw_copy.room_to_hallway(room, point);
                    if (cost){
                        hw_copy.total_cost += cost; 
                        to_visit.insert(hw_copy);
                    }
                }
            }
        }
        //std::cout << to_visit.size() << std::endl;
        to_visit.erase(to_visit.begin());
        n++;
    }
    std::cout << "Exceeded maximum number of iterations without converging on a solution.\n";
    return 0;
}

Rooms read_file(std::fstream& f){
    std::string input;
    std::getline(f, input);
    std::getline(f, input);
    std::getline(f, input);
    char a1,a2,b1,b2,c1,c2,d1,d2;
    std::sscanf(input.c_str(), "###%c#%c#%c#%c###", &a1, &b1, &c1, &d1);
    std::getline(f, input);
    std::sscanf(input.c_str(), "  #%c#%c#%c#%c#", &a2, &b2, &c2, &d2);
    Rooms rooms{{{'A',{{a2, 'D', 'D', a1}}}, {'B', {b2, 'B', 'C', b1}}, {'C', {c2, 'A', 'B', c1}}, {'D', {d2, 'C', 'A', d1}}}};
    return rooms;
}

int main(int argc, char* argv[]){
    std::fstream file(argv[1]);
    if (!file.is_open()){
        std::cerr << "Could not open file " << argv[1] << std::endl;
        return 1;
    }

    Rooms rooms = read_file(file);
    size_t min_cost = djikstra(rooms);
    
    std::cout << "Minimum cost is: " << min_cost << std::endl;
    return 0;
}