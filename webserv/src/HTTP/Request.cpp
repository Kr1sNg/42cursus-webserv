#include "../../includes/Request.hpp"
#include "../../includes/webserv.hpp"
//waiting for adding the good _uri (source path)

Request::Request(): _method("GET"), _uri(""), _version("HTTP/1.0"), _body("")
{
    _headers["Connection"] = "keep-alive";
}

Request::Request(const std::string &ogRequest, const Serverconfig &conf): _servConfig(conf), _body("")
{
    size_t start = 0;

    if (ogRequest.empty()) {
        return ;
    }
    while (start < ogRequest.size()) {
        size_t index = ogRequest.find("\r\n", start);
        if (index == std::string::npos)
            break ;
        std::string line = ogRequest.substr(start, index - start);
        if (start == 0)
            parseFirstLine(line);
        else if (line.empty()) {
            size_t bodypart = index + 2;
            _body = trimSpace(ogRequest.substr(bodypart));
            break;
            //end of headers, go to body
        }
        else
            parsingForHeader(line);
        start = index + 2;
    }
    ContentLengthParser();
}

Request::Request(const Request &cpy) 
    : _servConfig(cpy._servConfig), _method(cpy._method), _uri(cpy._uri), _version(cpy._version),
    _headers(cpy._headers), _body(cpy._body) {
}

Request &Request::operator=(const Request &other) {
    if (this != &other) {
        _servConfig = other._servConfig;
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

std::string Request::getHeader(const std::string &keyword) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(keyword);
    if (it != _headers.end())
        return (it->second);
    return ("");
}

const std::map<std::string, std::string> &Request::getHeaders() const {
    return _headers;
}

const std::string &Request::getBody() const {
    return _body;
}

const Serverconfig &Request::getConfig() const {
    return _servConfig;
}

// /*setters*/
// void Request::setMethod(const std::string &method) {
//     _method = method;
// }

// void Request::setUri(const std::string &uri) {
//     _uri = uri;
// }

// void Request::setVersion(const std::string &version) {
//     _version = version;
// }

// void Request::setHeaders(const std::map<std::string, std::string> &headers) {
//     _headers = headers;
// }

// void Request::setBody(const std::string &body) {
//     _body = body;
// }

// void Request::setConfig(const Serverconfig &conf) {
//     _conf = &conf;
// }

// void Request::addHeader(const std::string &key, const std::string &val) {
//     _headers[key] = val;
// }