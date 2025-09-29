/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:52:04 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:58:35 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	_LISTENER_HPP_
# define _LISTENER_HPP_

# include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "ILoop.hpp"
# include "Connection.hpp"

class Server;

class Listener: public IEventHandler
{
	private:
		int		_listenerFd;	
		Server	*_server;	// non-owning pointer
	
		
	public:
		Listener(void);
		Listener(Server *server, char *hostname, char *port);
		Listener(Listener const &src);
		Listener	&operator=(Listener const &rhs);
		virtual ~Listener();	// to be able to be called by PollLoop

		virtual int		getFd(void) const;
		virtual void	handleEvent(uint32_t events);
		
};

#endif