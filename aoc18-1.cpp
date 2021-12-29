#include <numeric>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <cstdint>
#include <sstream>
#include <memory>
#include <optional>

struct SnailNumber {
    SnailNumber() = default;
    SnailNumber(SnailNumber&&) = default;
    SnailNumber& operator=(SnailNumber&&) = default;

    SnailNumber(uint64_t value): left_(nullptr), right_(nullptr), value_(value) {}
    SnailNumber(const SnailNumber& other): left_(nullptr), right_(nullptr), value_(other.value_) {
        if (other.is_terminal())
            return;
        left_ = std::make_unique<SnailNumber>(*other.left_);
        right_ = std::make_unique<SnailNumber>(*other.right_);
    }
    SnailNumber& operator=(const SnailNumber& other){
        if (this == &other)
            return *this;
        if (other.is_terminal()){
            value_ = other.value_;
            left_ = nullptr;
            right_ = nullptr;
            return *this;
        }

        left_ = std::make_unique<SnailNumber>(*other.left_);
        right_ = std::make_unique<SnailNumber>(*other.right_);
        return *this;
    }

    explicit operator uint64_t() const {
        if (is_terminal())
            return value_;
        return 3 * static_cast<uint64_t>(*left_) + 2 * static_cast<uint64_t>(*right_);
    }

    private:
    std::unique_ptr<SnailNumber> left_;
    std::unique_ptr<SnailNumber> right_;
    uint64_t value_;

    bool is_terminal() const {return left_ == nullptr && right_ == nullptr;}
    bool explodes() {
        std::optional<uint64_t> spill;
        std::optional<uint64_t*> last_value;
        return lr_search(0, last_value, spill) || spill.has_value();
    }
    bool lr_search(uint32_t depth, std::optional<uint64_t*>& last_value, std::optional<uint64_t> &spill){
        if (is_terminal()){
            if (spill.has_value()){
                value_ += spill.value();
                spill.reset();
                return true;
            }
            last_value = &value_;
            return false;
        }

        if (depth < 4 || spill.has_value() || !left_->is_terminal() || !right_->is_terminal()){
            return left_->lr_search(depth+1, last_value, spill) || right_->lr_search(depth+1, last_value, spill);
        }

        if (last_value.has_value()){
            (*last_value.value()) += left_->value_;
        }
        spill = right_->value_;
        left_ = nullptr;
        right_ = nullptr;
        value_ = 0;
        return false; 
    }
    bool splits() {
        if (!is_terminal()){
            if (left_->splits())
                return true;
            return right_->splits();
        }
        if (value_ < 10)
            return false;
        
        left_ = std::make_unique<SnailNumber>(value_/2);
        right_ = std::make_unique<SnailNumber>((value_+1)/2);
        return true;
    }

    friend std::istream& operator>>(std::istream& s, SnailNumber& number);
    friend std::ostream& operator<<(std::ostream&, const SnailNumber&);
    friend SnailNumber operator+(const SnailNumber& left, const SnailNumber& right);

};

std::istream& operator>>(std::istream& s, SnailNumber& number){
    char c;
    if (!(s >> c))
        return s;
    if (c != '[')
        throw std::runtime_error("Expected left bracket.");
    number.left_ = std::make_unique<SnailNumber>();
    if (s.peek() == '['){
        s >> *number.left_;
    } else {
        s >> number.left_->value_;
    }
    if (!(s >> c) || c != ',')
        throw std::runtime_error("Expected delimited between left and right side of number.");
    number.right_ = std::make_unique<SnailNumber>();
    if (s.peek() == '['){
        s >> *number.right_;
    } else {
        s >> number.right_->value_;
    }
    if (!(s >> c) || c != ']')
        throw std::runtime_error("Expected right bracket.");
    return s; 
}

std::ostream& operator<<(std::ostream& s, const SnailNumber& number){
    if (number.is_terminal()){
        s << number.value_;
        return s;
    }
    s << "[" << *number.left_ << "," << *number.right_ << "]";
    return s;
}

SnailNumber operator+(const SnailNumber& left, const SnailNumber& right){
    SnailNumber result;
    result.left_ = std::make_unique<SnailNumber>(left);
    result.right_ = std::make_unique<SnailNumber>(right);

    do { //reduce the snail numbers
        while (result.explodes());
    } while (result.splits());
    return result;
}

int main(int argc, char* argv[]){
    if (argc != 2){
        std::cerr << "This program expects one parameter that is the input file.\n";
        return EXIT_FAILURE;
    }
    std::ifstream file(argv[1]);
    if (!file.is_open()){
        std::cerr << "Failed to open file\n";
        return EXIT_FAILURE;
    }

    std::vector<SnailNumber> numbers;
    std::string input;

    while(std::getline(file, input)){
        std::stringstream s(input);
        SnailNumber sn;
        s >> sn;
        numbers.push_back(sn);
    }

    SnailNumber sum = std::accumulate(numbers.begin()+1, numbers.end(), numbers[0], std::plus<>());
    std::cout << "The magnitude is: " << static_cast<uint64_t>(sum) << "\n";

    return 0;
}