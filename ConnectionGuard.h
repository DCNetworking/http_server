#pragma once
#include "Logger.h"
#include <unistd.h>
#include <string>

class ConnectionGuard {
public:
    ConnectionGuard(int fd, int conn_id) : fd_(fd), conn_id_(conn_id) {}
    ~ConnectionGuard() {
        if (fd_ >= 0) {
            ::close(fd_);
            Logger::log("  DONE connection #" + std::to_string(conn_id_)
                        + " (fd=" + std::to_string(fd_) + " closed)");
        }
    }
    ConnectionGuard(const ConnectionGuard&) = delete;
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;

    int fd() const { return fd_; }

private:
    int fd_;
    int conn_id_;
};