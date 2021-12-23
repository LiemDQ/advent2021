#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

struct Coordinate {
	int x;
	int y;
};

int sumOfAllInts(int n1, int n2 = 1){
	return (n1+1)*n1/2 - (n2+1)*n2/2;
}

int get_n_steps(int min_x, int max_x){
	int n = 0; 
	int val = sumOfAllInts(n);
	while (val< max_x){
		val = sumOfAllInts(n);
		if (val > max_x){
			return --n; 
		}
		n++;
	}
	return 0;
}

int highest_y(int n_steps, const int y_min, const int y_max){
	
	int y= sumOfAllInts(n_steps);
	while (n_steps > 0){
	//check that the probe falls into the zone
		y = sumOfAllInts(n_steps);
		int dy1 = y - y_min;
		int dy2 = y - y_max;
		if (get_n_steps(dy1, dy2)){
			return y;
		}
		n_steps--;
	}
	return 0;
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

	int n_steps = get_n_steps(x_min, x_max);
	std::cout << "n steps: " <<  n_steps << std::endl;
	int peak_y = highest_y(n_steps, y_min, y_max);
	std::cout << "Highest y: " << peak_y << std::endl;

	return 0;
}