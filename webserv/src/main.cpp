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
		
		size_t number_of_servers = config.getServersSize();
		std::cout << "numberofservers: " << number_of_servers << std::endl;

		// for (size_t i = 0; i < number_of_servers; ++i)
		// {
		// 	Serverconfig servConf = config.getServerConfig(i);
			
		// 	// size_t number_of_listen = servConf.getListenSize();
		// 	// std::cout << "numberoflisten: " << number_of_listen << std::endl;
			
		// 	// for (size_t j = 0; j < number_of_listen; ++ j)
		// 	// 	std::cout << "Server: listen: first: [" << servConf.getListen(j).first << "], second: [" << servConf.getListen(j).second << "]." << std::endl;
		// 	// std::cout << "Servername: [" << servConf.getServer_name() << "]." << std::endl;
			
		// 	Server serv(servConf);
		// 	serv.run();
		// }
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
