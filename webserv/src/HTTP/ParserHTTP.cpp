#include "../../includes/webserv.hpp"

int Request::methodCheck(const std::string &met) {
    if (met != "GET" && met != "POST" && met != "DELETE")
        return (0);
    return (1);
}

int Request::verCheck(const std::string &version) {
    if (version != "HTTP/1.1" && version != "HTTP/1.0")
        return (0);
    return (1);
}

std::string trimSpace(const std::string &str) {
    size_t start = 0;
    size_t end = 0;

    while (start < str.size() && isspace(str[start]))
        ++start;
    if (str.size() == start)
        return "";
    end = str.size() - 1;
    while (end > start && isspace(str[end]))
        end--;
    return (str.substr(start, end - start + 1));
}

std::string Request::getHeader(const std::string &search) const {
    std::map<std::string, std::string>::const_iterator iterator = _headers.find(search);
    if (iterator != _headers.end())
        return iterator->second; // second here point the value of the key we are searching for
    return "";
}

void  Request::parseFirstLine(std::string &line, Request &req) {
    //each time there are a space it's a new category of the line
    //method, and uri, and after version.
    size_t first = line.find(' ');
    size_t second = line.find(' ', first + 1);

    if (first == std::string::npos || second == std::string::npos) {
        std::cerr << "error: invalid request line" << std::endl;
        return ;
    }
    std::string met = line.substr(0, first);
    if (!methodCheck(met)) {
        std::cerr << "error: Method incorrect." << std::endl;
        return ;
    }
    std::string link =  line.substr(first + 1, second - first - 1);
    if (link.empty()) {
        std::cerr << "error: link is empty." << std::endl;
        return ;
    }
    std::string ver =  line.substr(second + 1);
    if (!verCheck(ver)) {
        std::cerr << "error: Version incorrect." << std::endl;
        return ;
    }
    req.setMethod(met);
    req.setUri(link);
    req.setVersion(ver);
}

static void parsingForHeader(const std::string &line, Request &req) {
    size_t cmn;

    cmn = line.find(":");
    if (cmn == std::string::npos) {
        std::cerr << "error: invalid header" << std::endl;
        return ;
    }
    std::string before = trimSpace(line.substr(0, cmn));
    if (before.empty()) {
        std::cerr << "error: invalid key" << std::endl;
        return ;
    }
    if (before.find(' ') != std::string::npos) {
        std::cerr << "error: header key can't contain space." << std::endl;
        return ;
    }
    std::string after = trimSpace(line.substr(cmn + 1));
    if (after.empty()) {
        std::cerr << "error: value can't be empty" << std::endl;
        return ;
    }
    req.addHeader(before, after);
}

void Request::ContentLengthParser(Request &request) {
    // rechercher Request-Header dans le std::map
    std::string existing_CL = request.getHeader("Content-Length");
    // verifier si l'entête existe et que sa valeur n'est pas nulle
    if (existing_CL.empty())
        return ; // Content-Length not found
    // Convertir sa valeur (string) en int
    size_t contentLen = std::atoi(existing_CL.c_str()); // c_str convert the std::string in a const char *
    std::string bodyCheck = request.getBody();
    // Vérifier que sa valeur est bien un nombre valide (pas nul/correspondant au bodylen)
    if (bodyCheck.size() < contentLen)
        std::cerr << "Error: body is too short" << std::endl;
    else if (bodyCheck.size() > contentLen)
        request.setBody(bodyCheck.substr(0, contentLen));
}

Request	Request::parserForRequest(const std::string &ogRequest, const Serverconfig &conf) { //to do : check conf
    size_t start = 0;
    Request request;

    if (ogRequest.empty()) {
        std::cerr << "error: empty request" << std::endl;
        return request;
    }
    while (start < ogRequest.size()) {
        size_t index = ogRequest.find("\r\n", start);
        if (index == std::string::npos)
            break ;
        std::string line = ogRequest.substr(start, index - start);
        if (start == 0)
            parseFirstLine(line, request);
        else if (line.empty()) {
            size_t bodypart = index + 2;
            request.setBody(trimSpace(ogRequest.substr(bodypart)));
            break;
            //end of headers, go to body
        }
        else
            parsingForHeader(line, request);
        start = index + 2;
    }
    ContentLengthParser(request);
    request.setConfig(conf);
    return request;

}

bool fileExists(const std::string &path)
{
    struct stat s;
    return (stat(path.c_str(), &s) == 0);
}

int Request::CompareConfig(const Serverconfig &conf) const {
    if (this->getBody().size() > conf.getClient_max_size())
        return 413;
    
    // Version simplifiée pour verifier la methode, à modifier avec la location
    const std::string &method = this->getMethod();
    if (method != "GET" && method != "POST" && method != "DELETE")
        return 405;
    
    std::string path = conf.getRoot() + this->getUri();
    if (!fileExists(path))
        return 404;
    return 200;
}