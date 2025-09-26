/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:01:19 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 16:24:05 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_HPP_
# define _SERVER_HPP_

# include "../webserv.hpp"
# include "Listener.hpp"
# include "ILoop.hpp"
# include "Connection.hpp"

class Server
{
	private:
		char	*_hostname;	//from file.conf
		char	*_port;		//from file.conf
	
		PollLoop					_loop;
		// std::map<int, Connection>	_connects;
		Listener					_listeners;

		Server(void);
		Server(Server const &src);
		Server	&operator=(Server const &rhs);
		
	public:
		Server(char *hostname, char *port);
		~Server();

		void	run(void);	//start the Server
		void	addListener(Listener &l);
	
};

#endif