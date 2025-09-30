/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:06:40 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/30 11:55:34 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// will take from conf file which IPs and ports to listen on

#include "../../includes/server/Server.hpp"
#include "../../includes/webserv.hpp"
#include "../../includes/server/Listener.hpp"
// #include "../../includes/server/EpollLoop.hpp"
#include "../../includes/server/PollLoop.hpp"

// Server::Server(char *hostname, char *port): _hostname(hostname), _port(port)
// {
// 	_listeners = new Listener(this, _hostname, _port);
// 	_loop.addHandler(_listener, POLLIN);
// }

Server::Server(void): _listener(NULL)
{
}

Server::~Server()
{
	if (_listener)
	{
		_loop.removeHandler(_listener->getFd());
		delete _listener;
	}

	// close and delete all connections
	for (std::map<int, Connection *>::iterator it = _connects.begin(); it != _connects.end(); ++it)
	{
		int fd = it->first;
		_loop.removeHandler(fd);
		delete it->second;
	}
	_connects.clear();
}

void	Server::start(char *hostname, char *port)
{
	_listener = new Listener(this, hostname, port);
	_loop.addHandler(_listener, POLLIN);
}

void	Server::markForClose(int clientFd)
{
	if (clientFd <= 0)
		return ;
	_fdToClose.push_back(clientFd);
}

void	Server::setFdEvents(int fd, uint32_t events)
{
	std::map<int, Connection *>::iterator it = _connects.find(fd);
	if (it == _connects.end())
		return ;
	_loop.modHandler(it->second, events);
}

void	Server::run(void)	// create poll, listener, connection
{
	while (1) // or signal!!!
	{
		// perform the signle poll iteration
		_loop.run();
		
		// process removals requested during handlers
		if (!_fdToClose.empty())
		{
			for (size_t i = 0; i < _fdToClose.size(); ++i)
			{
				int fd = _fdToClose[i];
				std::map<int, Connection *>::iterator it = _connects.find(fd);
				if (it != _connects.end())
				{
					_loop.removeHandler(fd);
					delete it->second;
					_connects.erase(it);
				}
			}
			_fdToClose.clear();
		}
	}
}

void	Server::acceptNewConnection(int clientFd)
{
	if (clientFd < 0)
		return ;
	// create connection object and register with the loop
	Connection	*c = new Connection(this, clientFd);
	_connects[clientFd] = c;
	_loop.addHandler(c, POLLIN);
	std::cout << "New connection accepted through fd=[" << clientFd << "]..." << std::endl;
}