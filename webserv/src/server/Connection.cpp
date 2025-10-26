/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/26 08:09:15 by tat-nguy         ###   ########.fr       */
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
			_servConfig(conf),
			_clientFd(cfd),
			_inBuf(),
			_outBuf(),
			_events(POLLIN),
			_headerComplete(false),
			_headerEndPos(0),
			_contentLength(0)
{
	setNonBlocking(_clientFd);
	std::cout << "Connection: servConf: server_name: " << _servConfig.getServer_name()[0] << std::endl;
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

	while (true)
	{
		ssize_t n = recv(_clientFd, buf, sizeof(buf), 0);
		if (n <= 0)
		{
			if (n < 0 || (errno == EAGAIN || errno == EWOULDBLOCK))
				break ; // no more data for now (not an error)
			_server->markForClose(_clientFd);
			return;
		}
		_inBuf.append(buf, n);
		std::cout << "Size " << n << " [RECV] from fd[" << _clientFd << "]: " << buf << std::endl;
		
		if (n < (ssize_t)sizeof(buf))
			break ;
	}

	// Check if we already got headers
	// detect a complete HTTP request by "\r\n\r\n"
	if (!_headerComplete)
	{
		size_t pos = _inBuf.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			_headerComplete = true;
			_headerEndPos = pos + 4;
			// find content-length
			std::string headers = _inBuf.substr(0, pos);
			size_t contentPos = headers.find("Content-Length:");
			if (contentPos != std::string::npos)
			{
				std::istringstream iss(headers.substr(contentPos + 16));
				iss >> _contentLength;
			}
			else
				_contentLength = 0;
		}
	}

	// If headers complete and full body received
	if (_headerComplete)
	{
		if (_inBuf.size() >= _headerEndPos + _contentLength)
		{
			// full Request received
			std::string fullRequest = _inBuf.substr(0, _headerEndPos + _contentLength);
			// Parse HTTP request
			Request request(_inBuf, _servConfig);
			_request = request;
			//Build Response
			Response response(request, _servConfig);
			_response = response;
		
			_outBuf = _response.toString();
			
			_events = POLLIN | POLLOUT;
			_server->setFdEvents(_clientFd, _events);
			
			_inBuf.erase(0, _headerEndPos + _contentLength);
			_headerComplete = false;
			_contentLength = 0;
		}
	}
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
		// Done sending, 
		if (!_response.getKeepAlive())
		{
			_server->markForClose(_clientFd);
			return ;
		}
		
		// Otherwise, stop watching POLLOUT and wait for next request
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