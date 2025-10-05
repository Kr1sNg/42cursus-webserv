#include "../includes/Response.hpp"

Response::Response()
    : _version("HTTP/1.1"), _status(200), _reason("OK"),
    _headers("", ""), _body("") {
}

Response::Response(const Response &cpy)
    : _version(cpy._version), _status(cpy._status), _reason(cpy._reason),
    _headers(cpy._headers), _body(cpy._body) {
}

Response &Response::operator=(const Response &other) {
    if (this != &other) {
        _version = other._version;
        _status = other._status;
        _reason = other._reason;
        _headers = other._headers;
        _body = other._body;
    }
}

Response::~Response() {
}