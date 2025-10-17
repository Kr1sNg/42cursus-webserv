#include "../../includes/webserv.hpp"

std::string Response::convertFileToString(const std::string &path) {
    std::ifstream   fd(path.c_str());
    std::string     content;
    std::string     line;

    if (!fd.is_open()) {
        return "";
    }
    while (std::getline(fd, line)) {
        content += line;
        content += "\n";
    }
    fd.close();
    return content;
}

std::string Response::getString() const {
    std::ostringstream fd;

    fd << _version << " " << _status << " " << _reason << "\r\n";
    if (!_needChunked)
        fd << "Content-Length: " << _body.size() << "\r\n";
    if (_keepAlive)
        fd << "Connection: keep-alive\r\n";
    else
        fd << "Connection: close\r\n";

    for (std::map<std::string, std::string>::const_iterator iterator = _headers.begin();
        iterator != _headers.end(); ++iterator) {
            if (!(iterator->first.empty()))
                fd << iterator->first << ": " << iterator->second << "\r\n";
    }
    for (size_t i = 0; i < _cookie.size(); ++i) {
        fd << "Set-Cookie: " << _cookie[i] << "\r\n";
    }
    if (_needChunked)
        fd << "Transfer-Encoding: chunked\r\n";
    fd << "\r\n";
    if (!_needChunked) {
        fd << _body;
    }
    else {
        if (!_body.empty())
            fd << std::hex << _body.size() << "\r\n" << _body << "\r\n";
        fd << "0\r\n\r\n";
    }

    return (fd.str());
}

Response::Response(const Request &req)
    : _version(req.getVersion()),
      _status(200),
      _reason("OK"),
      _body(""),
   
      _needChunked(false),
         _keepAlive(true)
{
    _headers["Server"] = "Webserv/1.0";
    std::string connection = req.getHeader("Connection");
    if (connection == "close")
        _keepAlive = false;
    else if (connection == "keep-alive")
        _keepAlive = true;
/* fonction pour get le type de contenue à faire
    _headers["Content-Type"] = getType(req.getUri());
*/
}

std::string    Response::BuildFromRequest(const Request &req) {
    std::ostringstream len;

    std::string path = req.getUri();
    std::string content;
    
    content = convertFileToString("www" +req.getUri() + "indx.html");
    if (content.empty())
    {
       // Response::setBody(req.getBody() + content);
        Response::setCode(404);
        Response::setReason("Not-Found");
        Response::setChunked(true);
        content = convertFileToString("www/error_pages/404_notfound.html");
        Response::setBody(content);
    }
    else if (req.getMethod() == "POST") {
        Response::setBody(req.getBody() + content);
        Response::setCode(200);
        Response::setReason("OK");
        Response::setChunked(true);
    }
    else {
        Response::setBody(content);
        Response::setCode(200);
        Response::setReason("OK");
    }
    /* content type (to do)
    _headers["Content-Type"] = getType(path); */

    return (getString() + Response::getBody());
}

Response::Response()
    : _version("HTTP/1.1"), _status(200), _reason("OK"), _body(""), _keepAlive(true) {
        _headers["Server"] = "Webserv/1.0";
        _headers["Content-Type"] = "text/html; charset=utf-8";
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

void Response::setKeepAlive(bool keep) {
    _keepAlive = keep;
}

void Response::MoreCookie(const std::string &cookie) {
    _cookie.push_back(cookie);
}
/* push_back add at the end of the vector */

void Response::setChunked(bool needchunk) {
    _needChunked = needchunk;
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

bool Response::getKeepAlive() const {
    return _keepAlive;
}

const std::vector<std::string> Response::EatCookie() const {
    return _cookie;
}

bool Response::getChunked() const {
    return _needChunked;
}
