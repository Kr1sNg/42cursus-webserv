/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:17 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:56:58 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/PollLoop.hpp"
#include "../../includes/server/Server.hpp"

Connection::Connection(void) {}

Connection::Connection(Server *server, int cfd):
			_server(server),
			_clientFd(cfd),
			_inBuf(),
			_outBuf()
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

	while (1)
	{
		ssize_t n = recv(_clientFd, buf, sizeof(buf), 0);
		if (n > 0)
			_inBuf.append(buf, n);
		else if (n == 0)
			return false;
		else
			break ;
	}
	std::cout << "[RECV] from fd[_clientFd]: " << buf << std::endl;
	return true;
}

bool	Connection::flushOutBuffer(void)
{
	while (!_outBuf.empty())
	{
		ssize_t n = send(_clientFd, _outBuf.data(), _outBuf.size(), MSG_NOSIGNAL);
		if (n > 0)
			_outBuf.erase(0, n);
		else
			break ;
	}
	return _outBuf.empty();
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
		char	buf[1024];
		int		n = recv(_clientFd, buf, sizeof(buf), 0);
		
		if (n <= 0)
		{
			if (n == 0)
				std::cout << "Client disconnected: fd[" << _clientFd << "]" << std::endl;
			else
				throwErrno("connection: recv: ");
			_loop->removeHandler(_clientFd);
			delete this;
			close(_clientFd);
			return ;
		}
		_buffer.append(buf, n);
		std::cout << "Received from client fd[" << _clientFd << "]: " << _buffer << std::endl;
	}
	
	if ((events & POLLOUT) && (!_buffer.empty()))
	{
		int	n = send(_clientFd, _buffer.c_str(), _buffer.size(), MSG_NOSIGNAL);
		if (n < 0)
		{
			_loop->removeHandler(_clientFd);
			close(_clientFd);
			delete this;
			throwErrno("connection: send: ");
		}
		_buffer.erase(0, n);
	}
	
	if (events & (POLLERR | POLLHUP | POLLNVAL))
	{
		_loop->removeHandler(_clientFd);
		close(_clientFd);
		delete this;
		throwErrno("connection close due to error");
	}
}