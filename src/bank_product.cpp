#include "lab04/bank_product.hpp"

#include <cmath>
#include <istream>
#include <ostream>

namespace lab04 {

BankProduct::operator double() const {
    return metric();
}

bool BankProduct::operator==(const BankProduct& other) const {
    return std::abs(metric() - other.metric()) < kEps;
}

std::ostream& operator<<(std::ostream& output, const BankProduct& product) {
    product.Print(output);
    return output;
}

std::istream& operator>>(std::istream& input, BankProduct& product) {
    product.Read(input);
    return input;
}

}  // namespace lab04
