#include "../../includes/webserv.hpp"

Response::Response():
        _version("HTTP/1.1"),
        _status(200),
        _reason("OK"),
        _keepAlive(true),    //"close" or "keep-alive"
        _isBodyFromFile(false)
{
}

Response::~Response()
{
}

Response::Response(const Response &cpy)
{
    *this = cpy;
}

Response    &Response::operator=(const Response &other)
{
    if (this != &other)
    {
        _version = other._version;
        _status = other._status;
        _reason = other._reason;
        _headers = other._headers;
        _keepAlive = other._keepAlive;
        _cookies = other._cookies;
        _generatedBody = other._generatedBody;
        _filePath = other._filePath;
        _isBodyFromFile = other._isBodyFromFile;
    }
    return *this;
}

/* setters */

void Response::setStatus(int code, const std::string &reason) {
    _status = code;
    _reason = reason;
}

void Response::setHeader(const std::string &key, const std::string &val) {
    _headers[key] = val;
}

void Response::setKeepAlive(bool keep) {
    _keepAlive = keep;
    if (_keepAlive)
        _headers["Connection"] = "keep-alive";
    else
        _headers["Connection"] = "close";
}

void    Response::addCookie(const std::string &cookie)
{
    _cookies.push_back(cookie);
}

void    Response::setBody(const std::string &body)
{
    _generatedBody = body;
    _isBodyFromFile = false; //this is a genrated body, not a file
    _filePath.clear();

    //Automaitclally set Content-length for generated bodies
    setHeader("Content-Length", intToStr(body.length()));
}

void    Response::buildFromFile(std::string const &path, Serverconfig conf)
{
    std::ifstream   file(path.c_str(), std::ios::binary);
    if (!file.is_open())
        return (buildError(404, "Not Found", conf));

    // std::ostringstream oss;
    // oss << file.rdbuf();
    // std::string body = oss.str();

    // take only file size
    file.seekg(0, std::ios::end);
    long length = file.tellg();
    file.close();

    if (length == -1)
        return (buildError(500, "Internal Server Error", conf));

    _filePath = path;
    _isBodyFromFile = true;
    _generatedBody.clear();

    setHeader("Content-Length", intToStr(length));
    setHeader("Content-Type", getType(path));
}

void    Response::buildError(int code, std::string const &reason, Serverconfig conf)
{
    setStatus(code, reason);
    setKeepAlive(false);

    std::ostringstream path;
    // path << "./www/error/" << code << ".html";
    path << conf.getError_pages(code);

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
}

/* getters */
int	Response::getStatus() const {
    return _status;
}

const std::string &Response::getReason() const {
    return _reason;
}

bool Response::getKeepAlive() const
{
    return _keepAlive;
}

bool Response::isBodyFromFile() const
{
    return _isBodyFromFile;
}

const std::string& Response::getFilePath() const
{
    return _filePath;
}

const std::string &Response::getBody() const {
    return _generatedBody;
}

std::string Response::getHeaderString(void) const
{
    std::stringstream   ss;

    //1. Status line
    ss << _version << " " << _status << " " << _reason << "\r\n";

    //2. Headers
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        ss << it->first << ": " << it->second << "\r\n";
    
    //3. Cookies
    for (std::vector<std::string>::const_iterator it = _cookies.begin(); it != _cookies.end(); ++it)
        ss << "Set-Cookie: " << *it << "\r\n";

    //4. Final CRLF
    ss << "\r\n";

    return (ss.str());
}

std::string Response::getType(std::string const &path)
{
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos)
        return ("application/octet-stream");

    std::string ext = path.substr(dot + 1);
    if (ext == "html" || ext == "htm")
        return ("text/html");
    if (ext == "css")
        return ("text/css");
    if (ext == "js")
        return ("text/javascript");
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

// void    Response::buildCGI(const std::string& content)
// {
//     std::istringstream  stream(content);
//     std::string         line;

