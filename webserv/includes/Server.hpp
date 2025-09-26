/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:01:19 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 13:28:34 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _SERVER_HPP_
# define _SERVER_HPP_

# include "webserv.hpp"

class Server
{
	private:
		char	*_port;		// port 
		int		_sockfd;
		
	
		Server(void);
		Server(Server const &src);
		Server	&operator=(Server const &rhs);
		
		static bool	_signal;
		
	public:
		Server(char *av);
		~Server();

		void	run(void);	//start the Server
		static void	sigchild_handler(int s);
		static void signal_handler(int s);
		void	Server::HandleClient(int fd);

};

#endif