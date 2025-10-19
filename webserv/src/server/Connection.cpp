/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/15 18:14:15 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/PollLoop.hpp"
#include "../../includes/server/Server.hpp"
#include "../../includes/Request.hpp"
#include "../../includes/Response.hpp"

// Connection::Connection(void) {}

Connection::Connection(Server *server, int cfd, const Serverconfig &conf):
			_server(server),
			_clientFd(cfd),
			_servConfig(conf),
			_inBuf(),
			_outBuf(),
			_events(POLLIN),
			_requestReady(false),
			_responseReady(false)
{
	setNonBlocking(_clientFd);
	std::cout << "Connection: servConf: server_name: " << _servConf.getServer_name() << std::endl;
}

Connection::~Connection()
{
	close(_clientFd);
}

int		Connection::getFd(void) const
{
	return (_clientFd);	
}

uint32_t	Connection::getEvents(void) const
{
	return (_events);
}

void	Connection::recvIntoBuffer(void) //receive !!!!
{
	char buf[4096];

	ssize_t n = recv(_clientFd, buf, sizeof(buf), 0);
	if (n <= 0)
	{
		_server->markForClose(_clientFd);
		return;
	}
	_inBuf.append(buf, n);
	std::cout << "Size " << n << " [RECV] from fd[" << _clientFd << "]: " << buf << std::endl;

	// detect a complete HTTP request by "\r\n\r\n"
	size_t pos = _inBuf.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		// Parse HTTP request
		_request = Request::parserForRequest(_inBuf, _servConfig);
		_requestReady = true;
		_inBuf.erase(0, pos + 4);
	}

	// if a full request is ready, build a response
	if (_requestReady)
	{
		 _outBuf = _response.BuildFromRequest(_request);
		// _response.setVersion("HTTP/1.1");
		// _response.setCode(200);
		// _response.setReason("OK");
		// _response.setHeader("Content-Type", "text/plain");
		// _response.setBody("Hello from server!\n");
		// convert response to text
		// _outBuf = _response.getString();

	}
		_events = POLLIN | POLLOUT;
		_server->setFdEvents(_clientFd, _events);
		
		_responseReady = true;
		_requestReady = false;
}

void	Connection::flushOutBuffer(void) //send
{
	if (_outBuf.empty())
		return ;
	std::cout << "///////" << std::endl;
	std::cout << _outBuf << std::endl;
	std::cout << "///////" << std::endl;
	ssize_t n = send(_clientFd, _outBuf.c_str(), _outBuf.size(), 0);
	if (n < 0)
	{
		if (errno != EWOULDBLOCK && errno != EAGAIN)
			_server->markForClose(_clientFd);
		return ;
	}
	_outBuf.erase(0, n);

	if (_outBuf.empty())
	{
		// Done sending, stop watching POLLOUT
		_events = POLLIN;
		_server->setFdEvents(_clientFd, _events);
	}
}

void	Connection::handleEvent(uint32_t events)
{
	if (events & POLLERR || events & POLLHUP || events & POLLNVAL)
	{
		_server->markForClose(_clientFd);
		return ;
	}

	if (events & POLLIN)
	{
		recvIntoBuffer();
	}
	if (events & POLLOUT)
		flushOutBuffer();
}