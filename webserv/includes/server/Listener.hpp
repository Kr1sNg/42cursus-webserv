/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:52:04 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/30 11:22:24 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	_LISTENER_HPP_
# define _LISTENER_HPP_

# include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "Connection.hpp"

class Server;

class Listener: public IEventHandler
{
	private:
		int		_listenerFd;	
		Server	*_server;	// non-owning pointer
	
		Listener	&operator=(Listener const &rhs);
	public:
		Listener(void);
		Listener(Server *server, const char *hostname, const char *port);
		Listener(Listener const &src);
		
		virtual ~Listener();	// to be able to be called by PollLoop

		virtual int		getFd(void) const;
		virtual void	handleEvent(uint32_t events);
		
};

#endif