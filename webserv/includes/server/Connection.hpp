/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/30 13:35:22 by tat-nguy         ###   ########.fr       */
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

class Connection: public IEventHandler
{
	private:
		Server	*_server; // non-owning pointer back to server for marking close/mod events	
		int		_clientFd;
		std::string	_inBuf;	// recevive
		std::string	_outBuf; // send

		// bool	_isClosed;
	
		Request		_request;
		Response	_response;
		bool		_requestReady;
		bool		_responseReady;

		Connection	&operator=(Connection const &rhs);
		Connection(Connection const &src);
	
	public:
		Connection(void);
		Connection(Server *server, int cfd);
		
		~Connection();

		int		getFd(void) const;
		void	handleEvent(uint32_t events);

		void	recvIntoBuffer(void);
		void	flushOutBuffer(void);
};

#endif