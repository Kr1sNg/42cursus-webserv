/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:34 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/06 19:54:32 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server/Server.hpp"




int main(int ac, char *av[])
{
	try
	{
		if (ac != 2)
			throw std::length_error("Usage: ./webserv [configuration file]");
		
		Config config = configfile(av[1]);
		Server serv(config);

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
