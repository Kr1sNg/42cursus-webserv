/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:51:50 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/25 11:21:34 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/webserv.hpp"
#include "../../includes/Listener.hpp"

Listener::Listener(char *hostname, char *port):
			_listenerFd(-1), _hostname(hostname), _port(port)
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
}

Listener::~Listener()
{
	if (_listenerFd > 0)
		close(_listenerFd);
}

int	Listener::getListenerFd(void) const
{
	return (_listenerFd);
}

int	Listener::acceptConnection(void)
{
	
}