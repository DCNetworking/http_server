#pragma once
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <sstream>

class Logger {
public:
    static void log(const std::string& msg) {
        std::lock_guard<std::mutex> lock(mutex_());
        std::ostringstream tid;
        tid << std::this_thread::get_id();
        std::cout << "[thread " << tid.str() << "] " << msg << std::endl;
    }

private:
    static std::mutex& mutex_() {
        static std::mutex m;
        return m;
    }
};