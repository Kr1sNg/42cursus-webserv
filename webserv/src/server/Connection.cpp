/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbahin <tbahin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/14 20:18:32 by tbahin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/PollLoop.hpp"
#include "../../includes/server/Server.hpp"
#include "../../includes/Request.hpp"
#include "../../includes/Response.hpp"

Connection::Connection(void) {}

Connection::Connection(Server *server, int cfd):
			_server(server),
			_clientFd(cfd),
			_inBuf(),
			_outBuf(),
			_requestReady(false),
			_responseReady(false)
{
	setNonBlocking(_clientFd);
}

Connection::~Connection()
{
	close(_clientFd);
}

int		Connection::getFd(void) const
{
	return (_clientFd);	
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
		_request = Request::parserForRequest(_inBuf);
		_requestReady = true;
		_inBuf.erase(0, pos + 4);
	}

	// if a full request is ready, build a response
	if (_requestReady)
	{
		_response = Response();
		// _response.setVersion("HTTP/1.1");
		// _response.setCode(200);
		// _response.setReason("OK");
		// _response.setHeader("Content-Type", "text/plain");
		// _response.setBody("Hello from server!\n");
		std::cout << "inside reponse" << std::endl;
		// convert response to text
		// _outBuf = _response.getString();
		_outBuf =
            "<html><body><h1>File Uploaded Successfully!</h1></body></html>";

		_responseReady = true;
		_requestReady = false;
	}
		_responseReady = true;
		_requestReady = false;
}

void	Connection::flushOutBuffer(void) //send
{
	if (_outBuf.empty())
		return ;

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
		// response fully sent!
		if (!_response.getKeepAlive())
			_server->markForClose(_clientFd);
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
		recvIntoBuffer();
	
	if (events & POLLOUT)
		flushOutBuffer();
}