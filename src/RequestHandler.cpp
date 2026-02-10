#include <winsock2.h>
#include <ws2tcpip.h>
#include "RequestHandler.h"
#include <sstream>
#include <iostream>
#include <thread>

RequestHandler::RequestHandler(OrderBook &b, Database &d)
    : book(b), db(d) {}

std::string RequestHandler::handle(const std::string &request)
{
    std::istringstream iss(request);
    std::string cmd;
    int account;
    double amount;

    iss >> cmd >> account >> amount;

    if (cmd == "ORDER")
    {
        book.addOrder(account, amount);
        db.saveOrder(account, amount);
        return "OK\n";
    }

    return "ERROR\n";
}

void RequestHandler::handleClient(SOCKET clientSocket)
{
    std::cout << "Requesthandler running on thread "
              << std::this_thread::get_id()
              << std::endl;

    char buffer[1024];
    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0';

            std::cout << "Recieved request: " << buffer;

            send(clientSocket, buffer, bytesReceived, 0);
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }
        else
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}