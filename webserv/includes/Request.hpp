/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:45:07 by cfiachet          #+#    #+#             */
/*   Updated: 2025/10/22 23:11:35 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <cctype>
# include <sys/stat.h>

# include "webserv.hpp"
# include "config/generalconfig.hpp"
/* std::map<string, string> variable ; explanation.
** it tooks 2 strings, the result will be something like that
** headers["Authorization"] = "token"; access in write and read
** is available with operator [""]; : std::cout << header["content"];
** The operator [""] search if it exists, else it doesn't, it create it.
*/


class Request
{
	private:

		Serverconfig _servConfig;
				
		std::string _method; //GET, POST, DELETE
		std::string _uri; //ressources path
		std::string _version; //ex : (HTTP/1.1)
		std::map<std::string, std::string> _headers; //check explanation
		std::string _body; // body of the http request POST
		
	public:
		// Request();
		Request(const std::string &ogRequest, const Serverconfig &conf);
		~Request();
		Request(const Request &cpy);
		Request &operator=(const Request &other);

		/*getter*/
		const std::string &getMethod() const;
		const std::string &getUri() const;
		const std::string &getVersion() const;
		const std::map<std::string, std::string> &getHeaders() const;
		const std::string &getBody() const;
		const Serverconfig &getConfig() const;

		/*setter*/
		// void setMethod(const std::string &method);
		// void setUri(const std::string &uri);
		// void setVersion(const std::string &version);
		// void setHeaders(const std::map<std::string, std::string> &headers);
		// void setBody(const std::string &body);
		// void setConfig(const Serverconfig &conf);


		// static Request	parserForRequest(const std::string &ogRequest, const Serverconfig &conf);
		// void addHeader(const std::string &key, const std::string &val);
		void ContentLengthParser(void);
		std::string getHeader(const std::string &keyword) const;
		int verCheck(const std::string &version);
		int methodCheck(const std::string &met);
		void  parseFirstLine(std::string &line);//, Request &req);
		int CompareConfig(const Serverconfig &conf) const;
		
		std::string trimSpace(const std::string &str);
		void parsingForHeader(const std::string &line);

};


#endif