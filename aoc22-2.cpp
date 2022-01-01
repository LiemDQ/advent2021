#include <fstream>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <numeric>
#include <optional>
#include <list>

enum class Status {
    ON,
    OFF
};

struct Cuboid {
    Cuboid(int x0, int x1, int y0, int y1, int z0, int z1) 
    : min_x(x0), max_x(x1), min_y(y0), max_y(y1), min_z(z0), max_z(z1) {}
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    int min_z;
    int max_z;
};

struct CuboidAndStatus {
    Cuboid cuboid;
    Status status;
};

std::ostream& operator<<(std::ostream& o, const Cuboid& cb){
    o << "(" << cb.min_x << "," << cb.max_x << "), (" << cb.min_y << "," << cb.max_y << "), (" << cb.min_z << "," << cb.max_z << ")";
    return o; 
}

using Cuboids = std::vector<Cuboid>;

long volume(const Cuboid& box){
    return long(box.max_x- box.min_x)*long(box.max_y-box.min_y)*long(box.max_z-box.min_z);
}

long volume(const Cuboids& boxes){
    return std::accumulate(boxes.cbegin(), boxes.cend(), 0L, [](long sum, const Cuboid& box){ return sum + volume(box); });
}

std::optional<Cuboids> clip_and_split(Cuboid& c1, const Cuboid& c2){
    const int x_min = std::max(c1.min_x, c2.min_x);
    const int x_max = std::min(c1.max_x, c2.max_x);
    const int y_min = std::max(c1.min_y, c2.min_y);
    const int y_max = std::min(c1.max_y, c2.max_y);
    const int z_min = std::max(c1.min_z, c2.min_z);
    const int z_max = std::min(c1.max_z, c2.max_z);
    //in order for two cuboids to clip,
    //the largest min dimension must be smaller than the smallest max dimension
    if (x_min < x_max && y_min < y_max && z_min < z_max){
        Cuboids cuboids; //vector of overlapping areas
        //slice boxes along the clipping planes
        if (c1.min_x < x_min && x_min < c1.max_x){
            //c1 is on the "left" side
            cuboids.emplace_back(c1.min_x, x_min, c1.min_y, c1.max_y, c1.min_z, c1.max_z);
            c1.min_x = x_min;
        }
        if (c1.min_x <= x_max && x_max < c1.max_x){
            //c1 is on the "right" side
            cuboids.emplace_back(x_max, c1.max_x, c1.min_y, c1.max_y, c1.min_z, c1.max_z);
            c1.max_x = x_max;
        }
        if (c1.min_y < y_min && y_min < c1.max_y){
            cuboids.emplace_back(c1.min_x, c1.max_x, c1.min_y, y_min, c1.min_z, c1.max_z);
            c1.min_y = y_min;
        }
        if (c1.min_y <= y_max && y_max < c1.max_y){
            cuboids.emplace_back(c1.min_x, c1.max_x, y_max, c1.max_y, c1.min_z, c1.max_z);
            c1.max_y = y_max;
        }
        if (c1.min_z < z_min && z_min < c1.max_z){
            cuboids.emplace_back(c1.min_x, c1.max_x, c1.min_y, c1.max_y, c1.min_z, z_min);
            c1.min_z = z_min;
        }
        if (c1.min_z <= z_min && z_min < c1.max_z && z_max < c1.max_z ){
            cuboids.emplace_back(c1.min_x, c1.max_x, c1.min_y, c1.max_y, z_max, c1.max_z);
            //c1.max_z = z_max;
        }
        //resulting vector is a collection of cuboids covering c2 EXCEPT where c1 intersects with c2
        return cuboids;
    }

    return std::nullopt;
}

void clip_cuboid(Cuboids& cuboids, Cuboid& cb){
    Cuboids clipped;
    for (auto& cuboid:cuboids){
        auto split_cuboids = clip_and_split(cuboid, cb);
        if (split_cuboids){
            clipped.insert(clipped.begin(), split_cuboids->begin(), split_cuboids->end());
        } else {
            clipped.emplace_back(cuboid);
        }
    }
    cuboids.swap(clipped);
}

int main(int argc, char* argv[]){
    std::fstream file(argv[1]);
    if(!file.is_open()){
        std::cerr << "Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::string input; 
    int x0, x1, y0, y1, z0, z1;
    std::vector<CuboidAndStatus> cuboids;
    while(std::getline(file, input)){
        char status[4]; 
        std::sscanf(input.c_str(), "%3s x=%d..%d,y=%d..%d,z=%d..%d", &status[0], &x0, &x1, &y0, &y1,&z0, &z1);
        Status rule = status[1] == 'n' ? Status::ON : Status::OFF;
        Cuboid cb{x0, ++x1, y0, ++y1, z0, ++z1};
        cuboids.emplace_back(CuboidAndStatus{cb, rule});
    }

    Cuboids non_clipping_boxes_1;
    for (auto& [cb, status]:cuboids){
        clip_cuboid(non_clipping_boxes_1, cb);
        if (status == Status::ON){
            non_clipping_boxes_1.push_back(cb);
        }
        
    }

    

    std::cout << "Total volume: " << volume(non_clipping_boxes_1) << std::endl;

    return 0;
}