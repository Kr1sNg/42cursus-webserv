/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 12:05:38 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/22 12:53:19 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// example where we'll wait 2.5 seconds for data to be ready to read from stdin

#include <iostream>
#include <poll.h>
#include <unistd.h>

int	main(void)
{
	struct pollfd	pfds[1];

	pfds[0].fd = 0;				// stdin
	pfds[0].events = POLLIN;	// inform me when ready to read

	std::cout << "Hit ENTER or wait 2.5 second for timeout" << std::endl;

	int	num_events = poll(pfds, 1, 2500); // 25 seconds timeout

	if (num_events == 0)
		std::cout << "Poll timed out! Nothing was input in!" << std::endl;
	else
	{
		int	pollin_happend = pfds[0].revents & POLLIN;
		if (pollin_happend)
		{
			std::cout << "File descriptor [" << pfds[0].fd << "] is ready to read..." << std::endl;
			std::string inpt;
			std::cin >> inpt;
			std::cout << "Input: " << inpt << std::endl; 
		}
		else
			std::cerr << "Unexpected event occurred: " << pfds[0].revents << std::endl;
	}
	return (0);
}
