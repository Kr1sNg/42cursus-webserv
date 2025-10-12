#include "../../includes/webserv.hpp"

Response Response::buildError(int code, const std::string &reason) {
    Response rep;

    rep.setCode(code);
    rep.setReason(reason);

    std::string Errorbody = "<html><head><title>" + std::to_string(code) + " " + reason +
        "</title></head>" "<body><h1>" + std::to_string(code) + " " + reason +
            "</h1><p>Default error page.</p></body></html>";
    rep.setBody(Errorbody);
    rep.setHeader("Content-Type", "text/html; charset=utf-8");
    rep.setKeepAlive(false);

    return rep;
}
