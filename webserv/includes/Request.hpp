/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:45:07 by cfiachet          #+#    #+#             */
/*   Updated: 2025/09/26 16:46:10 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <cctype>

/* std::map<string, string> variable ; explanation.
** it tooks 2 strings, the result will be something like that
** headers["Authorization"] = "token"; access in write and read
** is available with operator [""]; : std::cout << header["content"];
** The operator [""] search if it exists, else it doesn't, it create it.
*/


class Request {
	private:
		std::string _method; //GET, POST, DELETE
		std::string _uri; //ressources path
		std::string _version; //ex : (HTTP/1.1)
		std::map<std::string, std::string> _headers; //check explanation
		std::string _body; // body of the http request POST

	public:
		Request();
		~Request();
		Request(const Request &cpy);
		Request &operator=(const Request &other);

	/*getter*/
	const std::string &getMethod() const;
	const std::string &getUri() const;
	const std::string &getVersion() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getBody() const;

	/*setter*/
	void setMethod(const std::string &method);
	void setUri(const std::string &uri);
	void setVersion(const std::string &version);
	void setHeaders(const std::map<std::string, std::string> &headers);
	void setBody(const std::string &body);

	Request	parserForRequest(const std::string &ogRequest);
	void addHeader(const std::string &key, const std::string &val);
};


#endif