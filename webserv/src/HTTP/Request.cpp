/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 13:03:07 by cfiachet          #+#    #+#             */
/*   Updated: 2025/09/26 14:18:01 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

Request::Request()
	: method(""),
	uri(""),
	version("HTTP/1.1"),
	headers(),
	body() {
}

Request	parseHTTPRequest(const std::string &rawRequest) {
	Request req;
	std::istringstream iss(rawRequest);
	std::string line;
	/* GET /index.html HTTP/1.1 */
	if (std::getline(iss, line)) {
		if (!line.empty() && line.back() == '\r')
			line.pop_back(); //remove the final \r
		std::istringstream startLine(line);
		startLine >> req.method >> req.uri >> req.version;
	}
	/* headers */
	while (std::getline(iss, line) && line != "\r") {
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		size_t position = line.find(": ");
		if (position != std::string::npos) {
			std::string key = line.substr(0, position);
			std::string value = line.substr(position + 2);
			req.headers[key] = value;
		}
	}
	/* body */
	std::string body;
	while (std::getline(iss, line)) {
		body += line + "\n";
	}
	if (!body.empty() && body.back() == '\n')
		body.pop_back();
	req.body = body;

	return req;
}