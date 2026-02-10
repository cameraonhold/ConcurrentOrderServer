#include "OrderBook.h"

void OrderBook::addOrder(int accountId, double amount){
    std::unique_lock lock(mutex);
    balances[accountId] += amount;
}

double OrderBook::getBalance(int accountId) const{
    std::shared_lock lock(mutex);
    auto it = balances.find(accountId);
    return it == balances.end() ? 0.0 : it->second;
}