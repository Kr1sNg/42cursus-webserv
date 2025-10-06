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

/*getters*/
const std::string &Request::getMethod() const {
    return _method;
}

const std::string &Request::getUri() const {
    return _uri;
}

const std::string &Request::getVersion() const {
    return _version;
}

const std::map<std::string, std::string> &Request::getHeaders() const {
    return _headers;
}

const std::string &Request::getBody() const {
    return _body;
}

/*setters*/
void Request::setMethod(const std::string &method) {
    _method = method;
}

void Request::setUri(const std::string &uri) {
    _uri = uri;
}

void Request::setVersion(const std::string &version) {
    _version = version;
}

void Request::setHeaders(const std::map<std::string, std::string> &headers) {
    _headers = headers;
}

void Request::setBody(const std::string &body) {
    _body = body;
}

void Request::addHeader(const std::string &key, const std::string &val) {
    _headers[key] = val;
}