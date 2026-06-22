// EventLoop.cpp
#include "EventLoop.h"
#include "Logger.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "utils.h"
#include <unistd.h>
#include <sys/socket.h>
#include <cerrno>
#include <vector>

EventLoop::EventLoop(int port) : listen_socket_(port) {
    kq_ = kqueue();
    if (kq_ < 0) throw std::runtime_error("kqueue() failed");
    set_nonblocking(listen_socket_.fd());
    register_event(listen_socket_.fd(), EVFILT_READ, EV_ADD);

    Logger::log("EventLoop ready on port " + std::to_string(port));
}

EventLoop::~EventLoop() {
    if (kq_ >= 0) ::close(kq_);
}

void EventLoop::register_event(int fd, int16_t filter, uint16_t flags) {
    struct kevent ev;
    EV_SET(&ev, fd, filter, flags, 0, 0, nullptr);
    kevent(kq_, &ev, 1, nullptr, 0, nullptr);
}

void EventLoop::run() {
    std::vector<struct kevent> events(64);
    while (true) {
        int n = kevent(kq_, nullptr, 0, events.data(), events.size(), nullptr);

        if (n < 0) {
            if (errno == EINTR) continue;
            Logger::log("kevent error");
            break;
        }

        Logger::log("kevent woke up with " + std::to_string(n) + " ready event(s)");

        for (int i = 0; i < n; ++i) {
            int fd = static_cast<int>(events[i].ident);

            if (fd == listen_socket_.fd()) {
                accept_new_connections();
            } else {
                handle_readable(fd);
            }
        }
    }
}

void EventLoop::accept_new_connections() {
    while (true) {
        int client_fd = ::accept(listen_socket_.fd(), nullptr, nullptr);

        if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            if (errno == EINTR) continue;
            Logger::log("accept error");
            break;
        }

        set_nonblocking(client_fd);
        register_event(client_fd, EVFILT_READ, EV_ADD);

        int conn_id = ++connection_count_;
        conn_ids_[client_fd] = conn_id;
        buffers_[client_fd] = "";

        Logger::log("ACCEPTED connection #" + std::to_string(conn_id)
                    + " (fd=" + std::to_string(client_fd) + ")");
    }
}

void EventLoop::handle_readable(int fd) {
    char buf[4096];
    int conn_id = conn_ids_[fd];

    while (true) {
        ssize_t r = ::recv(fd, buf, sizeof(buf), 0);

        if (r > 0) {
            buffers_[fd].append(buf, r);
        }
        else if (r == 0) {
            Logger::log("  connection #" + std::to_string(conn_id) + " closed by peer");
            close_connection(fd);
            return;
        }
        else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            if (errno == EINTR) continue;
            Logger::log("  recv error on #" + std::to_string(conn_id));
            close_connection(fd);
            return;
        }
    }

    if (buffers_[fd].find("\r\n\r\n") != std::string::npos) {
        HttpRequest req = HttpRequest::parse(buffers_[fd]);
        Logger::log("  connection #" + std::to_string(conn_id)
                    + " -> " + req.method + " " + req.path);

        HttpResponse res;
        res.set_body("<h1>Connection #" + std::to_string(conn_id)
                     + " via kqueue</h1>");
        std::string raw = res.to_string();
        ::send(fd, raw.data(), raw.size(), 0);
        close_connection(fd);
    }
}

void EventLoop::close_connection(int fd) {
    int conn_id = conn_ids_.count(fd) ? conn_ids_[fd] : -1;

    register_event(fd, EVFILT_READ, EV_DELETE);
    ::close(fd);

    buffers_.erase(fd);
    conn_ids_.erase(fd);

    Logger::log("  DONE connection #" + std::to_string(conn_id) + " (fd closed)");
}