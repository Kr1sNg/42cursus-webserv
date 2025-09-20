/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:34 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/20 21:45:35 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/Server.hpp"

int main(int ac, char *av[])
{
	try
	{
		if (ac != 2)
		throw std::length_error("Usage: ./webserv [configuration file]");
		
		// run server here
		Server	serv(av[1]);
		serv.run();
		
	}
	catch (std::exception const &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	catch (...)	// in case there's any unexpected error from system calls
	{
		std::cerr << "Error: Unexpected Error" << std::endl;
		return (1);
	}
	return (0);
}
