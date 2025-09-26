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

Connection::Connection(void) {}

Connection::Connection(PollLoop &loop, int cfd): _loop(&loop), _clientFd(cfd), _buffer(NULL)
{
}

Connection::Connection(Connection const &src):
			_loop(src._loop),
			_clientFd(src._clientFd),
			_buffer(src._buffer)
{
}

Connection::~Connection()
{
	close(_clientFd);
}

int		Connection::getFd(void) const
{
	return (_clientFd);	
}

void	Connection::handleEvent(uint32_t events)
{
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