#include "Socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

Socket::Socket(int port) {
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);

    if (fd_ < 0) throw std::runtime_error("Socket creation failed");
    int opt = 1;

    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (::bind(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("bind() failed");

    if (::listen(fd_, 16) < 0)
        throw std::runtime_error("listen() failed");
};
Socket::~Socket() {
    if (fd_ >= 0) ::close(fd_);
};
Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        if (fd_ >= 0) ::close(fd_);
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

int Socket::accept_connection() const {
    int client_fd = ::accept(fd_, nullptr, nullptr);
    if (client_fd < 0) throw std::runtime_error("accept() failed");
    return client_fd;
}