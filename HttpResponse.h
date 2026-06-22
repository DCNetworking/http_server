#pragma once
#include <string>
#include <map>

class HttpResponse {
public:
    int status_code = 200;
    std::string status_text = "OK";
    std::string body;
    std::map<std::string, std::string> headers;

    void set_body(const std::string& content, const std::string& type = "text/html");
    std::string to_string() const;
};