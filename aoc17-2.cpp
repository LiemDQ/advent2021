#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

class Probe {
	public:
	Probe(int x, int y) : x_vel(x), y_vel(y) {}
	int x_vel;
	int y_vel;
	void update(){
		x += x_vel;
		y += y_vel;
		if (x_vel > 0){
			x_vel--;
		} else if (x_vel < 0){
			x_vel++;
		}
		y_vel--;
	}

	int y = 0;
	int x = 0;
};

struct Zone {
	int min_x;
	int max_x; 
	int min_y;
	int max_y;
	bool isInsideZone(Probe& probe){
		return (probe.x >= min_x && probe.x <= max_x) && (probe.y >= min_y && probe.y <= max_y); 
	}
};

int sumOfAllInts(int n1, int n2 = 0){
	return (n1+1)*n1/2 - (n2+1)*n2/2;
}

int get_n_steps(int min_x, int max_x){
	int n = 1; 
	int val = sumOfAllInts(n);

	while (val< max_x){
		n++;
		val = sumOfAllInts(n);
		if (val > min_x){
			return n; 
		}
	}
	return 0;
}

bool simulate(int x_vel, int y_vel, Zone& zone){
	Probe probe(x_vel, y_vel);
	while(probe.x <= zone.max_x && probe.y >= zone.min_y){
		if (zone.isInsideZone(probe)){
			return true;
		}
		probe.update();
	}
	return false;
}
int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
    if (!file.is_open()){
		std::cerr << "Could not open file: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	int x_min, x_max, y_min, y_max;
	std::string input;
	std::getline(file, input);
	
	sscanf(input.c_str(), "target area: x=%d..%d, y=%d..%d", &x_min, &x_max, &y_min, &y_max);
	std::cout << "x: " << x_min << "," << x_max << std::endl;
	std::cout << "y: " << y_min << "," << y_max << std::endl;

	Zone zone{x_min, x_max, y_min, y_max};

	//find reasonable bounds for x and y velocity guesses
	int min_x_guess = get_n_steps(zone.min_x, zone.max_x);
	int max_x_guess = zone.max_x;

	int min_y_guess = zone.min_y;
	int max_y_guess = 1000;
	int count = 0;
	for (int i = min_x_guess; i <= max_x_guess; i++){
		for (int j = min_y_guess; j <= max_y_guess; j++){
			if (simulate(i, j, zone)){
				count++;
			}
		}
	}	

	std::cout << "Num of successful simulations: " << count << std::endl;

	return 0;
}