#include "../includes/webserv.hpp"


const std::string trimSpace(const std::string &str) {
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

static void  parseFirstLine(std::string &line, Request &req) {
    //each time there are a space it's a new category of the line
    //method, and uri, and after version.
    size_t first = line.find(' ');
    size_t second = line.find(' ', first + 1);

    std::string met = line.substr(0, first);
    std::string link =  line.substr(first + 1, second - first - 1);
    std::string ver =  line.substr(second + 1);

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
    std::string after = trimSpace(line.substr(cmn + 1));
    if (after.empty()) {
        std::cerr << "error: value can't be empty" << std::endl;
        return ;
    }
    req.addHeader(before, after);
}

Request	Request::parserForRequest(const std::string &ogRequest) {
    size_t start = 0;
    Request request;

    while (start < ogRequest.size()) {
        size_t index = ogRequest.find("\r\n", start);
        if (index == std::string::npos)
            break ;
        std::string line = ogRequest.substr(start, index - start);
        if (start == 0)
            parseFirstLine(line, request);
        else if (line.empty()) {
            size_t bodypart = index + 2;
            request.setBody(ogRequest.substr(bodypart));
            break;
            //end of headers, go to body
        }
        else
            parsingForHeader(line, request);
        start = index + 2;
    }

    return request;

}
