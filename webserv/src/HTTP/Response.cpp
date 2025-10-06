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

/*getters*/
std::string	Response::getVersion() const {
    return _version;
}

int			Response::getStatus() const {
    return _status;
}

std::string	Response::getReason() const {
    return _reason;
}

std::map<std::string, std::string> Response::getHeaders() const {
    return _headers;
}

std::string	Response::getBody() const {
    return _body;
}

/*setters*/
void	Response::setVersion(const std::string &version) {
    _version = version;
}

void	Response::setStatus(const int &statut) {
    _status = statut;
}

void    Response::setReason(const std::string &reason) {
    _reason = reason;
}

void	Response::setHeaders(const std::map<std::string, std::string> &headers) {
    _headers = headers;
}

void	Response::setBody(const std::string &body) {
    _body = body;
}
