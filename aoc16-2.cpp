#include <vector>
#include <algorithm>
#include <cctype>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <cmath>
#include <numeric>
#include <cassert>

const char* hex_char_to_bin(char c){
	switch(std::toupper(c)){
		case '0': return "0000";
		case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
		default: return "";
    }
}

struct Header {
    unsigned long version;
    unsigned long id;
};

struct Packet {
    Header header;
    bool is_value;
    bool is_bit_length;
    unsigned long value;
    std::vector<Packet> children;
    Packet* parent = nullptr;
};

struct Tokenizer {
    std::string::const_iterator at; 
};

unsigned long getPacketType(Tokenizer& c){
    std::bitset<3> bits;
    for (int i = 2; i >= 0; i--){
        bits[i] = *c.at++ == '0' ? 0 : 1;
    }
    return bits.to_ulong();
}

unsigned long getPacketVersion(Tokenizer& c){
    return getPacketType(c);
}

bool isLengthInBits(Tokenizer& c){
    return *(c.at++) == '0' ? true : false; 
}

unsigned long getBitLength(Tokenizer& c){
    constexpr unsigned bit_length = 15;
    std::bitset<bit_length> bits;
    for (int i = bit_length-1; i >= 0; i-- ){
        bits[i] = *c.at++ == '0' ? 0 : 1;
    }
    return bits.to_ulong();
}

unsigned long getNumPackets(Tokenizer& c){
    constexpr unsigned bit_length = 11;
    std::bitset<bit_length> bits;
    for (int i = bit_length-1; i >= 0; i--){
        bits[i] = *c.at++ == '0' ? 0 : 1;
    }
    return bits.to_ulong();
}

Header getPacketHeader(Tokenizer& tk){
    Header header;
    header.version = getPacketVersion(tk);
    header.id = getPacketType(tk);
    return header; 
}

unsigned long getValue(Tokenizer& c){
    unsigned bit_count = 1;
    bool running = true;
    constexpr unsigned bit_length = 4;

    std::string str;
    while (running){
        running = *c.at++ == '0' ? 0 : 1;
        for (int i = bit_length-1; i >= 0; i--){
            str += *c.at++;
        }
    }
  
    //std::cout << str << std::endl;
    return std::stol(str, nullptr, 2); 
}

Packet getPacket(Tokenizer& tk){
    //std::cout << "Getting packet" << std::endl;
    Header header = getPacketHeader(tk);
    Packet packet{header};
    if (header.id == 4){
        packet.is_value = true;
        packet.value = getValue(tk);
        //std::cout << "Packet value: " << packet.value << std::endl;
    } else {
        packet.is_value = false;
        packet.is_bit_length = isLengthInBits(tk);

        if (packet.is_bit_length){
            //std::cout << "Getting bit length\n";
            packet.value = getBitLength(tk);
            auto current_bit = tk.at;
            //std::cout << "Bit length: " << packet.value << std::endl;
            auto end = current_bit + packet.value;
            while(tk.at < end){
                Packet child = getPacket(tk);
                child.parent = &packet;
                packet.children.push_back(child);
            }
        } else {
            //std::cout << "Getting num packets\n";
            packet.value = getNumPackets(tk);
            for (unsigned long i = 0; i < packet.value; i++){
                Packet child = getPacket(tk);
                child.parent = &packet;
                packet.children.push_back(child);
            }
        }
    }
    return packet; 
}

unsigned long extractVersionTotal(Packet& packet){
    unsigned long version_total = packet.header.version;
    for (Packet& child:packet.children){
        version_total += extractVersionTotal(child);
    }
    return version_total;
}

unsigned long extractCalculatedValue(Packet& packet){
    switch(packet.header.id){
        case 0: {
            return std::accumulate(packet.children.begin(), packet.children.end(), (long) 0,
                 [](unsigned long value, Packet& packet){ return value + extractCalculatedValue(packet);});
        }
        case 1: {
            return std::accumulate(packet.children.begin(), packet.children.end(), (long) 1,
                 [](unsigned long value, Packet& packet){ return value * extractCalculatedValue(packet);});
        }
        case 2: {
            Packet min_packet = *std::min_element(packet.children.begin(), packet.children.end(), 
                [](Packet& p1, Packet& p2){return extractCalculatedValue(p1) < extractCalculatedValue(p2);});
            return extractCalculatedValue(min_packet);
        }
        case 3: {
            Packet max_packet = *std::max_element(packet.children.begin(), packet.children.end(), 
                [](Packet& p1, Packet& p2){return extractCalculatedValue(p1) < extractCalculatedValue(p2);});
            return extractCalculatedValue(max_packet);
        }
        case 4: {
            return packet.value;
        }
        case 5: {
            return extractCalculatedValue(packet.children.front()) > extractCalculatedValue(packet.children.back()) ? 1 : 0;
        }
        case 6: {
            return extractCalculatedValue(packet.children.front()) < extractCalculatedValue(packet.children.back()) ? 1 : 0;
        } 
        case 7: {
            return extractCalculatedValue(packet.children.front()) == extractCalculatedValue(packet.children.back()) ? 1 : 0;
        }
        default:
            assert(0 && "Unreachable branch");
    }
}

unsigned long test(std::string hex){
    std::string bin;
    for (char c: hex){
        bin += hex_char_to_bin(c);
    }
    Tokenizer tk{bin.begin()};
    Packet pk = getPacket(tk);
    return extractCalculatedValue(pk);
}

int main(int argc, char* argv[]){
	std::fstream file(argv[1]);
    if (!file.is_open()){
		std::cerr << "Could not open file: " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	std::string input, binary;
	std::getline(file, input);
	for (char c:input){
		binary += hex_char_to_bin(c);
	}

	assert(test("C200B40A82") == 3);
    assert(test("04005AC33890") == 54);
    assert(test("880086C3E88112") == 7);
    assert(test("CE00C43D881120") == 9);
    assert(test("D8005AC2A8F0") == 1);
    assert(test("F600BC2D8F") == 0);
    assert(test("9C005AC2F8F0") == 0);
    assert(test("9C0141080250320F1802104A08") == 1);
    


    Tokenizer tokenizer; 
    tokenizer.at = binary.begin();
    Packet packet = getPacket(tokenizer);

    unsigned long v_total = extractCalculatedValue(packet);
    std::cout << "Total value: " << v_total << std::endl;   

	return 0;
}