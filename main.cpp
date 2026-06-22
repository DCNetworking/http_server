#include "EventLoop.h"
#include <csignal>
#include <iostream>

int main() {
    std::signal(SIGPIPE, SIG_IGN);
    try {
        EventLoop loop(8080);
        loop.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}