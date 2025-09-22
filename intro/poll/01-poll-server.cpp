/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01-poll-server.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 13:17:15 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/22 19:56:21 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// A server allows multiple connections

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <cerrno>
#include <fcntl.h>
#include <vector>

#define PORT "80"

// convert socket to IP address
char const *inet_ntop2(void *addr, char *buf, size_t size)
{
	struct sockaddr_storage	*sas = static_cast<sockaddr_storage *>(addr);
	struct sockaddr_in *sa4;
	struct sockaddr_in6	*sa6;
	void	*src;
	
	switch (sas->ss_family)
	{
		case AF_INET:
			sa4 = static_cast<sockaddr_in *>(addr);
			src = &(sa4->sin_addr);
			break ;
		case AF_INET6:
			sa6 = static_cast<sockaddr_in6 *>(addr);
			src = &(sa6->sin6_addr);
			break ;
		default:
			return NULL;
	}
	return (inet_ntop(sas->ss_family, src, buf, size));
}

// return a listening socket
int	get_listener_socket(char *port)
{
	int	listener, status, flags;
	int	yes = 1;
	
	struct addrinfo	hints, *ai, *p;
	
	// get a socket and bind it
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;	//server
	
	if ((status = getaddrinfo(NULL, port, &hints, &ai)) != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return (-1);
	}
	
	for (p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue ;
		
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
		
		// set server to NONBLOCK mode
		flags = fcntl(listener, F_GETFL, 0);
		if (fcntl(listener, F_SETFL, flags | O_NONBLOCK) <0)
		{
			std::cerr << "fcntl: " << strerror(errno) << std::endl;
			close(listener);
			return (-1);
		}
		
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			continue ;
		}
		break ;
	}
	
	if (p == NULL)
	{
		std::cerr << "bind: " << strerror(errno) << std::endl;
		close(listener);
		return (-1);
	}

	if (listen(listener, SOMAXCONN) == -1)
	{
		std::cerr << "server: listen:" << std::strerror(errno) << std::endl;
		close(listener);
		return (-1);
	}
	return (listener);
}

// Add new fd to the _pfds // use vector for _pfds
void	add_to_pfds(std::vector<pollfd> &pfds, int newfd)
{
	struct pollfd	pfd;
	pfd.fd = newfd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	pfds.push_back(pfd);
}

// handle incoming connections
void	handle_new_connection(int listener, std::vector<pollfd> &pfds)
{
	struct sockaddr_storage	client_addr;	//client addr
	socklen_t	addrlen;
	int			newfd;
	char		clientIP[INET6_ADDRSTRLEN];

	addrlen = sizeof(client_addr);
	newfd = accept(listener, (struct sockaddr *)&client_addr, &addrlen);

	if (newfd == -1)
	{
		std::cerr << "error accept" << std::endl;
		return ;
	}
	else
	{
		add_to_pfds(pfds, newfd);
		std::cout << "new connection from [" << inet_ntop2(&client_addr, clientIP, sizeof(clientIP))
			<< "] on socket [" << newfd << "]." << std::endl;
	}
}

// handle regular client data and client hangups
void	handle_client_data(int listener, std::vector<pollfd> &pfds, int *index)
{
	char	buf[1024];
	int		client_fd = pfds[*index].fd;
	int		nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
	
	if (nbytes > 0)
	{
		buf[nbytes] = '\0';
		std::cout << "server: recv from socket [" << client_fd << "]: " << buf << std::endl;
	}
	else
	{
		if (nbytes < 0)
			std::cerr << "error recv" << std::endl;
		else // (nbytes == 0)	// connection closed
			std::cout << "socket [" << client_fd << "] disconnected" << std::endl;
		close(client_fd);
		pfds.erase(pfds.begin() + (*index));
	}
}

// process all existing connections
void	process_connections(int listener, std::vector<pollfd> &pfds)
{
	for (int i = 0; i < pfds.size(); ++i)
	{
		// check if there's fd ready to read
		if (pfds[i].revents & (POLLIN | POLLHUP))
		{
			if (pfds[i].fd == listener)	// if we're the listener, it's a new connection
				handle_new_connection(listener, pfds);
			else	//otherwise, we're just a regular client
				handle_client_data(listener, pfds, &i);
		}
	}
}

int	main(int ac, char *av[])
{
	if (ac != 2)
	{
		std::cerr << "Usage: " << av[0] << " [PORT]" << std::endl;
		return (1);
	}

	std::vector<pollfd> _pfds;
	int	listener = get_listener_socket(av[1]);
	
	if (listener == -1)
	{
		std::cerr << "error getting listening socket" << std::endl;
		return (1);
	}

	// add listener to the list of fd
	add_to_pfds(_pfds, listener);
	
	std::cout << "Server: waiting for connections ..." << std::endl;

	while (true)
	{
		int poll_count = poll(&_pfds[0], _pfds.size(), -1);
		if (poll_count < 0)
		{
			std::cerr << "poll error" << std::endl;
			return (1);
		}
		process_connections(listener, _pfds);
	}
	return (0);
}

