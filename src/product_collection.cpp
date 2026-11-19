#include "lab04/product_collection.hpp"

#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

namespace lab04 {

ProductCollection::ProductCollection(const ProductCollection& other) {
    if (other.size_ != 0) {
        products_ = std::make_unique<std::unique_ptr<BankProduct>[]>(other.size_);
        capacity_ = other.size_;
        for (; size_ < other.size_; ++size_) {
            products_[size_] = other.products_[size_]->Clone();
        }
    }
}

ProductCollection& ProductCollection::operator=(const ProductCollection& other) {
    ProductCollection copy(other);
    Swap(copy);
    return *this;
}

ProductCollection::ProductCollection(ProductCollection&& other) noexcept {
    Swap(other);
}

ProductCollection& ProductCollection::operator=(ProductCollection&& other) noexcept {
    ProductCollection moved(std::move(other));
    Swap(moved);
    return *this;
}

void ProductCollection::Add(const BankProduct& product) {
    auto copy = product.Clone();
    if (size_ == capacity_) {
        Grow();
    }
    products_[size_] = std::move(copy);
    ++size_;
}

void ProductCollection::Remove(std::size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Нет продукта с таким индексом");
    }
    for (std::size_t i = index; i + 1 < size_; ++i) {
        products_[i] = std::move(products_[i + 1]);
    }
    --size_;
    products_[size_].reset();
}

std::size_t ProductCollection::size() const {
    return size_;
}

const BankProduct& ProductCollection::At(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Нет продукта с таким индексом");
    }
    return *products_[index];
}

double ProductCollection::TotalMetric() const {
    double sum{};
    for (std::size_t i = 0; i < size_; ++i) {
        sum += products_[i]->metric();
    }
    return sum;
}

const BankProduct* ProductCollection::MaxMetricProduct() const {
    if (size_ == 0) {
        return nullptr;
    }
    const BankProduct* best = products_[0].get();
    double best_metric = best->metric();
    for (std::size_t i = 1; i < size_; ++i) {
        const double current_metric = products_[i]->metric();

        if (current_metric > best_metric) {
            best = products_[i].get();
            best_metric = current_metric;
        }
    }
    return best;
}

void ProductCollection::Grow() {
    constexpr auto kMaxCapacity = std::numeric_limits<std::size_t>::max() / sizeof(std::unique_ptr<BankProduct>);
    if (capacity_ > kMaxCapacity / 2) {
        throw std::length_error("Коллекция слишком большая");
    }
    const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    auto new_products = std::make_unique<std::unique_ptr<BankProduct>[]>(new_capacity);
    for (std::size_t i = 0; i < size_; ++i) {
        new_products[i] = std::move(products_[i]);
    }
    products_ = std::move(new_products);
    capacity_ = new_capacity;
}

void ProductCollection::Swap(ProductCollection& other) noexcept {
    products_.swap(other.products_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

}  // namespace lab04
