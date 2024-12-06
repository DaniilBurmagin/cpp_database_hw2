#include "ordered_index.h"
#include <iostream>
#include <type_traits>

/**
 * @brief Конструктор перемещения.
 */
OrderedUnorderedIndex::OrderedUnorderedIndex(OrderedUnorderedIndex&& other) noexcept
    : index_map(std::move(other.index_map)) {}

/**
 * @brief Оператор перемещения.
 */
OrderedUnorderedIndex& OrderedUnorderedIndex::operator=(OrderedUnorderedIndex&& other) noexcept {
    if (this != &other) {
        index_map = std::move(other.index_map);
    }
    return *this;
}

/**
 * @brief Добавить запись в индекс.
 */
void OrderedUnorderedIndex::add_entry(const std::any& value, size_t row_id) {
    try {
        index_map[value].insert(row_id);
        std::cout << "Added entry to index: value=";
        if (value.type() == typeid(int)) {
            std::cout << std::any_cast<int>(value);
        }
        else if (value.type() == typeid(std::string)) {
            std::cout << std::any_cast<std::string>(value);
        }
        else if (value.type() == typeid(bool)) {
            std::cout << std::boolalpha << std::any_cast<bool>(value);
        }
        else {
            std::cout << "[unsupported type]";
        }
        std::cout << ", row_id=" << row_id << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to add entry to index: " + std::string(e.what()));
    }
}

/**
 * @brief Удалить запись из индекса.
 */
void OrderedUnorderedIndex::remove_entry(const std::any& value, size_t row_id) {
    auto it = index_map.find(value);
    if (it != index_map.end()) {
        it->second.erase(row_id);
        if (it->second.empty()) {
            index_map.erase(it);
        }
    }
}

/**
 * @brief Функция для сравнения двух значений std::any.
 */
bool compare_any(const std::any& a, const std::any& b) {
    if (a.type() != b.type()) {
        throw std::runtime_error("Type mismatch during comparison.");
    }

    if (a.type() == typeid(int)) {
        return std::any_cast<int>(a) == std::any_cast<int>(b);
    }
    if (a.type() == typeid(std::string)) {
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    }
    if (a.type() == typeid(bool)) {
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    }

    throw std::runtime_error("Unsupported type in comparison.");
}



bool compare_any_order(const std::any& a, const std::any& b) {
    if (a.type() != b.type()) {
        throw std::runtime_error("Type mismatch during comparison.");
    }

    if (a.type() == typeid(int)) {
        return std::any_cast<int>(a) < std::any_cast<int>(b);
    }
    if (a.type() == typeid(std::string)) {
        return std::any_cast<std::string>(a) < std::any_cast<std::string>(b);
    }
    if (a.type() == typeid(bool)) {
        return std::any_cast<bool>(a) < std::any_cast<bool>(b);
    }

    throw std::runtime_error("Unsupported type in comparison.");
}



/**
 * @brief Найти строки в заданном диапазоне значений.
 */
std::set<size_t> OrderedUnorderedIndex::find_range(const std::any& min_val, const std::any& max_val) const {
    std::set<size_t> result;

    auto it = index_map.lower_bound(min_val);
    while (it != index_map.end() && compare_any_order(it->first, max_val)) {
        result.insert(it->second.begin(), it->second.end());
        ++it; // Убедитесь, что итератор продвигается
    }

    return result;
}

/**
 * @brief Очистить индекс.
 */
void OrderedUnorderedIndex::clear() {
    index_map.clear();
}
