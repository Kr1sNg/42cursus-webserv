#include "../../includes/webserv.hpp"

Response::Response():
        _version("HTTP/1.1"),
        _status(200),
        _reason("OK"),
        _keepAlive(true)
{
}

static bool fileExists(const std::string &path)
{
    struct stat  st;
    return (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
}

Response::Response(Request const &req, Serverconfig const &conf):
        _version(req.getVersion()),
        _status(200),
        _reason("OK"),
        _servConfig(conf),
        _keepAlive(true)
{
    // set keepAlive
    if (req.getVersion() == "HTTP/1.0")
        _keepAlive = false;
    std::string conn = req.getHeader("Connection");
    if (conn == "close")
        _keepAlive = false;
    else if (conn == "keep-alive")
        _keepAlive = true;
    
    if (req.getMethod() == "GET")
    {
        std::string filePath = std::string(WWW) + req.getUri();
        if (filePath[filePath.size() - 1] == '/')
            filePath += "index.html";
        
        if (fileExists(filePath))
            buildFromFile(filePath);
        else
            buildError(404, "Not Found");
    }
    else if (req.getMethod() == "POST")
    {
        handleFileUpload(req); // TODO
        buildFromFile("./www/notif/upload_success.html");
    }
    else if (req.getMethod() == "DELETE")
    {

    }
    else
        buildError(405, "Method Not Allowed");
}


Response::~Response()
{
}

/* setters */
void Response::setVersion(const std::string &version) {
    _version = version;
}

void Response::setStatus(int code, const std::string &reason) {
    _status = code;
    _reason = reason;
}

void Response::setHeader(const std::string &key, const std::string &val) {
    _headers[key] = val;
}

void Response::setBody(const std::string &body) {
    _body = body;
}

void Response::setKeepAlive(bool keep) {
    _keepAlive = keep;
}

/* getters */
const std::string &Response::getVersion() const {
    return _version;
}

int	Response::getStatus() const {
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


// void Response::setKeepAlive(bool keep) {
//     _keepAlive = keep;
// }

// Response::Response(Request const &req, Serverconfig const &conf):
//         _version(req.getVersion()),
//         _status(0), // defaut code ?
//         _reason(""), // defaut ?
//         _body(""),
//         _needChunked(false),
//         _keepAlive(true),
//         _cookie()
// {
//     std::string path = req.getUri();
//     std::string content;
//     int indexMatch = matchLocation(path, conf);
//     if (indexMatch != -1 && conf.getLocations()[indexMatch].getCgi_pass() != "")
//     {
//         _body = cgiHandle(req, conf.getLocations()[indexMatch]);
//         _status = 200;
//         _reason = "OK"
//         _

//     }
// }

std::string Response::toString(void) const
{
    std::ostringstream oss;
    oss << _version << " " << _status << " " << _reason << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        oss << it->first << ": " << it->second << "\r\n";
    oss << "\r\n";
    oss << _body;
    return (oss.str());
}

// std::string Response::convertFileToString(const std::string &path) {
//     std::ifstream   fd(path.c_str());
//     std::string     content;
//     std::string     line;

//     if (!fd.is_open()) {
//         return "";
//     }
//     while (std::getline(fd, line)) {
//         content += line;
//         content += "\n";
//     }
//     fd.close();
//     return content;
// }

// Response Response::makeSimple(int code, const std::string &reason, )

// std::string Response::getString() const {
//     std::ostringstream fd;

//     fd << _version << " " << _status << " " << _reason << "\r\n";
//     if (!_needChunked)
//         fd << "Content-Length: " << _body.size() << "\r\n";
//     if (_keepAlive)
//         fd << "Connection: keep-alive\r\n";
//     else
//         fd << "Connection: close\r\n";

//     for (std::map<std::string, std::string>::const_iterator iterator = _headers.begin();
//         iterator != _headers.end(); ++iterator) {
//             if (!(iterator->first.empty()))
//                 fd << iterator->first << ": " << iterator->second << "\r\n";
//     }
//     for (size_t i = 0; i < _cookie.size(); ++i) {
//         fd << "Set-Cookie: " << _cookie[i] << "\r\n";
//     }
//     if (_needChunked)
//         fd << "Transfer-Encoding: chunked\r\n";
//     fd << "\r\n";
//     if (!_needChunked) {
//         fd << _body;
//     }
//     else {
//         if (!_body.empty())
//             fd << std::hex << _body.size() << "\r\n" << _body << "\r\n";
//         fd << "0\r\n\r\n";
//     }

//     return (fd.str());
// }


// int matchLocation(std::string url, const Serverconfig& serverconfig)
// {
//     int     save = -1; 
//     size_t     i = 0;
//     size_t  bestLength = 0;

//     while (i < serverconfig.getLocations().size()) 
//     {
//         if (url.rfind(serverconfig.getLocations()[i].getArg(), 0) == 0 && bestLength < serverconfig.getLocations()[i].getArg().size())
//         {
//             bestLength = serverconfig.getLocations()[i].getArg().size();
//             save = i;
//         }
//         i++;
//     }
//     return (save);
// }

// std::string    Response::BuildFromRequest(const Request &req, const Serverconfig& serverconfig) {
//     std::ostringstream len;

//     std::string path = req.getUri();
//     std::string content;
//     int indexMatch = 0;

//     indexMatch = matchLocation(path, serverconfig);
//     if (indexMatch != -1 && serverconfig.getLocations()[indexMatch].getCgi_pass() != "")
//     {
//         content = cgiHandle(req, serverconfig.getLocations()[indexMatch]);
//         Response::setBody(content);
//         Response::setCode(200);
//         Response::setReason("OK");
//     }
//     else
//     {
//         content = convertFileToString("www" +req.getUri() + "index.html");
//         if (content.empty())
//         {
//         // Response::setBody(req.getBody() + content);
//             Response::setCode(404);
//             Response::setReason("Not-Found");
//             Response::setChunked(true);
//             content = convertFileToString("www/error_pages/404_notfound.html");
//             Response::setBody(req.getBody() + content);
//         }
//         else if (req.getMethod() == "POST") {
//             Response::setBody(req.getBody() + content);
//             Response::setCode(200);
//             Response::setReason("OK");
//             Response::setChunked(true);
//         }
//         else {
//             Response::setBody(content);
//             Response::setCode(200);
//             Response::setReason("OK");
//         }
//     }
//     /* content type (to do)
//     _headers["Content-Type"] = getType(path); */

//     return (getString() /* + Response::getBody() */);
// }


Response::Response(const Response &cpy):
        _version(cpy._version),
        _status(cpy._status),
        _reason(cpy._reason),
        _headers(cpy._headers),
        _body(cpy._body),
        _servConfig(cpy._servConfig),
        _keepAlive(cpy._keepAlive)
{
}

Response &Response::operator=(const Response &other) {
    if (this != &other) {
        _version = other._version;
        _status = other._status;
        _reason = other._reason;
        _headers = other._headers;
        _body = other._body;
        _servConfig = other._servConfig;
        _keepAlive = other._keepAlive;
    }
    return *this;
}


// void Response::MoreCookie(const std::string &cookie) {
//     _cookie.push_back(cookie);
// }
// /* push_back add at the end of the vector */

// void Response::setChunked(bool needchunk) {
//     _needChunked = needchunk;
// }



// const std::vector<std::string> Response::EatCookie() const {
//     return _cookie;
// }

// bool Response::getChunked() const {
//     return _needChunked;
// }

static std::string intToStr(int n)
{
    std::ostringstream  oss;
    oss << n;
    return (oss.str());
}


void    Response::buildFromFile(std::string const &path)
{
    std::ifstream   file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return (buildError(404, "Not Found"));

    std::ostringstream oss;
    oss << file.rdbuf();
    std::string body = oss.str();

    setStatus(200, "OK");
    setHeader("Content-Lenght", intToStr(body.size()));
    setHeader("Content-Type", getType(path));
    setBody(body);

    file.close();
}

void    Response::buildError(int code, std::string const &reason)
{
    setVersion("HTTP/1.1");
    setStatus(code, reason);

    std::ostringstream path;
    path << "./www/errors/" << code << ".html";

    std::ifstream   file(path.str().c_str(), std::ios::binary);
    std::string body;

    if (file.is_open())
    {
        std::ostringstream oss;
        oss << file.rdbuf();
        body = oss.str();
        file.close();
    }
    else
    {
        std::ostringstream oss;
        oss << "<html><head><title>" << code << " " << reason
            << "</title></head><body><h1>"
            << code << " " << reason
            << "</h1><p>The requested page could not be found.</p></body></html>";
        body = oss.str();
    }

    setBody(body);
    setHeader("Content-Type", "text/html");
    setHeader("Content-Length", intToStr(body.size()));
    setKeepAlive(false);
}

std::string Response::getType(std::string const &path)
{
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return ("text/plain");

    std::string ext = path.substr(dot + 1);
    if (ext == "html" || ext == "htm")
        return ("text/html");
    if (ext == "css")
        return ("text/css");
    if (ext == "js")
        return ("application/javascript");
    if (ext == "png")
        return ("image/png");
    if (ext == "jpg" || ext == "jpeg")
        return ("image/jpeg");
    if (ext == "gif")
        return ("image/gif");
    if (ext == "txt")
        return ("text/plain");
    return ("application/octet-stream");
}

void    Response::handleFileUpload(const Request &req)
{
    (void)req;
    // receive file upload and save to ./www/uploads
}
