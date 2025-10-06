/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:34 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/06 19:09:28 by tat-nguy         ###   ########.fr       */
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
		
		size_t number_of_servers = config.getServersSize();
		for (size_t i = 0; i < number_of_servers; ++i)
		{
			Serverconfig servConf = config.getServerConfig(i);
			std::cout << "Server: listen: first: [" << servConf.getListen(i).first << "], second: [" << servConf.getListen(i).second << "]." << std::endl;
			std::cout << "Servername: [" << servConf.getServer_name(i) << "]." << std::endl;
			
			// Server serv(servConf);
			// serv.run();
		}
		
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
