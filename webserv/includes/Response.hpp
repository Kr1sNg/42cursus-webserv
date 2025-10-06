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
# include <iostream>

class Response {
	private:
		std::string	_version; //HTTP v1.1
		int 		_status; //200, 404, 500
		std::string	_reason; //"OK", "Not Found"
		std::map<std::string, std::string> _headers; // key/value : "Content-type/length"
		std::string	_body; //Body for the HTTP POST mode.

		Response(const Response &cpy);
		Response	&operator=(const Response &other);

	public:
		Response();
		~Response();
		Response(const Response &cpy);
		Response	&operator=(const Response &other);

	// std::string toString() const;
	/*getters*/
	std::string	getVersion() const;
	int			getStatus() const;
	std::string	getReason() const;
	std::map<std::string, std::string> getHeaders() const;
	std::string	getBody() const;

	/*setters*/
	void	setVersion(const std::string &version);
	void	setStatus(const int &statut);
	void	setReason(const std::string &reason);
	void	setHeaders(const std::map<std::string, std::string> &headers);
	void	setBody(const std::string &body);
	
};

#endif