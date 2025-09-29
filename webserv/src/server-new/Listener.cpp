/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:51:50 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:57:22 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Listener.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/PollLoop.hpp"

Listener::Listener(void)
{
}

Listener::Listener(PollLoop &loop, char *hostname, char *port):
			_loop(&loop), 
			_hostname(hostname),
			_port(port),
			_listenerFd(-1)
{
	int	status, flags;
	int	yes = 1;
	struct addrinfo	hints, *ai, *p;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(_hostname, _port, &hints, &ai)) != 0)
		throwGaiError("server: getaddrinfo: ", status);
	for (p = ai; p != NULL; p = p->ai_next)
	{
		_listenerFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (_listenerFd < 0)
			continue ;

		setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		
		flags = fcntl(_listenerFd, F_GETFL, 0);
		if (fcntl(_listenerFd, F_SETFL, flags | O_NONBLOCK) < 0)	// NONBLOCK
			throwErrno("server: fcntl: ");
	
		if (bind(_listenerFd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(_listenerFd);
			continue ;
		}
		break ;
	}
	freeaddrinfo(ai);
	if (p == NULL)
		throwErrno("server: bind: ");
	
	if (listen(_listenerFd, SOMAXCONN) == -1)
		throwErrno("sever: listen: ");
	std::cout << "Server: waiting for connections ..." << std::endl;
	
	_loop->addHandler(this, POLLIN);
}

Listener::Listener(Listener const &src):
			_loop(src._loop), 
			_hostname(src._hostname),
			_port(src._port),
			_listenerFd(src._listenerFd)
{
}

Listener	&Listener::operator=(Listener const &rhs)
{
	if (this != &rhs)
	{
		_loop = rhs._loop;
		_hostname = rhs._hostname;
		_port = rhs._port;
		_listenerFd = rhs._listenerFd;
	}
	return (*this);
}

Listener::~Listener()
{
	close(_listenerFd);
}

int	Listener::getFd(void) const
{
	return (_listenerFd);
}

void	Listener::handleEvent(uint32_t events)
{
	if (events & POLLIN)	// check if POLLIN is set, in case other bits are also set (POLLHUP or PULLOUT)
	{
		sockaddr_in	clientAddr;
		socklen_t	addrlen = sizeof(clientAddr);

		int clientFd = accept(_listenerFd, (sockaddr *)&clientAddr, &addrlen);
		if (clientFd < 0)
			throwErrno("server: accept: ");
		
		int flags = fcntl(clientFd, F_GETFL, 0);
		if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0)
			throwErrno("server: listener: fcntl: ");
		
		// _connects.insert(std::make_pair(clientFd, Connection(_loop, clientFd)));
		// _loop->addHandler(&_connects[clientFd], POLLIN | POLLOUT);
		Connection *conn = new Connection(*_loop, clientFd);
		_loop->addHandler(conn, POLLIN | POLLOUT);
		
		std::cout << "New connection accepted through fd=[" << clientFd << std::endl;
	}
	if (events & (POLLERR | POLLHUP | POLLNVAL))
		std::cout << "Listener: error on listening socket" << std::endl;
}

