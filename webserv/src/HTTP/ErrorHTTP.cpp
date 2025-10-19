#include "../../includes/webserv.hpp"

static std::string getReasonPhrase(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default:  return "Error";
    }
}

Response Response::buildError(int code, const std::string &reason) {
    Response rep;

    rep.setCode(code);
    rep.setReason(reason);
    if (reason == "Error" || reason.empty())
        rep.setReason(getReasonPhrase(code));
    else
        rep.setReason(reason);
    rep.setHeader("Content-Type", "text/html; charset=utf-8");
    rep.setHeader("Connection", "close");

    try {
        std::ostringstream path;
        path << "./www/errors/" << code << ".html";
        rep.setBody(convertFileToString(path.str()));
    }
    catch (const std::exception &e) {
        std::ostringstream ErrorBody;
        ErrorBody << "<html><head><title>" << code << " " << reason
            << "</title></head><body><h1>" << code << " " << reason
            << "</h1><p>Default error page.</p></body></html>";
        rep.setBody(ErrorBody.str());
    }
    return rep;
}

Response Response::buildRedirect(const std::string &location, int code) {
    Response rep;

    std::string reason;
    switch(code) {
        case 301:
        reason = "Moved Permanently";
        break ;
        case 302:
        reason = "Found";
        break ;
        case 303:
        reason = "See Other";
        break ;
        case 307:
        reason = "Temporary Redirect";
        break ;
        case 308:
        reason = "Permanently Redirect";
        break ;
        default:
        reason = "Redirect";
        break ;
    }
    rep.setCode(code);
    rep.setReason(reason);
    rep.setHeader("Location", location);
    std::ostringstream oss;
    oss << code; 
    std::string code_str = oss.str();
    std::string Redirectbody = "<html><head><title>" + code_str + " " + reason +
         "</title></head>" "<body><h1>" + code_str + " " + reason +
            "</h1><p>Redirecting to <a href=\"" + location + "\">" +
                location + "</a></p></body></html>";
    rep.setBody(Redirectbody);
    rep.setHeader("Content-Type", "text/html; charset=utf-8");
    rep.setKeepAlive(false);

    return rep;
}
