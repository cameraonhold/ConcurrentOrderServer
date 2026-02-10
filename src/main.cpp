#include "ThreadPool.h"
#include "TcpServer.h"
#include "OrderBook.h"
#include "Database.h"
#include "RequestHandler.h"
#include <iostream>
#include <cstdio>

int main() {
    ThreadPool pool(std::thread::hardware_concurrency());
    OrderBook book;
    Database db("orders.db");
    RequestHandler handler(book,db);

    TcpServer server(8000, pool, handler);
    server.start();

    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    server.stop();
    return 0;
}