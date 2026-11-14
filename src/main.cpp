#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

#include "lab04/bank_product.hpp"
#include "lab04/bank_products.hpp"
#include "lab04/product_collection.hpp"

namespace {

void ReadAndAdd(lab04::ProductCollection& products, lab04::BankProduct& product, const char* prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return;
    }
    std::istringstream input(line);
    if (!(input >> product) || (!input.eof() && !(input >> std::ws).eof())) {
        std::cout << "Ошибка ввода.\n";
        return;
    }
    products.Add(product);
    std::cout << "Продукт добавлен.\n";
}

void PrintProduct(const lab04::BankProduct& product) {
    std::cout << product << "\nГодовой эффект: " << product.metric() << '\n';
    if (const auto* deposit = dynamic_cast<const lab04::Deposit*>(&product)) {
        std::cout << "Капитализация: " << (deposit->isCumulative() ? "да" : "нет") << '\n';
    } else if (const auto* credit = dynamic_cast<const lab04::Credit*>(&product)) {
        std::cout << "Аннуитет: " << (credit->isAnnuity() ? "да" : "нет") << '\n';
    } else if (const auto* card = dynamic_cast<const lab04::Card*>(&product)) {
        std::cout << "Кешбэк: " << (card->hasCashback() ? "да" : "нет") << '\n';
    }
}

void PrintProducts(const lab04::ProductCollection& products) {
    if (products.size() == 0) {
        std::cout << "Коллекция пуста.\n";
        return;
    }
    for (std::size_t i = 0; i < products.size(); ++i) {
        std::cout << i + 1 << ". ";
        PrintProduct(products.At(i));
    }
}

void PrintMaximum(const lab04::ProductCollection& products) {
    if (const auto* product = products.MaxMetricProduct()) {
        PrintProduct(*product);
    } else {
        std::cout << "Коллекция пуста.\n";
    }
}

void RemoveProduct(lab04::ProductCollection& products) {
    std::cout << "Номер продукта: ";
    std::string line;
    if (!std::getline(std::cin, line)) {
        return;
    }
    std::istringstream input(line);
    long long number{};
    if (!(input >> number) || (!input.eof() && !(input >> std::ws).eof()) || number <= 0 ||
        static_cast<unsigned long long>(number) > products.size())
    {
        std::cout << "Нет продукта с таким номером.\n";
        return;
    }
    products.Remove(static_cast<std::size_t>(number - 1));
    std::cout << "Продукт удалён.\n";
}

}  // namespace

int main() {
    lab04::ProductCollection products;
    while (true) {
        std::cout
            << "\n1. Добавить вклад\n2. Добавить кредит\n3. Добавить карту\n"
               "4. Показать продукты\n5. Суммарная метрика\n6. Максимальная метрика\n"
               "7. Удалить продукт\n0. Выход\nКоманда: ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            return 0;
        }
        std::istringstream input(line);
        int command{};
        if (!(input >> command) || (!input.eof() && !(input >> std::ws).eof())) {
            std::cout << "Ошибка ввода.\n";
            continue;
        }
        switch (command) {
            case 0:
                return 0;
            case 1: {
                lab04::Deposit deposit;
                ReadAndAdd(products, deposit, "Сумма, доля ставки, годы, капитализация (0/1): ");
                break;
            }
            case 2: {
                lab04::Credit credit;
                ReadAndAdd(products, credit, "Сумма, ставка (для аннуитета в %), годы, аннуитет (0/1): ");
                break;
            }
            case 3: {
                lab04::Card card;
                ReadAndAdd(products, card, "Расходы за год, доля кешбэка: ");
                break;
            }
            case 4:
                PrintProducts(products);
                break;
            case 5:
                std::cout << "Суммарная метрика: " << products.TotalMetric() << '\n';
                break;
            case 6:
                PrintMaximum(products);
                break;
            case 7:
                RemoveProduct(products);
                break;
            default:
                std::cout << "Нет такой команды.\n";
                break;
        }
    }
}