//     while (std::getline(stream, line))
//     {
//         if (line == "\r")
//             break;

//     size_t pos = content.find(":");

//     if (pos != std::string::npos)
//     {
//         if (content.substr(0, pos) == "Content-Type")
//         {
//             setHeader("Content-Type", content.substr(pos + 1));
//         }
//         else if (content.substr(0, pos) == "status")
//         {
//             int status;
//             size_t pos2 = content.substr(pos + 1).find(" ");
//             std::stringstream ss(content.substr(pos + 1));
//             ss >> status;
//             setStatus(status, content.substr(pos2 + 1));
//         }
//         else
//         {
//             setHeader(content.substr(0, pos), content.substr(pos + 1));
//         }
//     }
//     }
//     while (std::getline(stream, line))
//     {
//         _body += line + "\n";
//     }
// }







// static std::string getBoundary(const std::string &contentType)
// {
//     std::string key = "boundary=";
//     size_t pos = contentType.find(key);
//     if (pos == std::string::npos)
//         return ("");
//     std::string boundary = contentType.substr(pos + key.length());

//     if (!boundary.empty() && boundary[0] == '"')
//         boundary = boundary.substr(1, boundary.size() - 2);
//     return ("--" + boundary);
// }

// static std::vector<std::string>    splitByBoundary(const std::string &body, const std::string &boundary)
// {
//     std::vector<std::string>    parts;
//     size_t  start = 0;
//     size_t  end;

//     while (true)
//     {
//         start = body.find(boundary, start);
//         if (start == std::string::npos)
//             break ;

//         start += boundary.length();

//         if (body.substr(start, 2) == "\r\n")
//             start += 2;

//         end = body.find(boundary, start);
//         if (end == std::string::npos)
//             break ;
        
//         std::string part = body.substr(start, end - start);
//         parts.push_back(part);
//         start = end;
//     }
//     return (parts);
// }

// static std::string  extractFilename(const std::string &part)
// {
//     std::string key = "filename=\"";
//     size_t  pos = part.find(key);
//     if (pos == std::string::npos)
//         return ("");
//     size_t start = pos + key.length();
//     size_t end = part.find("\"", start);
//     if (end == std::string::npos)
//         return ("");
//     return (part.substr(start, end - start));
// }

// static std::string  extractFileContent(const std::string &part)
// {
//     size_t pos = part.find("\r\n\r\n");
//     if (pos == std::string::npos)
//         return ("");
//     pos += 4;
//     std::string content = part.substr(pos);

//     if (content.size() >= 2 && content.substr(content.size() - 2) == "\r\n")
//         content.erase(content.size() - 2);
//     return (content);
// }


// void    Response::handleFileUpload(const Request &req)
// {
//     std::string contentType = req.getHeader("Content-Type");
//     std::string boundary = getBoundary(contentType);
//     std::string body = getBody();

//     if (boundary.empty())
//         return ;

//     //take file content
//     size_t  fileStart = body.find("\r\n\r\n");
//     if (fileStart == std::string::npos)
//         return ;
//     fileStart += 4;

//     size_t  fileEnd = body.find("--" + boundary, fileStart);
//     if (fileEnd == std::string::npos)
//         return ;

//     std::string fileContent = body.substr(fileStart, fileEnd - fileStart);

//     // take filename
//     std::string fileName = "upload.bin";
//     size_t  fnPos = body.find("filename=\"");
//     if (fnPos != std::string::npos)
//     {
//         fnPos += 10;
//         size_t fnEnd = body.find("\"", fnPos);
//         fileName = body.substr(fnPos, fnEnd - fnPos);
//     }

//     //save to uploads
//     std::ofstream   ofs("./www/uploads/" + fileName, std::ios::binary);
//     if (ofs.is_open())
//     {
//         ofs.write(fileContent.c_str(), fileContent.size());
//         ofs.close();
//     }
//     else
//         std::cerr << "Failed to open upload directory or file" << std::endl;

// }
