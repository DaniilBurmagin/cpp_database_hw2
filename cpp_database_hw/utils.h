#ifndef UTILS_H
#define UTILS_H

#include <string>

// Удаляет пробелы в начале и в конце строки.
std::string trim(const std::string& str);

// Проверяет, является ли строка числом.
bool is_numeric(const std::string& str);

// Функция parse_field: разбивает строку table.column на пару (table, column)
std::pair<std::string, std::string> parse_field(const std::string& field);

#endif // UTILS_H
