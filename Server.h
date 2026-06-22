// Server.hpp
#pragma once
#include "Socket.h"
#include "ThreadPool.h"
#include <atomic>
#include <string>

class Server {
public:
    Server(int port, size_t num_threads);
    void run();

private:
    void handle_client(int client_fd, int conn_id);
    std::string read_request(int fd);
    void send_all(int fd, const std::string& data);

    Socket socket_;
    ThreadPool pool_;
    std::atomic<int> connection_count_{0};
};