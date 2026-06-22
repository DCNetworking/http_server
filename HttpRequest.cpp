// HttpRequest.cpp
#include "HttpRequest.h"
#include <sstream>

HttpRequest HttpRequest::parse(const std::string& raw) {
    HttpRequest req;
    std::istringstream stream(raw);
    std::string line;
    std::getline(stream, line);
    {
        std::istringstream first(line);
        first >> req.method >> req.path >> req.version;
    }
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = line.substr(0, colon);
        std::string val = line.substr(colon + 1);
        req.headers[key] = val;
    }

    return req;
}