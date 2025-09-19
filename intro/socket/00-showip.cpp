/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   showip.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/18 22:57:35 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/18 23:50:58 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int	main(int ac, char *av[])
{
	struct addrinfo hints, *res, *p;
	int				status;
	char			ipstr[INET6_ADDRSTRLEN];

	if (ac != 2)
	{
		std::cerr << "Usage: ./a.out hostname" << std::endl;
		return (1);
	}

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	// either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;	// TCP
	
	if ((status = getaddrinfo(av[1], NULL, &hints, &res)) != 0)
	// res points to first addrinfo structure
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return (1);
	}
	
	// traverse the linked list
	for (p = res; p != NULL; p = p->ai_next)
	{
		void	*addr;
		std::string			ipver;
		struct sockaddr_in	*ipv4;
		struct sockaddr_in6	*ipv6;
		
		if (p->ai_family == AF_INET)
		{
			ipv4 = (struct sockaddr_in *)p->ai_addr;	// force cast
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else
		{
			ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}
		
		// convert IP to string
		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
		std::cout << "IP address for " << av[1] << " is: " << std::endl;
		std::cout << ipver << " " << ipstr << std::endl;
	}
	freeaddrinfo(res); // free linked list

	return (0);
}