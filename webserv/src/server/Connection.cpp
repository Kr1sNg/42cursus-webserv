/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/30 13:45:34 by tat-nguy         ###   ########.fr       */
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
			_hasActiveRequest(false)
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

bool	Connection::recvIntoBuffer(void)
{
	char buf[4096];

	ssize_t n = recv(_clientFd, buf, sizeof(buf) - 1, 0);
	if (n > 0)
	{
		buf[n] = '\0';
		_inBuf += buf;
	}
	else 
		return false;
	std::cout << "[RECV] from fd[" << _clientFd << "]: " << buf << std::endl;
	return true;
}

bool	Connection::flushOutBuffer(void)
{
	while (!_outBuf.empty())
	{
		ssize_t n = send(_clientFd, _outBuf.data(), _outBuf.size(), MSG_NOSIGNAL);
		if (n > 0)
		{
			std::cout << "[SEND] server sent to client: " << _outBuf << std::endl;
			_outBuf.erase(0, n);
		}
		else
		{
			std::cout << "[SEND] can't sent to client _outBuf: " << _outBuf << std::endl;
			break ;
		}
	}
	return _outBuf.empty();
}

void	Connection::queueResponse(std::string const &s)
{
	_outBuf += s;
}

void	Connection::handleEvent(uint32_t events)
{
	if (events & POLLERR || events & POLLHUP || events & POLLNVAL)
	{
		_server->markForClose(_clientFd);
		std::cout << "Connection: socket closed fd[" << _clientFd << "]." << std::endl;
		return ;
	}

	if (events & POLLIN)
	{
		if (!recvIntoBuffer())
		{
			queueResponse("sent to client");
			_server->markForClose(_clientFd);
			return ;
		}
	}
	
	if (events & POLLOUT)
	{
		if (!flushOutBuffer())
		{
			std::cout << "_outBuf is send! " << std::endl;
			// stop watching POLLOUT; let server set only POLLIN
			_server->setFdEvents(_clientFd, POLLIN);
			return ;
		}
	}
}