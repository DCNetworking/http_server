
#include "HttpResponse.h"
#include <sstream>

void HttpResponse::set_body(const std::string& content, const std::string& type) {
    body = content;
    headers["Content-Type"] = type;
    headers["Content-Length"] = std::to_string(body.size());
}

std::string HttpResponse::to_string() const {
    std::ostringstream out;
    out << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
    for (const auto& [key, val] : headers)
        out << key << ": " << val << "\r\n";
    out << "\r\n" << body;
    return out.str();
}