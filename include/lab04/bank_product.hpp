#pragma once

#include <iosfwd>
#include <memory>

namespace lab04 {

class BankProduct {
public:
    static constexpr double kEps{1e-6};

    virtual ~BankProduct() = default;

    virtual double metric() const = 0;
    virtual std::unique_ptr<BankProduct> Clone() const = 0;

    explicit operator double() const;
    bool operator==(const BankProduct& other) const;

protected:
    BankProduct() = default;
    BankProduct(const BankProduct&) = default;
    BankProduct& operator=(const BankProduct&) = default;
    BankProduct(BankProduct&&) = default;
    BankProduct& operator=(BankProduct&&) = default;

private:
    virtual void Print(std::ostream& output) const = 0;
    virtual void Read(std::istream& input) = 0;

    friend std::ostream& operator<<(std::ostream& output, const BankProduct& product);
    friend std::istream& operator>>(std::istream& input, BankProduct& product);
};

std::ostream& operator<<(std::ostream& output, const BankProduct& product);
std::istream& operator>>(std::istream& input, BankProduct& product);

}  // namespace lab04
