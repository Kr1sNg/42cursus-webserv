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

/*
Server owns the PollLoop, a single Listener (pointer),
and a map of active Connection* (one per client).
The server also owns removal of connections
(to avoid deleting a Connection while its handleEvent() is still running).
*/


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
		PollLoop	_loop;
		Listener	*_listener; // owned by server
		std::map<int, Connection *> _connects;	// active connections
		std::vector<int>	_fdToClose; // fds requested to close (processed after poll dispatch)


		Server(void);
		Server(Server const &src);
		Server	&operator=(Server const &rhs);
		
	public:
		Server();
		~Server();

		// create listener and register it
		void	start(char *hostname, char *port);
		
		// mark a client fd to be removed after current dispatch iteration
		void	markForClose(int clientFd);

		// change event interest for connection fd
		void	setFdEvents(int fd, uint32_t events);

		// main loop
		void	run(void);

		void	acceptNewConnection(int clientFd);
};

#endif