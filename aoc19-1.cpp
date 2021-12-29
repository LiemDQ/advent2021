#include <vector>
#include <fstream>
#include <string>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <utility>
#include <optional>
#include <cstdio>
#include <functional>
#include <array>
#include <cassert>



using DMatrix = std::vector<std::vector<long>>;

struct Rotation {
    constexpr Rotation(const int a, const int b, const int c,const int d, const int e, 
        const int f, const int g,const int h, const int i): 
    m{{{a,b,c}, {d,e,f}, {g,h,i}}}
    {//assert(a*(e*i-f*h)+b*(d*i-f*g)+c*(d*h-e*g) == 1 && "Rotation matrix does not have determinant 1");
    }
    std::array<std::array<int, 3>, 3> m;    
};

struct Coordinate {
    long x;
    long y;
    long z;

    friend Coordinate operator+(Coordinate& left, const Coordinate& right);
};

constexpr int NUM_ROTATIONS = 24;

Coordinate operator+(Coordinate& left, const Coordinate& right){
    Coordinate coord;
    coord.x = left.x + right.x;
    coord.y = left.y + right.y;
    coord.z = left.z + right.z;

    return coord;
};

Coordinate operator+(Coordinate&& left, const Coordinate& right){
    left.x += right.x;
    left.y += right.y;
    left.z += right.z;
    return left;
}

Coordinate operator*(const Rotation& rot, const Coordinate& c){
    //same semantics as matrix multiplication with a vector
    auto& m = rot.m;
    Coordinate result{
    m[0][0]*c.x + m[0][1]*c.y + m[0][2]*c.z, 
    m[1][0]*c.x + m[1][1]*c.y + m[1][2]*c.z, 
    m[2][0]*c.x + m[2][1]*c.y + m[2][2]*c.z};
    
    return result;
}

constexpr std::array<Rotation, NUM_ROTATIONS> rotations{{
    { 1, 0, 0,  0, 1, 0,   0, 0, 1},
    { 0, 0,-1,  0,-1, 0,  -1, 0, 0},
    {-1, 0, 0,  0,-1, 0,   0, 0, 1},

    { 0, 0,-1,  0, 1, 0,   1, 0, 0},
    {-1, 0, 0,  0, 1, 0,   0, 0,-1},
    { 0, 0, 1,  0,-1, 0,   1, 0, 0},
    
    { 1, 0, 0,  0,-1, 0,   0, 0,-1},
    { 0, 0, 1,  0, 1, 0,  -1, 0, 0},
    { 0, 1, 0,  0, 0, 1,   1, 0, 0},
    
    {-1, 0, 0,  0, 0,-1,   0,-1, 0},
    { 0,-1, 0,  0, 0,-1,   1, 0, 0},
    {-1, 0, 0,  0, 0, 1,   0, 1, 0},

    { 0,-1, 0,  0, 0, 1,  -1, 0, 0},
    { 1, 0, 0,  0, 0,-1,   0, 1, 0},
    { 0, 1, 0,  0, 0,-1,  -1, 0, 0},

    { 1, 0, 0,  0, 0, 1,   0,-1, 0},
    { 0, 0, 1,  1, 0, 0,   0, 1, 0},
    { 0,-1, 0, -1, 0, 0,   0, 0,-1},

    { 0, 0,-1, -1, 0, 0,   0, 1, 0},
    { 0,-1, 0,  1, 0, 0,   0, 0, 1},
    { 0, 0,-1,  1, 0, 0,   0,-1, 0},

    { 0, 1, 0, -1, 0, 0,   0, 0, 1},
    { 0, 0, 1, -1, 0, 0,   0,-1, 0},
    { 0, 1, 0,  1, 0, 0,   0, 0,-1}
    
}};

Coordinate operator-(const Coordinate& left, const Coordinate& right){
    Coordinate coord;
    coord.x = left.x - right.x;
    coord.y = left.y - right.y;
    coord.z = left.z - right.z;

    return coord; 
}

Coordinate operator/(Coordinate& left, double right){
    Coordinate result = left;
    result.x /= right;
    result.y /= right;
    result.z /= right;
    return result;
}

Coordinate operator/(Coordinate& left, Coordinate& right){
    Coordinate result = left;
    result.x /= right.x;
    result.y /= right.y;
    result.z /= right.z;
    return result;
}

