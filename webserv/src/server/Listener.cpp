/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:51:50 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/30 11:54:29 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/server/Listener.hpp"
#include "../../includes/server/Connection.hpp"
#include "../../includes/server/Server.hpp"
#include "../../includes/config/Serverconfig.hpp"

static Serverconfig dummy_conf;

static int	make_listen_socket(const char *hostname, const char *port)
{
	int	status;
	int listenerFd = -1;
	int	yes = 1;
	struct addrinfo	hints, *ai, *p;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(hostname, port, &hints, &ai)) != 0)
		throwGaiError("server: getaddrinfo: ", status);
	for (p = ai; p != NULL; p = p->ai_next)
	{
		listenerFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listenerFd < 0)
			continue ;

		setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		
		if (setNonBlocking(listenerFd) < 0)	// NONBLOCK
		{
			close(listenerFd);
			continue ;
		}
	
		if (bind(listenerFd, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listenerFd);
			continue ;
		}
		break ;
	}
	freeaddrinfo(ai);
	if (p == NULL)
	{
		close(listenerFd);
		return (-1);
	}
	
	if (listen(listenerFd, SOMAXCONN) < 0)
	{
		close(listenerFd);
		return (-1);
	}
	return (listenerFd);
}

// Listener::Listener(void)
// 	: _server(NULL), _servConfig(dummy_conf),
// {
// }

Listener::Listener(Server *server, const char *hostname, const char *port, const Serverconfig &conf):
			_listenerFd(-1),
			_server(server),
			_servConfig(_servConfig)
{
	_listenerFd = make_listen_socket(hostname, port);
	if (_listenerFd < 0)
		throw std::runtime_error("Server: Failed to create listener");
	std::cout << "Server: waiting for connections ..." << std::endl;
}

Listener::Listener(Listener const &src):
			_listenerFd(src._listenerFd),
			_server(src._server),
			_servConfig(src._servConfig)
{
}

// Listener	&Listener::operator=(Listener const &rhs)
// {
// 	if (this != &rhs)
// 	{
// 		_loop = rhs._loop;
// 		_hostname = rhs._hostname;
// 		_port = rhs._port;
// 		_listenerFd = rhs._listenerFd;
// 	}
// 	return (*this);
// }

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
		while (1)
		{
			sockaddr_in	clientAddr;
			socklen_t	addrlen = sizeof(clientAddr);

			int clientFd = accept(_listenerFd, (sockaddr *)&clientAddr, &addrlen);
			if (clientFd < 0)
				break ;
		
			setNonBlocking(clientFd);
			
			//forward to server to register and own the connection
			_server->acceptNewConnection(clientFd, _servConfig);
		}
	}
	if (events & (POLLERR | POLLHUP | POLLNVAL))
		std::cerr << "Listener: error on listening socket" << std::endl;
}

