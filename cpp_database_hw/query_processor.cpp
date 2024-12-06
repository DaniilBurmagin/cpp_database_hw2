#include "query_processor.h"
#include "database.h"
#include <sstream>
#include <stdexcept>
#include <iostream> 
#include <algorithm>
#include "utils.h"
#include <string>

std::string QueryProcessor::parse_and_execute(Database& db, const std::string& query) {
    std::istringstream stream(query);
    std::string command;
    stream >> command;

    if (command == "CREATE") {
        std::string temp;
        stream >> temp;
        if (temp == "TABLE") {
            std::string table_name;
            stream >> table_name;

            std::string schema_def;
            std::getline(stream, schema_def, '(');
            std::getline(stream, schema_def, ')');

            std::istringstream schema_stream(schema_def);
            std::map<std::string, std::string> schema;
            std::string column;
            while (std::getline(schema_stream, column, ',')) {
                auto colon_pos = column.find(':');
                if (colon_pos == std::string::npos)
                    throw std::runtime_error("Syntax error in schema definition.");
                std::string col_name = trim(column.substr(0, colon_pos));
                std::string col_type = trim(column.substr(colon_pos + 1));
                schema[col_name] = col_type;
            }

            db.create_table(table_name, schema);
            std::cout << "Table created: " << table_name << std::endl;
            return "Table " + table_name + " created.";
        }
    }
    else if (command == "INSERT") {
        std::string temp, table_name, values_def;
        stream >> temp; // TO
        if (temp != "TO") throw std::runtime_error("Syntax error: Expected 'TO' after INSERT.");

        stream >> table_name;

        std::getline(stream, values_def, '(');
        std::getline(stream, values_def, ')');
        values_def = trim(values_def);

        if (values_def.empty()) {
            throw std::runtime_error("No values specified for INSERT.");
        }

        std::istringstream values_stream(values_def);
        std::map<std::string, std::any> values;
        std::string value;
        while (std::getline(values_stream, value, ',')) {
            auto equals_pos = value.find('=');
            if (equals_pos != std::string::npos) {
                std::string col_name = trim(value.substr(0, equals_pos));
                std::string col_value = trim(value.substr(equals_pos + 1));

                if (col_value.empty()) {
                    throw std::runtime_error("Empty value for column: " + col_name);
                }

                if (col_value[0] == '\'') {
                    col_value = col_value.substr(1, col_value.size() - 2);
                    values[col_name] = col_value;
                }
                else if (col_value == "true" || col_value == "false") {
                    values[col_name] = (col_value == "true");
                }
                else {
                    if (!is_numeric(col_value)) {
                        throw std::runtime_error("Invalid numeric value for column: " + col_name);
                    }
                    values[col_name] = std::stoi(col_value);
                }
            }
        }

        Table* table = db.get_table(table_name);
        if (!table) throw std::runtime_error("Table not found: " + table_name);

        // ������������ ID
        if (values.find("id") != values.end()) {
            int id_value = std::any_cast<int>(values["id"]);
            if (!table->is_unique("id", id_value)) {
                throw std::runtime_error("Duplicate ID detected: " + std::to_string(id_value));
            }
        }

        table->insert(values);
        std::cout << "Row inserted into table: " << table_name << std::endl;
        return "Row inserted into " + table_name + ".";
    }
    else if (command == "DELETE") {
        std::string temp, table_name, condition;
        stream >> temp >> table_name >> temp;
        if (temp != "WHERE") throw std::runtime_error("Syntax error: Expected 'WHERE'.");

        std::getline(stream, condition);
        condition = trim(condition);

        if (condition.empty()) {
            throw std::runtime_error("Missing or empty condition in DELETE query.");
        }

        Table* table = db.get_table(table_name);
        if (!table) throw std::runtime_error("Table not found: " + table_name);

        table->remove(condition);
        std::cout << "Rows deleted from table: " << table_name << std::endl;
        return "Rows deleted from " + table_name + ".";
    }
    else if (query.find("JOIN") != std::string::npos) {
        if (query.find("JOIN") != std::string::npos) {
            size_t join_pos = query.find("JOIN");
            size_t on_pos = query.find("ON");
            std::string table1_name = trim(query.substr(14, join_pos - 14)); // FROM table1
            std::string table2_name = trim(query.substr(join_pos + 5, on_pos - join_pos - 5)); // JOIN table2
            std::string condition = trim(query.substr(on_pos + 3)); // ON table1.col1 = table2.col2

            auto delimiter_pos = condition.find('=');
            std::string table1_field = trim(condition.substr(0, delimiter_pos - 1));
            std::string table2_field = trim(condition.substr(delimiter_pos + 2));

            auto [table1_prefix, column1_name] = parse_field(table1_field);
            auto [table2_prefix, column2_name] = parse_field(table2_field);

            if (table1_prefix != table1_name || table2_prefix != table2_name) {
                throw std::runtime_error("Field does not belong to specified table in JOIN.");
            }

            Table* table1 = db.get_table(table1_name);
            Table* table2 = db.get_table(table2_name);

            if (!table1 || !table2) {
                throw std::runtime_error("One or both tables not found for JOIN.");
            }

            Table result = table1->join(*table2, column1_name, column2_name);

            std::ostringstream oss;
            for (const auto& row : result.select("true")) {
                for (const auto& [key, value] : row) {
                    if (value.type() == typeid(std::string)) {
                        oss << key << ": " << std::any_cast<std::string>(value) << ", ";
                    }
                    else if (value.type() == typeid(int)) {
                        oss << key << ": " << std::any_cast<int>(value) << ", ";
                    }
                    else if (value.type() == typeid(bool)) {
                        oss << key << ": " << (std::any_cast<bool>(value) ? "true" : "false") << ", ";
                    }
                }
                oss << "\n";
            }

            return oss.str();
        }

        return "Unknown command.";
    }
    else if (command == "UPDATE") {
        std::string table_name, temp, updates_str, condition;

        // ������ ����� �������
        stream >> table_name >> temp;
        if (temp != "SET") {
            throw std::runtime_error("Syntax error: Expected 'SET' after table name in UPDATE query.");
        }

        // ������ ����������� �������� �� ��������� ����� "WHERE"
        std::getline(stream, updates_str, 'W');
        updates_str = trim(updates_str); // ������� ������ �������

        if (updates_str.empty()) {
            throw std::runtime_error("Missing update values in UPDATE query.");
        }

        // ������ ������� ����� "WHERE"
        std::getline(stream, condition);
        condition = "W" + condition; // ���������� 'W', ����������� � ���������� ������
        condition = trim(condition);
        if (condition.substr(0, 6) == "WHERE ") {
            condition = condition.substr(6); // ������� "WHERE " �� ������ ������
        }

        if (condition.empty()) {
            throw std::runtime_error("Empty condition in UPDATE query.");
        }

        // ������ ����������� ��������
        std::map<std::string, std::any> updates;
        std::istringstream updates_stream(updates_str);
        std::string update;
        while (std::getline(updates_stream, update, ',')) {
            auto equals_pos = update.find('=');
            if (equals_pos == std::string::npos) {
                throw std::runtime_error("Syntax error in UPDATE values: " + update);
            }

            std::string col_name = trim(update.substr(0, equals_pos));
            std::string col_value = trim(update.substr(equals_pos + 1));

            if (col_name.empty()) {
                throw std::runtime_error("Empty column name in UPDATE values.");
            }

            // ���������� ��� ��������
            if (col_value == "NULL" || col_value == "null") {
                updates[col_name] = std::any(); // ������������ ��� ������ ��������
            }
            else if (col_value[0] == '\'' && col_value.back() == '\'') {
                col_value = col_value.substr(1, col_value.size() - 2); // ������� �������
                updates[col_name] = col_value; // ��������� ��������
            }
            else if (col_value == "true" || col_value == "false") {
                updates[col_name] = (col_value == "true"); // ������� ��������
            }
            else {
                if (!is_numeric(col_value)) {
                    throw std::runtime_error("Invalid numeric value in UPDATE for column: " + col_name);
                }
                updates[col_name] = std::stoi(col_value); // ������������� ��������
            }
        }

        // ��������� �������
        Table* table = db.get_table(table_name);
        if (!table) {
            throw std::runtime_error("Table not found: " + table_name);
        }

        // ���������� ����������
        table->update(condition, updates);

        std::cout << "Rows updated in table: " << table_name << "\n";
        return "Rows updated in " + table_name + ".";
        }
    else if (command == "SELECT") {
        std::string columns, temp, table_name, condition;
        stream >> columns >> temp >> table_name;

        // �������� ������� WHERE � ������ �������
        if (stream >> temp && temp == "WHERE") {
            std::getline(stream, condition);
            condition = trim(condition); // �������� ������ ��������
        }
        else {
            condition = "true"; // ���� WHERE �����������, �������� ��� ������
        }

        if (condition.empty()) {
            throw std::runtime_error("Missing or empty condition in SELECT query.");
        }

        Table* table = db.get_table(table_name);
        if (!table) throw std::runtime_error("Table not found: " + table_name);

        auto rows = table->select(condition);
        std::ostringstream result;

        // �������������� ������ �����������
        for (const auto& row : rows) {
            for (const auto& [col_name, value] : row) {
                if (value.type() == typeid(std::string)) {
                    result << col_name << ": " << std::any_cast<std::string>(value) << ", ";
                }
                else if (value.type() == typeid(int)) {
                    result << col_name << ": " << std::any_cast<int>(value) << ", ";
                }
                else if (value.type() == typeid(bool)) {
                    result << col_name << ": " << (std::any_cast<bool>(value) ? "true" : "false") << ", ";
                }
            }
            result << "\n";
        }
        return result.str();
    }



    return "Unknown command.";
}
