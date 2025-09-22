/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   03-simple-client.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 11:14:30 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/22 09:58:08 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// a simple client connect to the server and server openning port

#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>

#define PORT "3490"

#define BUFFER_SIZE 1024

// get sockaddr: IPv4 or IPv6 (the same with server)
void	*get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return (&(((struct sockaddr_in *)sa)->sin_addr));
	return (&(((struct sockaddr_in6 *)sa)->sin6_addr));
}

int	main(void)
{
	int				sockfd, numbytes, rv;
	char			buf[BUFFER_SIZE];
	struct addrinfo	hints, *servinfo, *p;
	char			s[INET6_ADDRSTRLEN];

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// try both IPv4 IPv6 -> choose the first working socket
	hints.ai_socktype = SOCK_STREAM;	// TCP

	if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << std::endl;
		return (1);
	}

	// loop through all the results and connect to the first option of addrinfo we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
		{
			std::cerr << "client: socket: " << std::strerror(errno) << std::endl;
			continue ;
		}
		
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
		std::cout << "client: attempting connection to " << s << std::endl;
		
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			std::cerr << "client: connect: " << std::strerror(errno) << std::endl;
			close(sockfd);
			continue ;	// go to next option
		}
		break ;
	}
	if (p == NULL)
	{
		std::cerr << "client: failed to connect" << std::endl;
		return (1);
	}
	
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	std::cout << "client: connected to " << s << std::endl;
	
	freeaddrinfo(servinfo); // done with this

	if ((numbytes = recv(sockfd, buf, BUFFER_SIZE - 1, 0)) == -1)
	{
		std::cerr << "client: recv: " << std::strerror(errno) << std::endl;
		close(sockfd);
		return (1);
	}
	
	buf[numbytes] = '\0';
	std::cout << "client: received: " << buf << std::endl;
	close(sockfd);
	return (0);
}