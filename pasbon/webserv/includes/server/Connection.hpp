/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbahin <tbahin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/11/02 17:46:47 by tbahin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* What a connection does:
	- parse HTTP requests from _inBuffer
	- build a Request object once a full HTTP request is received
	- create and serialize a Response object using your Response class;
	- send back the response through _outBuffer

	Socket <=> 	Connection 	<=> Request
							<=> Response

*/

#ifndef _CONNECTION_HPP_
# define _CONNECTION_HPP_

# include "PollLoop.hpp"
# include "Server.hpp"
# include "IEventHandler.hpp"
# include "../Request.hpp"
# include "../Response.hpp"
# include "CGIPipeHandler.hpp"
# include "../webserv.hpp"

class Server;
class CGIPipeHandler;

enum ConnectionState
{
	CONN_READING_HEADERS,
	CONN_HANDLING_BODY,
	CONN_GENERATING_RESPONSE,
	CONN_WRITING_RESPONSE,
};

class Connection: public IEventHandler
{
	private:
		Server	*_server; // non-owning pointer back to server for marking close/update events	
		Serverconfig _servConfig;
		
		int		_clientFd;
		// std::string	_inBuf;	// recevive
		std::string _bodyCGI;
		std::string	_outBuf; // send

		uint32_t	_events;
	
		Request		_request;
		Response	_response;

		ConnectionState	_connState;
		size_t			_bodyBytesReceived;
		std::ofstream	_uploadFile;
		std::string		_uploadFilePath;
		bool			_isUploading;

		std::ifstream	_fileStream;	//for streaming response bodies

		void			resetConnection(void);

		// for CGI pipe
		pid_t	_cgiPid;
		CGIPipeHandler	*_cgiReadHandler;
		CGIPipeHandler	*_cgiWriteHandler;
		std::string     _cgiWriteBuffer;  // Buffer to write to CGI (the request body)
    	// std::string     _cgiReadBuffer;   // _bodyCGI

		
		Connection	&operator=(Connection const &rhs);
		Connection(Connection const &src);
	
	public:
		Connection(void);
		Connection(Server *server, int cfd, const Serverconfig &conf);
	
		~Connection();

		int		getFd(void) const;
		void	handleEvent(uint32_t events);

		// handler Recv()
		void	handleReadHeaders(void);
		void	handleReadBody(void);
		

		// handler Send();
		void	handleWrite(void);

		uint32_t	getEvents(void) const;

		std::string trimSpace(const std::string &str);
		bool	checkHostName(std::string ogRequest);
		void	searchHost(const std::string &line, bool& check);
		bool	compareHost(std::string hostname);
		void	generateResponse(void);
		void	generateErrorResponse(int code, std::string const &reason);

		// handle Cookies and session Management
		std::string	sessionManagement(void);
		std::string	parseCookie(const std::string &header, const std::string &cookie);
		std::string	createSessionId(void);
		void	generateVisitCountResponse(std::string ses);
		

		//for CGI pipe
		void	handleCgi(const Locationconfig &location);
		void	cleanupCgi(void);
		void	handleCgiWrite(uint32_t events);
		void	handleCgiRead(uint32_t events);
};

#endif