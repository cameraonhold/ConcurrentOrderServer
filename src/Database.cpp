#include "Database.h"
#include <stdexcept>

Database::Database(const std::string& file) {
    if (sqlite3_open(file.c_str(), &db) != SQLITE_OK)
        throw std::runtime_error("Failed to open database");

    const char* sql =
        "CREATE TABLE IF NOT EXISTS orders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "account INTEGER"
        "amount REAL);";

    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}

void Database::saveOrder(int accountId, double amount) {
    std::lock_guard lock(mutex);

    const char* sql = "INSERT INTO orders (account, amount) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, accountId);
    sqlite3_bind_double(stmt, 2, amount);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

Database::~Database() {
    sqlite3_close(db);
}