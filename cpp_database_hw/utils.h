#ifndef UTILS_H
#define UTILS_H

#include <string>

// ������� ������� � ������ � � ����� ������.
std::string trim(const std::string& str);

// ���������, �������� �� ������ ������.
bool is_numeric(const std::string& str);

// ������� parse_field: ��������� ������ table.column �� ���� (table, column)
std::pair<std::string, std::string> parse_field(const std::string& field);

#endif // UTILS_H
