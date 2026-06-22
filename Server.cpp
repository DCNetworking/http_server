// Server.cpp
#include "Server.h"
#include "Logger.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <unistd.h>
#include <thread>
#include <chrono>
#include <sys/socket.h>

#include "ConnectionGuard.h"

Server::Server(int port, size_t num_threads)
    : socket_(port), pool_(num_threads) {
    Logger::log("Server listening on port " + std::to_string(port));
}

void Server::run() {
    while (true) {
        int client_fd = socket_.accept_connection();
        int conn_id = connection_count_.fetch_add(1) + 1;
        Logger::log("ACCEPTED connection #" + std::to_string(conn_id)
                    + " (fd=" + std::to_string(client_fd) + ")");

        pool_.enqueue([this, client_fd, conn_id]() {
            handle_client(client_fd, conn_id);
        });
    }
}

void Server::handle_client(int client_fd, int conn_id) {
    ConnectionGuard conn(client_fd, conn_id); // od tej chwili close+log są pewne
    Logger::log("  START handling connection #" + std::to_string(conn_id));

    std::string raw_request = read_request(conn.fd());
    if (raw_request.empty()) {
        Logger::log("  connection #" + std::to_string(conn_id) + " sent nothing");
        return;
    }
    HttpRequest req = HttpRequest::parse(raw_request);
    Logger::log("  connection #" + std::to_string(conn_id)
                + " -> " + req.method + " " + req.path);

    HttpResponse res;
    res.set_body("<h1>Connection #" + std::to_string(conn_id) + "</h1>");

    send_all(conn.fd(), res.to_string());
}
std::string Server::read_request(int fd) {
    std::string data;
    char buffer[4096];

    while (true) {
        ssize_t n = ::recv(fd, buffer, sizeof(buffer), 0);

        if (n > 0) {
            data.append(buffer, n);

            // Szukamy końca nagłówków. Dla GET (bez body) to wystarcza.
            if (data.find("\r\n\r\n") != std::string::npos) {
                break;
            }
            // TODO później: dla POST odczytać Content-Length i doczytać body
        }
        else if (n == 0) {
            // Klient zamknął połączenie po swojej stronie (EOF)
            break;
        }
        else { // n < 0
            // Błąd. EINTR = przerwane sygnałem, można spróbować ponownie.
            if (errno == EINTR) continue;
            Logger::log("  recv error");
            break;
        }
    }
    return data;
}

void Server::send_all(int fd, const std::string& data) {
    size_t total_sent = 0;

    while (total_sent < data.size()) {
        ssize_t n = ::send(fd,
                           data.data() + total_sent,      // skąd zaczynamy
                           data.size() - total_sent,      // ile jeszcze zostało
                           0);

        if (n > 0) {
            total_sent += static_cast<size_t>(n);
        }
        else if (n < 0) {
            if (errno == EINTR) continue; // przerwane sygnałem — ponów
            Logger::log("  send error");
            return; // realny błąd — klient pewnie zniknął
        }
    }
}