#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string_view>
#include <sstream>
#include <array>
#include <cstring>
#include <optional>

using ModelNumber = std::array<int, 14>;

class ALU {
    std::unordered_map<char, long> registers{{'w', 0}, {'x', 0}, {'y', 0}, {'z',0}};
    public:
        long get(char reg){
            return registers.at(reg);
        }

        void inp(char reg, int val){
            registers.at(reg) = val;
        }

        void add(char a, char b){
            registers.at(a) = registers.at(a) + registers.at(b);
        }

        void mul(char a, char b){
            registers.at(a) = registers.at(a)*registers.at(b);
        }

        void div(char a, char b){
            registers.at(a) = registers.at(a)/registers.at(b);
        }

        void mod(char a, char b){
            registers.at(a) = registers.at(a) % registers.at(b);
        }

        void eql(char a, char b){
            registers.at(a) = registers.at(a) == registers.at(b) ? 1 : 0;
        }

        void add(char a, long b){
            registers.at(a) = registers.at(a) + b;
        }

        void mul(char a, long b){
            registers.at(a) = registers.at(a) * b;
        }

        void div(char a, long b){
            registers.at(a) = registers.at(a) / b;
        }

        void mod(char a, long b){
            registers.at(a) = registers.at(a) % b;
        }

        void eql(char a, long b){
            registers.at(a) = registers.at(a) == b ? 1 : 0;
        }

        void clear(){
            registers['w'] = 0L;
            registers['x'] = 0L;
            registers['y'] = 0L;
            registers['z'] = 0L;
        }
};

void execute_instruction(ALU& alu, std::string_view instr, std::string_view a, std::string_view b){
    if (instr == "add"){
        alu.add(a[0], b[0]);
    } else if (instr == "mul"){
        alu.mul(a[0], b[0]);
    } else if (instr == "div"){
        alu.div(a[0], b[0]);
    } else if (instr == "mod"){
        alu.mod(a[0], b[0]);
    } else if (instr == "eql"){
        alu.eql(a[0], b[0]);
    }
}

void execute_instruction(ALU& alu, std::string_view instr, std::string_view a, long b){
    if (instr == "add"){
        alu.add(a[0], b);
    } else if (instr == "mul"){
        alu.mul(a[0], b);
    } else if (instr == "div"){
        alu.div(a[0], b);
    } else if (instr == "mod"){
        alu.mod(a[0], b);
    } else if (instr == "eql"){
        alu.eql(a[0], b);
    }
}


enum ParseResult {
    ALU_INPUT,
    ALU_NUMBER,
    ALU_REGISTER
};

 ParseResult parse_input(ALU& alu, const std::string& str, int num){
    assert(num >= 0 && num < 10 && "invalid digit given");
    std::stringstream ss(str);
    std::string instruction;
    std::string a, b;
    
    ss >> instruction;
    if (instruction == "inp"){
        ss >> a;
        alu.inp(a[0], num);
        return ALU_INPUT;
    } else {
        ss >> a >> b;
        for (char c:b){
            if (!isdigit(c) && c != '-'){
                execute_instruction(alu, instruction, a, b);
                return ALU_REGISTER;
            }
        }
        long b_val = b[0] - '0';
        execute_instruction(alu, instruction, a, b_val);
        return ALU_NUMBER;
    }
}

int execute_instructions(ALU& alu, const std::vector<std::string>& instructions, const ModelNumber& num_array){
    size_t counter = 0;
    for (const auto& instr:instructions){
        ParseResult result = parse_input(alu, instr, num_array.at(counter));
        if (result == ALU_INPUT && counter < num_array.size()-1){
            counter++;
        }        
    }
    return alu.get('z');
}

std::optional<ModelNumber> generate_model_number(unsigned long value, bool verbose = false){
    //assert(value < 100'000'000'000'000 && value > 10'000'000'000'000 && "Value is not in the correct range");
    ModelNumber array;
    for (int i = 0; i < array.size(); i++){
        array[i] = value % 10;
        value /= 10;
        if (array[i] == 0)
            return std::nullopt;
    }
    if (verbose){
        std::cout << "Generated number: ";
        for (int i = array.size()-1; i >= 0; i--){
            std::cout << array[i] << ",";
        }
        std::cout << "\n";
    }

    return array;
}


int main(int argc, char* argv[]){
    std::fstream file(argv[1]);
    if(!file.is_open()){
        std::cerr << "Could not open file " << argv[1] << std::endl;
        return 1;
    }

    ALU alu;

    std::vector<std::string> instructions;
    std::string input;
    while(std::getline(file, input)){
        instructions.push_back(input);
    }

    auto num = generate_model_number(11'111'111'111'111UL);
    std::array<unsigned long, 20> nums{
        (11'111'111'111'111UL),
        (21'121'111'111'111UL),
        (31'131'111'111'111UL),
        (41'141'111'111'111UL),
        (51'151'111'111'111UL),
        (61'161'111'111'111UL),
        (71'171'111'111'111UL),
        (81'181'111'111'111UL),
        (91'191'111'111'111UL),
        (11'112'111'111'111UL),
        (11'113'111'111'111UL),
        (11'114'111'111'111UL),
        (11'115'111'111'111UL),
        (11'116'111'111'111UL),
        (11'117'111'111'111UL),
        (11'118'111'111'111UL),
        (11'119'111'111'111UL),
        (11'111'211'111'111UL),
        (91'699'394'894'995UL)
    };
    
    for (auto& v:nums){
        auto num = generate_model_number(v, true);
        int result = execute_instructions(alu, instructions, *num);
        std::cout << "Result: " << result << std::endl;
        alu.clear();
        // std::cout << "Full register breakdown - w:" << alu.get('w') << " x:" << alu.get('x') << " y:" 
        //     << alu.get('y') << " z:" << alu.get('z') << "\n";   

    }    

    // long max_val = 99'999'999'999'999;
    // bool done = false;
    // while(max_val > 11'111'111'111){
    //     auto num = generate_model_number(max_val);
    //     if (num){
    //         long result = execute_instructions(alu, instructions, *num);
    //         if (result == 0){
    //             break;
    //         }
    //     }
    //     max_val--;
    // }

    // std::cout << "Highest number: " << max_val;


    return 0;
}