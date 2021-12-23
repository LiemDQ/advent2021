#include <vector>
#include <stack>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <optional>

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

int get_completion_score(char brack){
	switch(brack){
		case ')':
			return 1;
		case ']':
			return 2;
		case '}':
			return 3;
		case '>':
			return 4;
		default:
			return 0;
	}
}

long complete_brackets(std::stack<char>& stack){
	long score = 0;
	while(!stack.empty()){
		score *= 5;
		char c = stack.top();
		score += (long) get_completion_score(match_start_bracket(c));
		stack.pop();
	}
	return score; 
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

struct LegalScore {
	bool is_legal;
	int score;
};

std::optional<long> get_score(const std::string& str){
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
				//std::cout << "Expected " << match_start_bracket(b) << " but found " << c << " instead" << std::endl;
				return {};
			}
		}
	}
	return std::optional<long>{complete_brackets(stack)};
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
	std::vector<std::string> brackets;
	std::string inputs;
	while(std::getline(file, inputs)){
		brackets.push_back(inputs);
	}

	std::vector<long> score; 
	int line = 1;
	for (auto& bracket_line:brackets){
		//std::cout << line++ << ": ";
		auto result = get_score(bracket_line);
		if(result){
			score.push_back(*result);
		}
	}

	std::sort(score.begin(), score.end());
	auto index = score.size()/2;
	for(auto&& val:score){
		std::cout << val << std::endl;
	}
	std::cout << "Final score is: " << score[index] << std::endl;

	return 0;
}