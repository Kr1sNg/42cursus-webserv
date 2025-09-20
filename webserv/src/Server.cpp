/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:06:40 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/20 21:45:46 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// will take from conf file which IPs and ports to listen on

#include "../includes/Server.hpp"
#include "../includes/webserv.hpp"

bool	Server::_signal = false;

Server::Server(char *av): _port(av), _sockfd(-1)
{
}

Server::~Server()
{
	close(_sockfd);
}

// reaping zombie processes
void	Server::sigchild_handler(int s)
{
	(void)s;
	int	saved_errno = errno;
	while (waitpid(-1, NULL, WNOHANG) >0)
	{	
	}
	errno = saved_errno;
}

void	Server::signal_handler(int s)
{
	(void)s;
	std::cout << "Signal Received !" << std::endl;
	Server::_signal = true;
}

void	Server::run(void)
{
	int					newfd, status;
	struct addrinfo		hints, *servinfo, *p;
	struct sockaddr_in	client_addr;
	socklen_t			sin_size;
	int					yes = 1;
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, _port, &hints, &servinfo)) != 0)
		throwGaiError("getaddrinfo", status);
	
	// loop through all results and bind to the first option we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "server: socket: " << std::strerror(errno) << std::endl;
			continue ; // go to the next node
		}
		if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		{
			freeaddrinfo(servinfo);
			throwErrno("setsockopt");	// prog exit here
			//close(sockfd); // -> in deconstructor
		}
		if (bind(_sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			std::cerr << "server: bind: " << std::strerror(errno) << std::endl;
			close(_sockfd);
			continue ; // go to the next node
		}
		break ;
	}
	freeaddrinfo(servinfo);
	
	if (p == NULL)
		throw std::runtime_error("server: failed to bind");
	
	if (listen(_sockfd, SOMAXCONN) == -1)
		throwErrno("listen");
	
	if (signal(SIGCHLD, Server::sigchild_handler) == SIG_ERR
		|| signal(SIGINT, Server::signal_handler) == SIG_ERR
		|| signal(SIGQUIT, Server::signal_handler) == SIG_ERR)
		throwErrno("signal");
		
	std::cout << "server: waiting for connection ..." << std::endl;

	while (_signal == false)
	{
		sin_size = sizeof(client_addr);
		if ((newfd = accept(_sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
		{
			std::cerr << "server: accept:" << std::strerror(errno) << std::endl;
			continue ;
		}
		
		std::cout << "server: got a connection" << std::endl;
		
		if (fork() == 0)	//child process
		{
			close(_sockfd);
			if (send(newfd, "Hello from Server!\n", 20, 0) == -1)
				std::cerr << "server: send:" << std::strerror(errno) << std::endl;
			close(newfd);
			return ;
		}
		close(newfd); // parent
	}
	close(_sockfd);
}