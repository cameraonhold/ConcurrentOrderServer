#pragma once
#include <string>
#include <windows.h>
#include <winsock2.h>
#include "OrderBook.h"
#include "Database.h"

class RequestHandler {
    public:
    RequestHandler(OrderBook& book, Database& db);
    std::string handle(const std::string& request);
    void handleClient(SOCKET clientSocket);

    private:
    OrderBook& book;
    Database& db;
};