#pragma once

#include <iosfwd>
#include <memory>

#include "lab04/bank_product.hpp"

namespace lab04 {

class Deposit final : public BankProduct {
public:
    Deposit() = default;
    Deposit(double sum, double rate, int years, bool cumulative);

    double metric() const override;
    std::unique_ptr<BankProduct> Clone() const override;
    bool isCumulative() const;

private:
    void Print(std::ostream& output) const override;
    void Read(std::istream& input) override;

    double sum_{};
    double rate_{};
    int years_{1};
    bool cumulative_{};
};

class Credit final : public BankProduct {
public:
    Credit() = default;
    Credit(double sum, double rate, int years, bool annuity);

    double metric() const override;
    std::unique_ptr<BankProduct> Clone() const override;
    bool isAnnuity() const;

private:
    void Print(std::ostream& output) const override;
    void Read(std::istream& input) override;

    double sum_{};
    double rate_{};
    int years_{1};
    bool annuity_{};
};

class Card final : public BankProduct {
public:
    Card() = default;
    Card(double spending, double rate);

    double metric() const override;
    std::unique_ptr<BankProduct> Clone() const override;
    bool hasCashback() const;

private:
    void Print(std::ostream& output) const override;
    void Read(std::istream& input) override;

    double spending_{};
    double rate_{};
};

}  // namespace lab04
