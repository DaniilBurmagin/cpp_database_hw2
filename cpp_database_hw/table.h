#ifndef TABLE_H
#define TABLE_H

#include <map>
#include <vector>
#include <string>
#include <any>
#include <functional>
#include <memory>
#include <iostream>
#include "index.h" // Подключаем реализацию UnorderedIndex

class Table {
public:
    Table(const std::map<std::string, std::string>& schema);
    Table join(const Table& other, const std::string& on_this_field, const std::string& on_other_field) const;
    Table() = default;

    void insert(const std::map<std::string, std::any>& values);
    void remove(const std::string& condition);
    void update(const std::string& condition, const std::map<std::string, std::any>& updates);
    std::vector<std::map<std::string, std::any>> select(const std::string& condition) const;
    bool is_unique(const std::string& column_name, const std::any& value) const;

    void create_index(const std::string& column);
    void auto_index(const std::string& column);

    void save(std::ostream& os) const;
    void load(std::istream& is);
    std::shared_ptr<Table> clone() const;

    void print(std::ostream& os) const; // Добавили метод print
    size_t get_column_index(const std::string& column_name) const; // Добавили метод get_column_index

private:
    std::vector<std::string> columns;
    std::map<std::string, std::string> column_types;
    std::vector<std::vector<std::any>> rows;
    std::map<std::string, UnorderedIndex> indices; // Используем UnorderedIndex из index.h
    std::map<std::string, std::string> constraints;

    std::function<bool(const std::map<std::string, std::any>&)> parse_condition(const std::string& condition) const;
    std::function<bool(const std::map<std::string, std::any>&)> parse_simple_condition(const std::string& condition) const;
};

#endif // TABLE_H
