#include "Tcpserver.h"
#include "ThreadPool.h"
#include <iostream>
#include <stdexcept>
#pragma comment(lib, "ws2_32.lib")

TcpServer::TcpServer(int port, ThreadPool& pool, RequestHandler& handler)
    : port_(port), listenSocket_(INVALID_SOCKET), threadpool_(pool), clienthandler_(handler), running_(false) {}

TcpServer::~TcpServer()
{
    stop();
}

void TcpServer::start()
{
    if (running_)
        return;

    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0)
    {
        throw std::runtime_error("WSAStartup failed");
    }

    SOCKET serverFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    addrinfo *result = nullptr;
    std::string portStr = std::to_string(port_);
    int addrResult = getaddrinfo(nullptr, portStr.c_str(), &hints, &result);
    if (addrResult != 0)
    {
        WSACleanup();
        throw std::runtime_error("getaddrinfo failed");
    }

    listenSocket_ = socket(
        result->ai_family,
        result->ai_socktype,
        result->ai_protocol);

    if (listenSocket_ == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    if (bind(listenSocket_, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR)
    {
        freeaddrinfo(result);
        closesocket(listenSocket_);
        WSACleanup();
        throw std::runtime_error("Bind failed");
    }

    freeaddrinfo(result);

    if (listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(listenSocket_);
        WSACleanup();
        throw std::runtime_error("Listen failed");
    }

    running_ = true;

    std::cout << "Server started on port " << port_ << std::endl;

    std::thread(&TcpServer::acceptLoop, this).detach();
}

void TcpServer::stop()
{
    if (!running_)
        return;

    running_ = false;

    if (listenSocket_ != INVALID_SOCKET)
    {
        closesocket(listenSocket_);
        listenSocket_ = INVALID_SOCKET;
    }
    WSACleanup();

    std::cout << "Server stopped." << std::endl;
}

void TcpServer::acceptLoop()
{
    while (running_)
    {
        SOCKET clientSocket = accept(listenSocket_, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET){
            if (running_)
            {
                std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            }
            continue;
        }

        threadpool_.enqueue([this, clientSocket](){
            std::cout << "Handling client on thread " << std::this_thread::get_id() << std::endl;
            
            clienthandler_.handleClient(clientSocket);
            closesocket(clientSocket); 
        });
    }
}
