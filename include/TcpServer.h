#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <WinSock2.h>
#include <ws2tcpip.h>

#include <functional>
#include <thread>
#include <atomic>
#include "ThreadPool.h"
#include "Requesthandler.h"

class ThreadPool;

class TcpServer {
    public:
    using ClientHandler = std::function<void(SOCKET)>;

    TcpServer(int port, ThreadPool& pool, RequestHandler& handler);
    ~TcpServer();

    void start();
    void stop();

    private:
    void acceptLoop();

    int port_;
    SOCKET listenSocket_;
    ThreadPool& threadpool_;
    RequestHandler& clienthandler_;
    std::atomic<bool> running_;
};