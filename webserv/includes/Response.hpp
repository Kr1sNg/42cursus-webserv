/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 12:58:08 by cfiachet          #+#    #+#             */
/*   Updated: 2025/10/26 09:54:00 by tat-nguy         ###   ########.fr       */
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


class Response
{
	private:
		

		std::string	_version; //HTTP v1.1
		int 		_status; //200, 404, 500
		std::string	_reason; //"OK", "Not Found"
		std::map<std::string, std::string> _headers; // key/value : "Content-type/length"
		std::string	_body; //Body for the HTTP POST mode.

		Serverconfig	_servConfig;
		// bool _needChunked;
		bool _keepAlive;
		// std::vector<std::string> _cookie;


	public:
		Response(Request const &req, Serverconfig const &conf);
		Response();
		~Response();
		Response(const Response &cpy);
		Response	&operator=(const Response &other);
		// Response(const Request &req);

		/*setter*/
		void setVersion(const std::string &version);
		void setStatus(int code, const std::string &reason);
		void setHeader(const std::string &key, const std::string &val);
		void setBody(const std::string &body);
		
		void setKeepAlive(bool keep);
		
		// void MoreCookie(const std::string &cookie);
		// void setChunked(bool needchunk);

		/*getter*/
		const std::string &getVersion() const;
		int	getStatus() const;
		const std::string &getReason() const;
		const std::map<std::string, std::string> &getHeaders() const;
		const std::string &getBody() const;
		
		bool getKeepAlive() const;
		// const std::vector<std::string> EatCookie() const;
		// bool getChunked() const;

		// static Response buildError(int code, const std::string &reason);
		// static Response buildRedirect(const std::string &location, int code);
		// std::string getString() const;
		// static std::string convertFileToString(const std::string &path);
		// std::string	BuildFromRequest(const Request &req, const Serverconfig& serverconfig);

		//new add
		std::string toString(void) const;
		
		void buildFromFile(std::string const &path);
		void buildError(int code, std::string const &reason);

		std::string getType(std::string const &path);
		
		void    handleFileUpload(const Request &req);
};

#endif