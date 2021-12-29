#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

struct Coordinate {
	int x;
	int y;
};

int sumOfAllInts(int n1, int n2 = 0){
	return (n1+1)*n1/2 - (n2+1)*n2/2;
}

int getHeightFromVelocity(int initial_y_vel){
	return sumOfAllInts(initial_y_vel);
}

bool isInSummationBounds(int min_dist, int max_dist){
	int n = 1;
	int val = sumOfAllInts(n);
	bool result = false; 
	while (val <= max_dist){
		if (val >= min_dist){
			result = true;
		}
		n++;
		val = sumOfAllInts(n);
	}
	return result;
}

int getMaximumHeight(int y_min, int y_max, int n_steps){
	int i = n_steps;
	int height = getHeightFromVelocity(i);
	int diff = height - getHeightFromVelocity(i-1);
	int spacing = y_max - y_min;
	int result = 0;
	std::cout << height << std::endl;
	for (; i <= 1000; i++){
		
		height = getHeightFromVelocity(i);
		if (isInSummationBounds(height - y_max, height - y_min)){
			result = height;
		}
	}
	return result;
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
	int peak_y = getMaximumHeight(y_min, y_max, n_steps);
	std::cout << "Highest y: " << peak_y << std::endl;

	return 0;
}