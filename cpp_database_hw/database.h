#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "table.h"

class Database {
public:
    // ������ ������� � ��������� ������ � ������.
    void create_table(const std::string& name, const std::map<std::string, std::string>& schema);

    // �������� ��������� �� ������� �� � �����.
    Table* get_table(const std::string& name);

    // ��������� SQL-������ � ���������� ��������� � ���� ������.
    std::string execute(const std::string& query);

    // ��������� ���� ������ � �������� ����.
    void save_to_file(const std::string& filename) const;

    // ��������� ���� ������ �� ��������� �����.
    void load_from_file(const std::string& filename);

    // ������ ����������.
    void begin_transaction();

    // ����� ����������.
    void rollback_transaction();

    // ������������� ����������.
    void commit_transaction();

private:
    std::map<std::string, std::shared_ptr<Table>> tables; // ��������� ������
    std::vector<std::map<std::string, std::shared_ptr<Table>>> transaction_stack; // ���� ��� ����������
};

#endif // DATABASE_H
