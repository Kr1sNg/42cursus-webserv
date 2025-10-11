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
# include <iostream>

class Response {
	private:
		std::string	_version; //HTTP v1.1
		int 		_status; //200, 404, 500
		std::string	_reason; //"OK", "Not Found"
		std::map<std::string, std::string> _headers; // key/value : "Content-type/length"
		std::string	_body; //Body for the HTTP POST mode.

		// bool _isChunked;
		// bool _keepAlive;

		Response(const Response &cpy);
		Response	&operator=(const Response &other);

	public:
		Response();
		~Response();

	/*setter*/
	void setVersion(const std::string &version);
	void setCode(int code);
	void setReason(const std::string &reason);
	void setHeader(const std::string &key, const std::string &val);
	void setBody(const std::string &body);

	/*getter*/
	const std::string &getVersion() const;
	int	getCode() const;
	const std::string &getReason() const;
	const std::map<std::string, std::string> &getHeaders() const;
	const std::string &getBody() const;



	std::string getString() const;
};

#endif