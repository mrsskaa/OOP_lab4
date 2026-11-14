#include "lab04/bank_products.hpp"

#include <cmath>
#include <ios>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace lab04 {
namespace {

constexpr double kMonthsPerYear{12.0};
constexpr double kPercentPerUnit{100.0};

bool IsNonNegative(double value) {
    return std::isfinite(value) && value >= 0.0;
}

bool IsValidTerm(double sum, double rate, int years) {
    return IsNonNegative(sum) && IsNonNegative(rate) && years > 0;
}

bool ReadLabel(std::istream& input, std::string_view label) {
    input >> std::ws;
    for (const char expected : label) {
        if (input.get() != expected) {
            input.setstate(std::ios::failbit);
            return false;
        }
    }
    return true;
}

template <typename T>
bool ReadField(std::istream& input, bool labelled, std::string_view label, T& value) {
    return (!labelled || ReadLabel(input, label)) && (input >> value);
}

std::ostringstream RecordStream(const std::ostream& output) {
    std::ostringstream record;
    record.imbue(output.getloc());
    record.precision(std::numeric_limits<double>::max_digits10);
    return record;
}

}  // namespace

Deposit::Deposit(double sum, double rate, int years, bool cumulative)
    : sum_(sum), rate_(rate), years_(years), cumulative_(cumulative) {
    if (!IsValidTerm(sum, rate, years) || !std::isfinite(metric())) {
        throw std::invalid_argument("Некорректные параметры вклада");
    }
}

double Deposit::metric() const {
    if (!cumulative_) {
        return sum_ * rate_;
    }
    if (sum_ == 0.0) {
        return 0.0;
    }
    return sum_ * (std::expm1(years_ * std::log1p(rate_)) / years_);
}

std::unique_ptr<BankProduct> Deposit::Clone() const {
    return std::make_unique<Deposit>(*this);
}

bool Deposit::isCumulative() const {
    return cumulative_;
}

void Deposit::Print(std::ostream& output) const {
    auto record = RecordStream(output);
    record << "Deposit: sum=" << sum_ << " rate=" << rate_ << " years=" << years_ << " cumulative=" << cumulative_;
    output << record.str();
}

void Deposit::Read(std::istream& input) {
    double sum{};
    double rate{};
    int years{};
    int cumulative{};
    const bool labelled = (input >> std::ws).peek() == 'D';
    if ((labelled && !ReadLabel(input, "Deposit:")) || !ReadField(input, labelled, "sum=", sum) ||
        !ReadField(input, labelled, "rate=", rate) || !ReadField(input, labelled, "years=", years) ||
        !ReadField(input, labelled, "cumulative=", cumulative) || (cumulative != 0 && cumulative != 1))
    {
        input.setstate(std::ios::failbit);
        return;
    }

    try {
        *this = Deposit(sum, rate, years, cumulative == 1);
    } catch (const std::invalid_argument&) {
        input.setstate(std::ios::failbit);
    }
}

Credit::Credit(double sum, double rate, int years, bool annuity)
    : sum_(sum), rate_(rate), years_(years), annuity_(annuity) {
    if (!IsValidTerm(sum, rate, years) || !std::isfinite(metric())) {
        throw std::invalid_argument("Некорректные параметры кредита");
    }
}

double Credit::metric() const {
    if (!annuity_) {
        return sum_ * rate_;
    }
    const double monthly_rate = rate_ / (kMonthsPerYear * kPercentPerUnit);
    if (monthly_rate == 0.0) {
        return sum_ / years_;
    }
    const double months = years_ * kMonthsPerYear;
    const double payment_factor = -std::expm1(-months * std::log1p(monthly_rate));
    return sum_ * (kMonthsPerYear * (monthly_rate / payment_factor));
}

std::unique_ptr<BankProduct> Credit::Clone() const {
    return std::make_unique<Credit>(*this);
}

bool Credit::isAnnuity() const {
    return annuity_;
}

void Credit::Print(std::ostream& output) const {
    auto record = RecordStream(output);
    record << "Credit: sum=" << sum_ << " rate=" << rate_ << " years=" << years_ << " annuity=" << annuity_;
    output << record.str();
}

void Credit::Read(std::istream& input) {
    double sum{};
    double rate{};
    int years{};
    int annuity{};
    const bool labelled = (input >> std::ws).peek() == 'C';
    if ((labelled && !ReadLabel(input, "Credit:")) || !ReadField(input, labelled, "sum=", sum) ||
        !ReadField(input, labelled, "rate=", rate) || !ReadField(input, labelled, "years=", years) ||
        !ReadField(input, labelled, "annuity=", annuity) || (annuity != 0 && annuity != 1))
    {
        input.setstate(std::ios::failbit);
        return;
    }

    try {
        *this = Credit(sum, rate, years, annuity == 1);
    } catch (const std::invalid_argument&) {
        input.setstate(std::ios::failbit);
    }
}

Card::Card(double spending, double rate) : spending_(spending), rate_(rate) {
    if (!IsNonNegative(spending) || !IsNonNegative(rate) || !std::isfinite(metric())) {
        throw std::invalid_argument("Некорректные параметры карты");
    }
}

double Card::metric() const {
    return spending_ * rate_;
}

std::unique_ptr<BankProduct> Card::Clone() const {
    return std::make_unique<Card>(*this);
}

bool Card::hasCashback() const {
    return rate_ > 0.0;
}

void Card::Print(std::ostream& output) const {
    auto record = RecordStream(output);
    record << "Card: spending=" << spending_ << " rate=" << rate_;
    output << record.str();
}

void Card::Read(std::istream& input) {
    double spending{};
    double rate{};
    const bool labelled = (input >> std::ws).peek() == 'C';
    if ((labelled && !ReadLabel(input, "Card:")) || !ReadField(input, labelled, "spending=", spending) ||
        !ReadField(input, labelled, "rate=", rate))
    {
        input.setstate(std::ios::failbit);
        return;
    }

    try {
        *this = Card(spending, rate);
    } catch (const std::invalid_argument&) {
        input.setstate(std::ios::failbit);
    }
}

}  // namespace lab04
