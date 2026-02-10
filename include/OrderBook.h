#pragma once
#include <unordered_map>
#include <shared_mutex>

class OrderBook {
    public:
    void addOrder(int accountId, double amount);
    double getBalance(int accountId) const;

    private:
    mutable std::shared_mutex mutex;
    std::unordered_map<int, double> balances;
};