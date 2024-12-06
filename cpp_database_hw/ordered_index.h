#pragma once
#ifndef ORDERED_INDEX_H
#define ORDERED_INDEX_H

#include <map>
#include <set>
#include <any>
#include <stdexcept>

/**
 * @brief ������� ��� ��������� ���� �������� std::any.
 */
bool compare_any(const std::any& a, const std::any& b);

/**
 * @brief ������� ��� ��������� ���� �������� std::any.
 * ���������, ������ �� ������ ��������, ��� ������.
 */
bool compare_any_order(const std::any& a, const std::any& b);


/**
 * @class OrderedUnorderedIndex
 * ���������� �������������� ������� ��� �������� ������� �� ���������� ��������.
 */
class OrderedUnorderedIndex {
public:
    OrderedUnorderedIndex() = default; ///< ����������� �� ���������.

    OrderedUnorderedIndex(OrderedUnorderedIndex&& other) noexcept;
    OrderedUnorderedIndex& operator=(OrderedUnorderedIndex&& other) noexcept;

    void add_entry(const std::any& value, size_t row_id);
    void remove_entry(const std::any& value, size_t row_id);
    std::set<size_t> find_range(const std::any& min_val, const std::any& max_val) const;
    void clear();

private:
    std::map<std::any, std::set<size_t>, decltype(&compare_any_order)> index_map{ compare_any_order };
};


#endif // ORDERED_INDEX_H
