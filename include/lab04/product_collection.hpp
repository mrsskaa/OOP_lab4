#pragma once

#include <cstddef>
#include <memory>

#include "lab04/bank_product.hpp"

namespace lab04 {

class ProductCollection {
public:
    ProductCollection() = default;
    ProductCollection(const ProductCollection& other);
    ProductCollection& operator=(const ProductCollection& other);
    ProductCollection(ProductCollection&& other) noexcept;
    ProductCollection& operator=(ProductCollection&& other) noexcept;
    ~ProductCollection() = default;

    void Add(const BankProduct& product);
    void Remove(std::size_t index);
    std::size_t size() const;
    const BankProduct& At(std::size_t index) const;
    double TotalMetric() const;
    const BankProduct* MaxMetricProduct() const;

private:
    void Grow();
    void Swap(ProductCollection& other) noexcept;

    std::unique_ptr<std::unique_ptr<BankProduct>[]> products_;
    std::size_t size_{};
    std::size_t capacity_{};
};

}  // namespace lab04
