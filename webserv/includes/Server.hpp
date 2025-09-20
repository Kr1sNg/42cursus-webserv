/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:01:19 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/20 21:30:36 by tat-nguy         ###   ########.fr       */
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

};

#endif