#include "database.h"
#include <iostream>
#include <string>

void test_create_insert();
void test_select();
void test_update();
void test_delete();
void test_index();
void test_join();
void test_transactions();

int main() {
    while (true) {
        std::cout << "\nChoose a test to run:\n";
        std::cout << "1. Test CREATE + INSERT\n";
        std::cout << "2. Test SELECT\n";
        std::cout << "3. Test UPDATE\n";
        std::cout << "4. Test DELETE\n";
        std::cout << "5. Test INDEX\n";
        std::cout << "6. Test JOIN\n";
        std::cout << "7. Test TRANSACTIONS\n";
        std::cout << "8. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            test_create_insert();
            break;
        case 2:
            test_select();
            break;
        case 3:
            test_update();
            break;
        case 4:
            test_delete();
            break;
        case 5:
            test_index();
            break;
        case 6:
            test_join();
            break;
        case 7:
            test_transactions();
            break;
        case 8:
            std::cout << "Exiting...\n";
            return 0;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}

// 1. Test CREATE + INSERT
void test_create_insert() {
    try {
        Database db;

        std::cout << "Running CREATE + INSERT test...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("INSERT TO users (id=2,name='Bob',is_admin=true)");
        db.execute("INSERT TO users (id=3,name='Charlie',is_admin=false)");
        std::cout << "Table 'users' created and rows inserted successfully.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error in CREATE + INSERT test: " << e.what() << std::endl;
    }
}

// 2. Test SELECT
void test_select() {
    try {
        Database db;

        std::cout << "Running SELECT test...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("INSERT TO users (id=2,name='Bob',is_admin=true)");
        std::cout << db.execute("SELECT * FROM users WHERE true") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in SELECT test: " << e.what() << std::endl;
    }
}
// 3. Test UPDATE
void test_update() {
    try {
        Database db;

        std::cout << "Running UPDATE test...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("UPDATE users SET name='UpdatedAlice', is_admin=true WHERE id=1");
        std::cout << db.execute("SELECT * FROM users WHERE true") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in UPDATE test: " << e.what() << std::endl;
    }
}
// 4. Test DELETE
void test_delete() {
    try {
        Database db;

        std::cout << "Running DELETE test...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("DELETE FROM users WHERE id=1");
        std::cout << db.execute("SELECT * FROM users WHERE true") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in DELETE test: " << e.what() << std::endl;
    }
}

// 5. Test INDEX
void test_index() {
    try {
        Database db;

        std::cout << "Running INDEX test...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("CREATE INDEX ON users (id)");
        std::cout << db.execute("SELECT * FROM users WHERE id=1") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in INDEX test: " << e.what() << std::endl;
    }
}
// 6. Test JOIN
void test_join() {
    try {
        Database db;

        std::cout << "Running JOIN test...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        db.execute("CREATE TABLE roles (id:int32,role_name:string)");
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("INSERT TO roles (id=1,role_name='Admin')");
        std::cout << db.execute("SELECT * FROM users JOIN roles ON users.id = roles.id") << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error in JOIN test: " << e.what() << std::endl;
    }
}

// 6. Test Транзакции
void test_transactions() {
    try {
        Database db;

        std::cout << "Running TRANSACTIONS test...\n";

        // === Создание таблицы ===
        std::cout << "Creating table 'users'...\n";
        db.execute("CREATE TABLE users (id:int32,name:string,is_admin:bool)");
        std::cout << "Table 'users' created successfully.\n";

        // === Вставка данных ===
        db.execute("INSERT TO users (id=1,name='Alice',is_admin=false)");
        db.execute("INSERT TO users (id=2,name='Bob',is_admin=true)");
        db.execute("INSERT TO users (id=3,name='Charlie',is_admin=false)");
        db.execute("INSERT TO users (id=4,name='David',is_admin=true)");
        db.execute("INSERT TO users (id=5,name='Eve',is_admin=false)");
        std::cout << "Rows inserted into 'users'.\n";

        // === Создание индексов ===
        db.execute("CREATE INDEX ON users (id)");
        db.execute("CREATE INDEX ON users (is_admin)");
        std::cout << "Indexes created on 'id' and 'is_admin'.\n";

        // === Проверка индекса ===
        std::cout << "Range query with index:\n";
        std::cout << db.execute("SELECT * FROM users WHERE id >= 2 AND id <= 4") << std::endl;

        // === Тест с большой базой данных ===
        for (int i = 6; i <= 10000; ++i) {
            db.execute("INSERT TO users (id=" + std::to_string(i) + ",name='User" + std::to_string(i) + "',is_admin=false)");
        }
        std::cout << "Inserted large dataset.\n";

        // === Тест транзакций ===
        db.begin_transaction();
        for (int i = 10001; i <= 11000; ++i) {
            db.execute("INSERT TO users (id=" + std::to_string(i) + ",name='TxUser" + std::to_string(i) + "',is_admin=true)");
        }
        db.rollback_transaction();
        std::cout << "Rows after rollback:\n" << db.execute("SELECT * FROM users WHERE id >= 10001") << std::endl;

        db.begin_transaction();
        for (int i = 11001; i <= 12000; ++i) {
            db.execute("INSERT TO users (id=" + std::to_string(i) + ",name='TxUser" + std::to_string(i) + "',is_admin=false)");
        }
        db.commit_transaction();
        std::cout << "Rows after commit:\n" << db.execute("SELECT * FROM users WHERE id >= 11001") << std::endl;

        // === Сохранение данных ===
        db.save_to_file("db_large.bin");
        std::cout << "Data saved to 'db_large.bin'.\n";

    }
    catch (const std::exception& e) {
        std::cerr << "Error in TRANSACTIONS test: " << e.what() << std::endl;
    }
}