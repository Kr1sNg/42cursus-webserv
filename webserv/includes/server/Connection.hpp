/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/15 18:07:26 by tat-nguy         ###   ########.fr       */
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
		Server			*_server; // non-owning pointer back to server for marking close/update events	
		int				_clientFd;
		
		Serverconfig	_servConf;

		std::string		_inBuf;	// recevive
		std::string		_outBuf; // send

		uint32_t	_events;

		// bool	_isClosed;
	
		Request		_request;
		Response	_response;
		bool		_requestReady;
		bool		_responseReady;

		Connection	&operator=(Connection const &rhs);
		Connection(Connection const &src);
	
	public:
		Connection(void);
		Connection(Server *server, int cfd, Serverconfig const &conf);
		
		~Connection();

		int		getFd(void) const;
		void	handleEvent(uint32_t events);

		void	recvIntoBuffer(void);
		void	flushOutBuffer(void);

		uint32_t	getEvents(void) const;
};

#endif