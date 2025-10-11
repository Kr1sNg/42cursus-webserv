#include "../../includes/webserv.hpp"


std::string Response::getString() const {
    std::ostringstream fd;

    fd << _version << " " << _status << " " << _reason << "\r\n";
    fd << "Content-Length: " << _body.size() << "\r\n";
    fd << "Connection: close\r\n";

    for (std::map<std::string, std::string>::const_iterator iterator = _headers.begin();
        iterator != _headers.end(); ++iterator) {
            if (!(iterator->first.empty()))
                fd << iterator->first << ": " << iterator->second << "\r\n";
    }
    fd << "\r\n";
    fd << _body;

    return (fd.str());
}


Response::Response()
    : _version("HTTP/1.1"), _status(200), _reason("OK"), _body("") {
        _headers[""] = "";
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
    return *this;
}

Response::~Response() {
}
/* setters */
void Response::setVersion(const std::string &version) {
    _version = version;
}

void Response::setCode(int code) {
    _status = code;
}

void Response::setReason(const std::string &reason) {
    _reason = reason;
}

void Response::setHeader(const std::string &key, const std::string &val) {
    if (key.empty()) {
        std::cerr << "error: empty header key" << std::endl;
        return ;
    }
    _headers[key] = val;
}

void Response::setBody(const std::string &body) {
    _body = body;
}

/* getters */
const std::string &Response::getVersion() const {
    return _version;
}

int	Response::getCode() const {
    return _status;
}

const std::string &Response::getReason() const {
    return _reason;
}

const std::map<std::string, std::string> &Response::getHeaders() const {
    return _headers;
}

const std::string &Response::getBody() const {
    return _body;
}


