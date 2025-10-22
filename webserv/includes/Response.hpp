/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 12:58:08 by cfiachet          #+#    #+#             */
/*   Updated: 2025/09/26 13:56:33 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <sstream>
# include <fstream>
# include <iostream>
# include <stdexcept>
# include "config/generalconfig.hpp"
# include "webserv.hpp"
// # include "Request.hpp"

class Request;


class Response {
	private:
		std::string	_version; //HTTP v1.1
		int 		_status; //200, 404, 500
		std::string	_reason; //"OK", "Not Found"
		std::map<std::string, std::string> _headers; // key/value : "Content-type/length"
		std::string	_body; //Body for the HTTP POST mode.

		bool _needChunked;
		bool _keepAlive;
		std::vector<std::string> _cookie;


	public:
		Response();
		~Response();
		Response(const Response &cpy);
		Response	&operator=(const Response &other);
		Response(const Request &req);

	/*setter*/
	void setVersion(const std::string &version);
	void setCode(int code);
	void setReason(const std::string &reason);
	void setHeader(const std::string &key, const std::string &val);
	void setBody(const std::string &body);
	void setKeepAlive(bool keep);
	void MoreCookie(const std::string &cookie);
	void setChunked(bool needchunk);

	/*getter*/
	const std::string &getVersion() const;
	int	getCode() const;
	const std::string &getReason() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getBody() const;
	bool getKeepAlive() const;
	const std::vector<std::string> EatCookie() const;
	bool getChunked() const;

	static Response buildError(int code, const std::string &reason);
	static Response buildRedirect(const std::string &location, int code);
	std::string getString() const;
	static std::string convertFileToString(const std::string &path);
	std::string	BuildFromRequest(const Request &req, const Serverconfig& serverconfig);
};

#endif