// EventLoop.hpp
#pragma once
#include "Socket.h"
#include <sys/event.h>
#include <unordered_map>
#include <string>

class EventLoop {
public:
    explicit EventLoop(int port);
    ~EventLoop();
    void run();

private:
    void register_event(int fd, int16_t filter, uint16_t flags);
    void accept_new_connections();
    void handle_readable(int fd);
    void close_connection(int fd);

    Socket listen_socket_;
    int kq_;
    std::unordered_map<int, std::string> buffers_;
    std::unordered_map<int, int> conn_ids_;
    int connection_count_ = 0;
};