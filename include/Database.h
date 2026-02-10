#pragma once
#include <sqlite3.h>
#include <mutex>
#include <string>

class Database {
    public:
    Database(const std::string& file);
    ~Database();

    void saveOrder(int accountId, double amount);


    private:
    sqlite3* db;
    std::mutex mutex;
};