/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   01-connect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 18:12:47 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/19 19:34:56 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// example: test a simple TCP connection to "www.example.com", port "80" or "443" HTTPS

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cerrno>	// errno
#include <cstring>	// strerror
#include <unistd.h>	// close

int	main(void)
{
	struct addrinfo	hints, *res, *p;
	int	status, sockfd;

	// prepare hints
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// we don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP
	
	// resolve host and port
	if ((status = getaddrinfo("www.example.com", "443", &hints, &res)) != 0)
	{
		// using gai_strerror for getaddrinfo
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return (1);
	}
	
	// traverse the linked list res until find a matching address
	for (p = res; p != NULL; p = p->ai_next)
	{
		// create socket
		if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		{
			// using strerror(errno) to have error reason
			std::cerr << "socket: " << std::strerror(errno) << std::endl;
			continue ; // try next address
		}

		// connect 
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
		{
			std::cerr << "connect: " << std::strerror(errno) << std::endl;
			close(sockfd);
			continue ; // try next address
		}

		break; // success
	}
	
	if (p == NULL)
	{
		std::cerr << "Failed to connect" << std::endl;
		freeaddrinfo(res);
		return (-42);
	}
	
	std::cout << "Connected successfully!" << std::endl;
	
	// Send HTTP GET request to test
	char const *msg = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";
	send(sockfd, msg, std::strlen(msg), 0);
	
	// receive response
	char	buffer[1024];
	int	bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
	if (bytes > 0)
	{
		buffer[bytes] = '\0';
		std::cout << "Received: " << buffer << std::endl;
	}
	
	// cleanup
	freeaddrinfo(res);
	close(sockfd);
	return (0);
}