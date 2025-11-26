/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 12:58:08 by cfiachet          #+#    #+#             */
/*   Updated: 2025/11/26 18:00:42 by tat-nguy         ###   ########.fr       */
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
# include "../config/generalconfig.hpp"
# include "../webserv.hpp"

class Request;

class Response
{
	private:
		
		std::string	_version; //HTTP v1.1©c
		int 		_status; //200, 404, 500
		std::string	_reason; //"OK", "Not Found"
		
		std::map<std::string, std::string> _headers; // key/value : "Content-type/length"
		bool _keepAlive;
		std::vector<std::string> _cookies; //to store "Set-Cookie" value

		std::string	_generatedBody; //for small, in-memory bodies
		std::string	_filePath;	// path to the file to be streamed
		bool		_isBodyFromFile;
		
		std::string	getType(const std::string &path);

	public:
		Response();
		~Response();
		Response(const Response &cpy);
		Response	&operator=(const Response &other);

		/*setter*/
		void setStatus(int code, const std::string &reason);
		void setHeader(const std::string &key, const std::string &val);
		void setKeepAlive(bool keep);
		void addCookie(const std::string &cookie);
		
		void setBody(const std::string &body);

		void    buildFromFile(std::string const &path, Serverconfig conf);
		void    buildError(int code, std::string const &reason, Serverconfig conf);
		
		bool    buildCGI(const std::string& content);
		/*getter*/
		int	getStatus(void) const;
		const std::string &getReason(void) const;
		bool	getKeepAlive(void) const;
		bool	isBodyFromFile(void) const;
		const std::string	&getFilePath(void) const;
		const std::string	&getBody(void) const;
		static std::string	intToStr(int n);
		std::string getHeaderString(void) const;
		
};

#endif