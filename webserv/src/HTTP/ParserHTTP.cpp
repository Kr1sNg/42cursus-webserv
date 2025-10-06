#include "../includes/webserv.hpp"

static void  parseFirstLine(std::string &line, Request &req) {
    //each time there are a space it's a new category of the line
    //method, and uri, and after version.
}

static void parsingForHeader(std::string &line, Request &req) {

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
