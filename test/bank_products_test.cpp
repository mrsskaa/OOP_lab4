#include "lab04/bank_products.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <gtest/gtest.h>

#include "lab04/bank_product.hpp"

namespace lab04 {
namespace {

static_assert(std::is_abstract_v<BankProduct>);
static_assert(std::has_virtual_destructor_v<BankProduct>);

TEST(BankProductsTest, SimpleDepositAnnualEffect) {
    const Deposit deposit(1000, 0.10, 2, false);

    EXPECT_NEAR(deposit.metric(), 100.0, BankProduct::kEps);
}

TEST(BankProductsTest, CompoundDepositAnnualEffect) {
    const Deposit deposit(1000, 0.10, 2, true);

    EXPECT_NEAR(deposit.metric(), 105.0, BankProduct::kEps);
}

TEST(BankProductsTest, SimpleCreditAnnualEffect) {
    const Credit credit(1000, 0.15, 3, false);

    EXPECT_NEAR(credit.metric(), 150.0, BankProduct::kEps);
}

TEST(BankProductsTest, AnnuityCreditAnnualPayment) {
    const Credit credit(1000, 12.0, 1, true);

    EXPECT_NEAR(credit.metric(), 1066.1854641401, BankProduct::kEps);
}

TEST(BankProductsTest, InterestFreeAnnuityRepaysPrincipal) {
    const Credit credit(2400, 0.0, 2, true);

    EXPECT_DOUBLE_EQ(credit.metric(), 1200.0);
}

TEST(BankProductsTest, CardAnnualCashback) {
    const Card card(8000, 0.02);

    EXPECT_DOUBLE_EQ(card.metric(), 160.0);
}

TEST(BankProductsTest, DepositDowncastExposesCompounding) {
    const Deposit deposit(1000, 0.10, 2, true);
    const BankProduct* product = &deposit;

    const auto* actual = dynamic_cast<const Deposit*>(product);

    ASSERT_NE(actual, nullptr);
    EXPECT_TRUE(actual->isCumulative());
}

TEST(BankProductsTest, CreditDowncastExposesAnnuity) {
    const Credit credit(1000, 12, 1, true);
    const BankProduct* product = &credit;

    const auto* actual = dynamic_cast<const Credit*>(product);

    ASSERT_NE(actual, nullptr);
    EXPECT_TRUE(actual->isAnnuity());
}

TEST(BankProductsTest, CardDowncastExposesCashback) {
    const Card card(1500, 0.02);
    const BankProduct* product = &card;

    const auto* actual = dynamic_cast<const Card*>(product);

    ASSERT_NE(actual, nullptr);
    EXPECT_TRUE(actual->hasCashback());
}

TEST(BankProductsTest_Fail, DowncastToWrongTypeReturnsNull) {
    const Card card;
    const BankProduct* product = &card;

    EXPECT_EQ(dynamic_cast<const Credit*>(product), nullptr);
}

TEST(BankProductsTest, EqualityComparesMetricsAcrossTypes) {
    const Deposit deposit(1000, 0.10, 1, false);
    const Card nearly_equal(100.0000005, 1);
    const Credit different(2000, 0.10, 1, false);

    EXPECT_TRUE(deposit == nearly_equal);
    EXPECT_FALSE(deposit == different);
}

TEST(BankProductsTest, DoubleConversionReturnsVirtualMetric) {
    const Deposit deposit(1000, 0.10, 2, true);
    const BankProduct& product = deposit;

    EXPECT_NEAR(static_cast<double>(product), 105.0, BankProduct::kEps);
}

TEST(BankProductsTest, ZeroAmountsGiveZeroEffect) {
    EXPECT_DOUBLE_EQ(Deposit(0, 0.10, 1, true).metric(), 0.0);
    EXPECT_DOUBLE_EQ(Credit(0, 0.10, 1, false).metric(), 0.0);
    EXPECT_DOUBLE_EQ(Card(0, 0.02).metric(), 0.0);
}

TEST(BankProductsTest, StreamReadsValuesInVariantOrder) {
    Deposit deposit;
    Credit credit;
    Card card;
    std::istringstream input("1000 0.1 2 1\n2400 0 2 1\n8000 0.02");

    ASSERT_TRUE(input >> deposit >> credit >> card);

    EXPECT_NEAR(deposit.metric(), 105.0, BankProduct::kEps);
    EXPECT_DOUBLE_EQ(credit.metric(), 1200.0);
    EXPECT_DOUBLE_EQ(card.metric(), 160.0);
}

TEST(BankProductsTest, PrintedRecordsCanBeReadBack) {
    const Deposit deposit(1200, 0.125, 3, true);
    const Credit credit(2048, 12.5, 2, true);
    const Card card(7500, 0.015625);
    std::stringstream stream;

    stream << deposit << '\n' << credit << '\n' << card;
    const std::string text = stream.str();
    EXPECT_NE(text.find("Deposit: sum=1200 rate=0.125 years=3 cumulative=1"), std::string::npos);
    EXPECT_NE(text.find("Credit: sum=2048 rate=12.5 years=2 annuity=1"), std::string::npos);
    EXPECT_NE(text.find("Card: spending=7500 rate=0.015625"), std::string::npos);

    Deposit read_deposit;
    Credit read_credit;
    Card read_card;
    ASSERT_TRUE(stream >> read_deposit >> read_credit >> read_card);
    EXPECT_DOUBLE_EQ(read_deposit.metric(), deposit.metric());
    EXPECT_DOUBLE_EQ(read_credit.metric(), credit.metric());
    EXPECT_DOUBLE_EQ(read_card.metric(), card.metric());
}

TEST(BankProductsTest_Throw, ConstructorsRejectInvalidValues) {
    EXPECT_THROW(Deposit(-1, 0.10, 2, true), std::invalid_argument);
    EXPECT_THROW(Credit(1000, 0, 0, true), std::invalid_argument);
    EXPECT_THROW(Card(100, -0.01), std::invalid_argument);
}

TEST(BankProductsTest_Fail, InvalidFlagLeavesProductUnchanged) {
    Deposit deposit(1000, 0.10, 1, false);
    std::istringstream input("2000 0.05 2 3");

    input >> deposit;

    EXPECT_TRUE(input.fail());
    EXPECT_DOUBLE_EQ(deposit.metric(), 100.0);
}

TEST(BankProductsTest_Fail, NegativeYearsLeaveProductUnchanged) {
    Credit credit(1000, 0.10, 1, false);
    std::istringstream input("2000 0.08 -1 0");

    input >> credit;

    EXPECT_TRUE(input.fail());
    EXPECT_DOUBLE_EQ(credit.metric(), 100.0);
}

}  // namespace
}  // namespace lab04
