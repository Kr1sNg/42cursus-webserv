#include "../../includes/webserv.hpp"
# include "../../includes/Response.hpp"
#include <iostream>

void printRequest(const Request &req) {
	std::cout << "-----------------------------" << std::endl;
	std::cout << "Method:  " << req.getMethod() << std::endl;
	std::cout << "URI:     " << req.getUri() << std::endl;
	std::cout << "Version: " << req.getVersion() << std::endl;

	std::cout << "\nHeaders:" << std::endl;
	std::map<std::string, std::string> headers = req.getHeaders();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
		std::cout << "  " << it->first << ": " << it->second << std::endl;

	std::cout << "\nBody:    [" << req.getBody() << "]" << std::endl;
	std::cout << "-----------------------------" << std::endl;
}

/*int main() {
	std::string req1 =
		"GET /index.html HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"User-Agent: curl/7.64.1\r\n"
		"\r\n";

	std::string req2 =
		"POST /form HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Length: 11\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"Hello World";

	std::string req3 =
		"DELETE /data/item42 HTTP/1.0\r\n"
		"Host: localhost\r\n"
		"\r\n";

	std::cout << "=== Test 1 : GET ===" << std::endl;
	Request r1 = Request::parserForRequest(req1);
	printRequest(r1);

	std::cout << "\n=== Test 2 : POST ===" << std::endl;
	Request r2 = Request::parserForRequest(req2);
	printRequest(r2);

	std::cout << "\n=== Test 3 : DELETE ===" << std::endl;
	Request r3 = Request::parserForRequest(req3);
	printRequest(r3);

	return 0;
}*/
int main() {
	Response res;

    res.setCode(404);
    res.setReason("Not Found");
    res.setHeader("Content-Type", "text/html");
    res.setBody("<html><body><h1>404 Not Found</h1></body></html>");

	res.MoreCookie("sessionid=abc42; Http; Path=/index");
    res.MoreCookie("theme=dark; Path=/index");

    std::cout << res.getString() << std::endl;

	Response error404 = Response::buildError(404, "Not Found");
	std::cout << error404.getString() << std::endl;	

	Response redirect302 = Response::buildRedirect("/index.html", 302);
	std::cout << redirect302.getString() << std::endl;
}
