#pragma once
#include <string>

class Database; // ��������������� ����������

class QueryProcessor {
public:
    static std::string parse_and_execute(Database& db, const std::string& query);
};
