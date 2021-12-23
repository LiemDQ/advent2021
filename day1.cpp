#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

constexpr int window_size = 3;

bool is_increase(int first, int second){
	return second > first; 
}

int sum_window(std::vector<int>& vec, int index){
	int sum = 0;
	for (int i = 0; i < window_size; i++){
		sum += vec[index+i];
	}
	return sum;
}

int main(int argc, char* argv[]){
	std::vector<int> depths;
	//std::string filename = argv[0];
	std::ifstream file{argv[1]};
	//read from file here
	if (!file.is_open()){
		std::cerr << "Could not open the file: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}
	
	int a;
	while (file >> a){
		depths.push_back(a);	
	}
	file.close();
	
	std::cout << "File read. Entries found: " << depths.size() << std::endl; 

	int count = 0;
	for (int i = 0; i < depths.size()- window_size; i++){
		int a = sum_window(depths, i);
		int b = sum_window(depths, i +1);
		if (is_increase(a,b))
			count++;
	}
	std::cout << "Count: " << count << std::endl;
	
	return 0;
}