/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:45:07 by cfiachet          #+#    #+#             */
/*   Updated: 2025/11/02 15:17:49 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// only parsing _inBuf to Request, we don't compare Request and Config here!
// request parser finish when \r\n\r\n (after read headers) => pass to Handler
// we will compare Request and Config in Response(request, config)
// if reason != 200 => send error code to Response directly

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

enum ParseState
{
	PARSING_REQUEST_LINE,
	PARSING_HEADERS,
	PARSING_BODY,
	PARSING_COMPLETE,
	PARSING_ERROR
};

class Request
{
	private:
		ParseState	_state;
		
		int	_statusCode;		// HTTP error code if parsing fails 400 Bad Request
		std::string	_reason;
		std::string	_buffer;	// buffer to store request data
		
		std::string _method; 	// GET, POST, DELETE
		std::string _uri; 		// ressources path
		std::string _version; 	// HTTP/1.1
		std::map<std::string, std::string> _headers; //check explanation
		// std::string _body; 		// body of the http request POST
		
		size_t	_contentLength;	// expected body length from Content-Length
		// size_t	_bodyBytesRead;	// how many body bytes we've read so far
		
		bool	_isChunked; 	//to track transfer encoding
		
	public:
		Request(void);
		// Request(const std::string &ogRequest);
		~Request();
		Request(const Request &cpy);
		Request &operator=(const Request &cpy);


		void	append(const char* buffer, size_t nread);
		/*getter*/
		ParseState	getState(void);
		int getStatusCode(void);
		const std::string &getReason(void) const;
		
		const std::string &getMethod(void) const;
		const std::string &getUri(void) const;
		const std::string &getVersion(void) const;
		const std::map<std::string, std::string> &getHeaders(void) const;

		bool    isParsingComplete(void) const;
		bool    isError(void) const;
		
		std::string &getBuffer(void);
		size_t  getContentLength(void) const;
		bool    isChunked(void) const;

		std::string getHeader(const std::string &keyword) const;
		
		size_t  findNextCRLF(void);
		bool	setError(int code, const std::string &reason);
		std::string trimSpace(const std::string &str);
		
		void	parse(void);
		bool    parsingRequestLine(void);
		bool	parsingHeaders(void);

};


#endif