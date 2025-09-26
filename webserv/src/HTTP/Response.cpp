/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 13:03:16 by cfiachet          #+#    #+#             */
/*   Updated: 2025/09/26 13:40:44 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"

Response::Response()
	: version("HTTP/1.1"),
	status(200),
	reason("OK"),
	headers(),
	body() {
}

std::string Response::toString() const {
	std::ostringstream oss;

	oss << version << " " << status << " " << reason << "\r\n";
	std::map<std::string, std::string>::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
		oss << it->first << ": " << it->second << "\r\n";
	oss << "\r\n";
	oss << body;

	return oss.str();
}
