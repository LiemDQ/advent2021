#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <iostream>
#include <cassert>

char match_end_bracket(char brack){
	switch (brack) {
		case ')':
			return '(';
		case '}':
			return '{';
		case ']':
			return '[';
		case '>':
			return '<';
		default:
			return brack;
	}
}

char match_start_bracket(char brack){
	switch (brack) {
		case '(':
			return ')';
		case '{':
			return '}';
		case '[':
			return ']';
		case '<':
			return '>';
		default:
			return brack;
	}
}

int get_illegal_score(char brack){
	switch (brack){
		case ')':
			return 3;
		case ']':
			return 57;
		case '}':
			return 1197;
		case '>':
			return 25137;
		default:
			return 0;
	}
}

bool is_open_bracket(char brack){
	if(brack == '(' || brack == '{' || brack == '[' || brack == '<'){
		return true;
	} else if (brack == ')' || brack == '}' || brack ==  ']' || brack == '>') {
		return false; 
	} else {
		assert(0 && "This branch should be unreachable; parse error encountered");
		return false; 
	}
}

int get_score(const std::string& str){
	std::stack<char> stack;
	for(char c: str){
		if(is_open_bracket(c)){
			stack.push(c);
		} else {
			char b = stack.top();
			if (b == match_end_bracket(c)){
				stack.pop();
				continue;
			} else {
				std::cout << "Expected " << match_start_bracket(b) << " but found " << c << " instead" << std::endl;
				return get_illegal_score(c);
			}
		}
	}
	return 0;
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	std::vector<std::string> brackets;
	std::string inputs;
	while(std::getline(file, inputs)){
		brackets.push_back(inputs);
	}

	int score; 
	int line = 1;
	for (auto& bracket_line:brackets){
		//std::cout << line++ << ": ";
		score += get_score(bracket_line);
	}

	std::cout << "Final score is: " << score << std::endl;

	return 0;
}