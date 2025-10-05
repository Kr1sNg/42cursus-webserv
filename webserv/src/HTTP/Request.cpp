#include "../includes/Request.hpp"

//waiting for adding the good _uri (source path)

Request::Request()
    : _method("GET"), _uri("/index"), _version("HTTP/1.1"),
    _headers("User-Agent", ""), _body("") {
}

Request::Request(const Request &cpy) 
    : _method(cpy._method), _uri(cpy._uri), _version(cpy._version),
    _headers(cpy._headers), _body(cpy._body) {
}

Request &Request::operator=(const Request &other) {
    if (this != &other) {
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        _body = other._body;
    }
    return *this;
}

Request::~Request() {
}