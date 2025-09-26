/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:06:40 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:59:34 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// will take from conf file which IPs and ports to listen on

#include "../../includes/server/Server.hpp"
#include "../../includes/webserv.hpp"
#include "../../includes/server/Listener.hpp"
// #include "../../includes/server/EpollLoop.hpp"
#include "../../includes/server/PollLoop.hpp"

Server::Server(char *hostname, char *port): _hostname(hostname), _port(port)
{
	_loop = PollLoop();
	_listeners = Listener(_loop, _hostname, _port);
}

Server::~Server()
{
}

void	Server::run(void)	// create poll, listener, connection
{
	_loop.run();
	
	
	
	// Listener	listener(_hostname, _port);
	
	// int	socket;
	// if ((socket = listener.getListenerFd()) == -1)
	// 	throw std::runtime_error("server: error getting listener socket");
	
	// add_to_poll(socket);
	// std::cout << "Server: waiting for connections ..." << std::endl;
	
	// while (true) //need to change to signal
	// {
	// 	int	poll_count = poll(&_pfds[0], _pfds.size(), -1);
	// 	if (poll_count < 0)
	// 		throw std::runtime_error("server: poll: error");

	// 	for (int i = 0; i < _pfds.size(); ++i) // check if there's fd is ready to read
	// 	{
	// 		if (_pfds[i].revents != 0)
	// 		{
	// 			if (_pfds[i].fd == socket)	// new connection
	// 			{
	// 				add_to_poll(socket);
	// 				std::cout << "new connection on socket [" << socket << "]" << std::endl;
	// 			}
	// 			else
	// 				Connection connect(socket, _pfds[i].revents);
	// 		}
	// 	}
			
	// }

}