bool operator==(const Coordinate& left, const Coordinate& right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

struct CoordinateHash {
    std::size_t operator()(const Coordinate& c) const noexcept {
        std::size_t xh = std::hash<long>{}(c.x);
        std::size_t yh = std::hash<long>{}(c.y);
        std::size_t zh = std::hash<long>{}(c.z);

        return xh^(yh<<1)^(zh<<2);
    }
};

std::ostream& operator<<(std::ostream& o, const Coordinate& c){
    o << "(" << c.x << "," << c.y << "," << c.z << ")";
    return o;
}

Coordinate dist(const Coordinate& left, const Coordinate& right){
    Coordinate coord;
    coord.x = std::abs(left.x - right.x);
    coord.y = std::abs(left.y - right.y);
    coord.z = std::abs(left.z - right.z);

    return coord;
}

long l2norm(const Coordinate& left, const Coordinate& right){
    auto c = left - right;
    return c.x*c.x + c.y*c.y + c.z*c.z;
}

long manhattan(const Coordinate& left, const Coordinate& right){
    Coordinate c = left - right;
    return std::abs(c.x) + std::abs(c.y) + std::abs(c.z);
}

using Map = std::pair<int, int>;

struct Scanner {
    int64_t id;
    std::optional<Coordinate> position;
    std::vector<Coordinate> beacons;
    DMatrix distances;
    DMatrix sorted_distances;

    explicit Scanner(int64_t i,  std::vector<Coordinate>& b): 
    id(i),
    position(std::nullopt),
    beacons(b),
    distances(compute_distance_matrix(beacons)),
    sorted_distances(distances)
    {
        for (int i = 0; i < distances.size(); i++){
            std::stable_sort(sorted_distances[i].begin(), sorted_distances[i].end());
        }
    }

    DMatrix compute_distance_matrix(const std::vector<Coordinate>& b){
        std::vector<long> row(b.size(), 0);
        DMatrix d(b.size(), row);
        for (long i = 0; i < b.size(); ++i){
            for (long j = i+1; j < b.size(); ++j){
                d[i][j] = d[j][i] = l2norm(b[i], b[j]);
            }
        }
        return d;
    }
    
    /**
     * Move the coordinates of this scanner 
     * and its beacons to the reference frame of the dest scanner
     */
    void translate(const Scanner& dest, const std::vector<Map>& map){
        std::cout << "Translating scanner " << id << " to reference frame of scanner " << dest.id << "\n";
        std::vector<Coordinate> dest_v;
        std::vector<Coordinate> curr_v;
        for (const auto& rot:rotations){
            std::cout << "//////////////////////////////////////////" << std::endl;
            for (auto [i,j]: map){
                curr_v.push_back(rot*(this->beacons[j]));
                dest_v.push_back(dest.beacons[i]);
                std::cout << dest.beacons[i] << "  ->  " << beacons[j] << std::endl;
            }
            Coordinate displacement = dest_v[0] - curr_v[0];
            std::cout << "Displacement: \n"; 
            std::cout << displacement << std::endl;
            std::cout << dest_v[0] << "  ->  " << curr_v[0] << std::endl;
            std::transform(curr_v.begin(), curr_v.end(), curr_v.begin(), [&displacement](Coordinate& c){ return c+displacement;});
            if (std::equal(curr_v.begin(), curr_v.end(), dest_v.begin())){
                std::transform(beacons.begin(), beacons.end(), beacons.begin(), [&displacement, &rot](Coordinate& c){ return rot*c + displacement; });
                this->position = displacement;
                return;
            }
            curr_v.clear();
            dest_v.clear();
        }

        assert(0 && "No good rotation or displacement found.");        
        
    }
};

std::vector<Map> find_common_beacons(const Scanner& s1, const Scanner& s2){
    const auto& d1 = s1.distances;
    const auto& sd1 = s1.sorted_distances;
    const auto& d2 = s2.distances;
    const auto& sd2 = s2.sorted_distances;

    std::vector<Map> maps;
    
    for (int i = 0; i < sd1.size(); i++){
        for (int j = 0; j < sd2.size(); j++){
            auto iter1 = sd1[i].cbegin();
            auto iter2 = sd2[j].cbegin();
            int count = 0;
            while (iter1 != sd1[i].cend() && iter2 != sd2[j].cend() && count < 12){
                if (*iter1 == *iter2){
                    count++;
                    iter1++;
                    iter2++;
                } else if (*iter1 < *iter2){
                    iter1++;
                } else {
                    iter2++;
                }
            }
            if (count >= 12){
                std::vector<bool> used(d1[i].size(), 0);
                for (int k = 0; k < d1[i].size(); k++){
                    for (int l = 0; l < d2[j].size(); l++){
                        if (!used[l] && d1[i][k] == d2[j][l]){
                            used[l] = true;
                            maps.emplace_back(k,l);
                        }
                    }
                }
                assert(maps.size() >= 12 && "Maps did not capture all matches");
                return maps;
            }
        }
    }
    return maps;
}

int main(int argc, char* argv[]){
    std::fstream file(argv[1]);
    std::string input;

    std::vector<Scanner> scanners, known;
    std::vector<Coordinate> values;

    uint64_t id = 0;

    while(std::getline(file, input)){
        if (input[0] == '-' && input[1] == '-') { continue;}
        if(input.empty()) {
            scanners.emplace_back(id++, values);
            values.clear();
            continue;
        }
        int x,y,z;
        sscanf(input.c_str(), "%d,%d,%d", &x, &y, &z);
        Coordinate coord{x,y,z};
        values.push_back(coord);
    }
    scanners.emplace_back(id++, values);

    std::cout << "Scan readings parsed. Number of scanners: " << scanners.size() << "\n"; 
    
    scanners[0].position = {0,0,0};
    known.push_back(scanners[0]);
    while (known.size() < scanners.size()){
        auto sz = std::size(known);
        for (int i = 0; i < sz; i++){
            const auto& sc = known[i];
            for (auto& p: scanners){
                if (p.position) continue;
                // std::cout << "Finding common beacons between scanner " << sc.id << " and " << p.id << std::endl;
                auto map = find_common_beacons(sc, p);
                // std::cout << map.size() << std::endl;
                if ( map.size() >= 12){
                    // std::cout << "Found common beacons" << std::endl;
                    p.translate(sc, map);
                    known.push_back(p);
                    break; 
                }
            }
        }
    }

    std::unordered_set<Coordinate, CoordinateHash> unique_beacons;
    for (auto& s:known){
        for (auto& beacon:s.beacons){
            unique_beacons.insert(beacon);
        }
    }


    std::cout << "Number of beacons: " << unique_beacons.size() << std::endl;
    return 0;
}