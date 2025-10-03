/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:34 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/30 11:47:23 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"
#include "../includes/server/Server.hpp"

int main(int ac, char *av[])
{
	try
	{

		// if (ac != 2)
		// 	throw std::length_error("Usage: ./webserv [configuration file]");
		
		if (ac != 2)
			throw std::length_error("Usage: ./webserv [.config]");
			
		// run server here
		
		Server serv; 
		Config config;

		config = configfile(av[1]);

		serv.start(config.getServers()[0].getServer_name()[0].c_str(), config.getServers()[0].getListen()[0].c_str());
		std::cout << "Server listening on port " << av[2] << std::endl;
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
