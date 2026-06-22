#pragma once
#include <string>

class Socket {

    int fd_;

public:
    // Create Socket
    explicit Socket(int port);
    ~Socket();
    // Copy Blocking - two objects cannot store same fd
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    // Allow move
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    int accept_connection() const; // wait for client
    int fd() const {return fd_;} // return fd
};