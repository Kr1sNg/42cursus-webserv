/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/11/02 15:10:48 by tat-nguy         ###   ########.fr       */
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

# include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "PollLoop.hpp"
# include "../Request.hpp"
# include "../Response.hpp"

class Server;

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
		std::string	_outBuf; // send

		uint32_t	_events;
	
		Request		_request;
		Response	_response;

		// bool	_headerComplete;
		// size_t	_headerEndPos;
		// size_t	_contentLength;

		ConnectionState	_connState;
		size_t			_bodyBytesReceived;
		std::ofstream	_uploadFile;
		std::string		_uploadFilePath;
		bool			_isUploading;

		std::ifstream	_fileStream;	//for streaming response bodies

		std::string		_bodyCGI;
		void			resetConnection(void);
		
		// std::map<std::string, int>	_sessions;
		
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
		
};

#endif