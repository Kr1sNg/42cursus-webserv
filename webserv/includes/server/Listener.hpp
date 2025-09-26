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

class Listener: public IEventHandler
{
	private:
		PollLoop	*_loop;				// poll or epoll from class Server
		char	*_hostname;			// e.g. localhost // 127.0.0.1
		char	*_port;				// e.g. 8080
		int		_listenerFd;	
	
		
	public:
		Listener(void);
		Listener(PollLoop &loop, char *hostname, char *port);
		Listener(Listener const &src);
		Listener	&operator=(Listener const &rhs);
		~Listener();

		int		getFd(void) const;
		void	handleEvent(uint32_t events);
		
};

#endif