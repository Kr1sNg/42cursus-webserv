/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   02-simple-server.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 22:28:57 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/19 23:29:00 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// a simple stream server sends "Hello, world!" out over a stream connection.

#include <iostream>
#include <cerrno>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h> // inet_ntop

#define PORT "3490"

void	sigchild_handler(int s)
{
	(void)s;
	// waitpid() might overwrite errno, so we save and restore it
	int saved_errno = errno;
	while (waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6
void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return (&(((struct sockaddr_in*)sa)->sin_addr));
	return (&(((struct sockaddr_in6*)sa)->sin6_addr));
}

int	main(void)
{
	// listen on sockfd, new connection on newfd
	int						sockfd, newfd, rv;
	struct addrinfo			hints, *servinfo, *p;
	struct sockaddr_storage	client_addr;
	socklen_t				sin_size;
	int						yes = 1;
	char					s[INET6_ADDRSTRLEN];
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;	// use my IP, so set "NULL" for node of getaddrinfo
	
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
		return (1);
	}
	
	// loop through all results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "server: socket: " << std::strerror(errno) << std::endl;
			continue ;
		}
		
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		{
			std::cerr << "setsockopt: " << std::strerror(errno) << std::endl;
			close(sockfd);
			freeaddrinfo(servinfo);
			return (1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			std::cerr << "server: bind: " << std::strerror(errno) << std::endl;
			close(sockfd);
			continue ;
		}
		break;
	}
	
	freeaddrinfo(servinfo);
	
	if (p == NULL)
	{
		std::cerr << "server: failed to bind" << std::endl;
		close(sockfd);
		return (1);
	}

	if (listen(sockfd, SOMAXCONN) == -1)
	{
		std::cerr << "server: listen:" << std::strerror(errno) << std::endl;
		close(sockfd);
		return (1);
	}

	signal(SIGCHLD, sigchild_handler);	// reap all dead processes

	std::cout << "server: waiting for connections ..." << std::endl;

	// main accept() loop
	while (1)
	{
		sin_size = sizeof(client_addr);
		if ((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1)
		{
			std::cerr << "server: accept:" << std::strerror(errno) << std::endl;
			continue ;
		}

		inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), s, sizeof(s));
		std::cout << "server: got connection from " << s << std::endl;

		if (fork() == 0)
		{
			// this is child process
			close(sockfd); // child doesn't need the listener
			if (send(newfd, "Hello, World!\n", 14, 0) == -1)
			{
				std::cerr << "server: send:" << std::strerror(errno) << std::endl;
			}
			close(newfd);
			return (1);
		}
		close(newfd); // parent doesn't need this
	}
	close(sockfd);
	return (0);
}