#include "lab04/product_collection.hpp"

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <utility>

#include <gtest/gtest.h>

#include "lab04/bank_product.hpp"
#include "lab04/bank_products.hpp"

namespace lab04 {
namespace {

class TrackedProduct final : public BankProduct {
public:
    explicit TrackedProduct(int& destruction_count) : destruction_count_(destruction_count) {}
    ~TrackedProduct() override {
        ++destruction_count_;
    }

    double metric() const override {
        return 0.0;
    }

    std::unique_ptr<BankProduct> Clone() const override {
        return std::make_unique<TrackedProduct>(destruction_count_);
    }

private:
    void Print(std::ostream&) const override {}
    void Read(std::istream&) override {}

    int& destruction_count_;
};

TEST(ProductCollectionTest, EmptyCollectionHasZeroTotal) {
    const ProductCollection products;

    EXPECT_EQ(products.size(), 0u);
    EXPECT_DOUBLE_EQ(products.TotalMetric(), 0.0);
}

TEST(ProductCollectionTest_Fail, EmptyCollectionHasNoMaximum) {
    const ProductCollection products;

    EXPECT_EQ(products.MaxMetricProduct(), nullptr);
}

TEST(ProductCollectionTest, AggregatesDifferentProductTypes) {
    ProductCollection products;
    products.Add(Deposit(1000, 0.10, 2, true));
    products.Add(Credit(1000, 0.15, 3, false));
    products.Add(Card(8000, 0.02));

    EXPECT_EQ(products.size(), 3u);
    EXPECT_NEAR(products.TotalMetric(), 415.0, BankProduct::kEps);
}

TEST(ProductCollectionTest, FindsMaximumInMiddle) {
    ProductCollection products;
    products.Add(Deposit(1000, 0.10, 1, false));
    products.Add(Credit(1000, 0.20, 1, false));
    products.Add(Card(8000, 0.02));

    EXPECT_EQ(products.MaxMetricProduct(), &products.At(1));
}

TEST(ProductCollectionTest, EqualMaximumKeepsFirstProduct) {
    ProductCollection products;
    products.Add(Deposit(1000, 0.10, 1, false));
    products.Add(Card(1000, 0.10));

    EXPECT_EQ(products.MaxMetricProduct(), &products.At(0));
}

TEST(ProductCollectionTest, FindsStrictMaximumEvenWithinEqualityTolerance) {
    ProductCollection products;
    products.Add(Card(100, 1));
    products.Add(Card(100.0000005, 1));

    EXPECT_EQ(products.MaxMetricProduct(), &products.At(1));
}

TEST(ProductCollectionTest, RemovingMaximumPreservesOrderAndFindsNextMaximum) {
    ProductCollection products;
    products.Add(Card(100, 1));
    products.Add(Card(300, 1));
    products.Add(Card(200, 1));

    products.Remove(1);

    ASSERT_EQ(products.size(), 2u);
    EXPECT_DOUBLE_EQ(products.At(0).metric(), 100.0);
    EXPECT_EQ(products.MaxMetricProduct(), &products.At(1));
    EXPECT_DOUBLE_EQ(products.At(1).metric(), 200.0);
}

TEST(ProductCollectionTest, CopyOwnsIndependentProductsOfSameDynamicType) {
    ProductCollection original;
    original.Add(Card(100, 1));
    ProductCollection copy(original);
    ProductCollection assigned;
    assigned.Add(Deposit());
    assigned = original;

    EXPECT_NE(&original.At(0), &copy.At(0));
    EXPECT_NE(&original.At(0), &assigned.At(0));
    original.Remove(0);

    EXPECT_NE(dynamic_cast<const Card*>(&copy.At(0)), nullptr);
    EXPECT_DOUBLE_EQ(copy.At(0).metric(), 100.0);
    EXPECT_DOUBLE_EQ(assigned.At(0).metric(), 100.0);
}

TEST(ProductCollectionTest, MovedFromCollectionCanBeReused) {
    ProductCollection original;
    original.Add(Card(100, 1));

    ProductCollection moved(std::move(original));
    ProductCollection assigned;
    assigned.Add(Credit());
    assigned = std::move(moved);

    EXPECT_EQ(original.size(), 0u);
    EXPECT_EQ(moved.size(), 0u);
    EXPECT_DOUBLE_EQ(assigned.TotalMetric(), 100.0);
    original.Add(Deposit(10, 1, 1, false));
    EXPECT_DOUBLE_EQ(original.TotalMetric(), 10.0);
}

TEST(ProductCollectionTest, RemovesAndDestroysThroughBaseExactlyOnce) {
    int destruction_count{};
    const TrackedProduct product(destruction_count);
    {
        ProductCollection products;
        products.Add(product);
        products.Add(product);
        products.Add(product);

        products.Remove(1);

        EXPECT_EQ(destruction_count, 1);
    }
    EXPECT_EQ(destruction_count, 3);
}

TEST(ProductCollectionTest, GrowthPreservesProductsAndPointers) {
    ProductCollection products;
    products.Add(Card(1, 1));
    const BankProduct* first = &products.At(0);

    for (std::size_t i = 0; i < 1000; ++i) {
        products.Add(Card(2, 1));
    }

    EXPECT_EQ(products.size(), 1001u);
    EXPECT_EQ(&products.At(0), first);
    EXPECT_DOUBLE_EQ(products.TotalMetric(), 2001.0);
}

TEST(ProductCollectionTest_Throw, RejectsOutOfRangeIndices) {
    ProductCollection products;

    EXPECT_THROW(products.At(0), std::out_of_range);
    EXPECT_THROW(products.Remove(0), std::out_of_range);
}

}  // namespace
}  // namespace lab04
