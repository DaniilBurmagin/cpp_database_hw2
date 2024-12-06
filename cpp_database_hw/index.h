#pragma once
#include <unordered_map>
#include <vector>
#include <any>
#include <string>

class UnorderedIndex {
private:
    std::unordered_map<std::string, std::vector<size_t>> string_index_data;
    std::unordered_map<int, std::vector<size_t>> int_index_data;

public:
    UnorderedIndex() = default;
    ~UnorderedIndex() = default;

    // Копирующий конструктор
    UnorderedIndex(const UnorderedIndex& other) = default;

    // Оператор копирования
    UnorderedIndex& operator=(const UnorderedIndex& other) = default;

    // Перемещающий конструктор
    UnorderedIndex(UnorderedIndex&& other) noexcept = default;

    // Оператор перемещения
    UnorderedIndex& operator=(UnorderedIndex&& other) noexcept = default;

    void add_entry(const std::any& key, size_t row_index);
    std::vector<size_t> find(const std::any& key) const;
    void remove_entry(const std::any& key, size_t row_index);
};
