/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:52:04 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/25 11:04:16 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	_LISTENER_HPP_
# define _LISTENER_HPP_

# include "webserv.hpp"

class	Listener
{
	private:
		int		_listenerFd;		// listener fd
		char	*_hostname;	//e.g. localhost // 127.0.0.1
		char	*_port;

		Listener(void);
		Listener(Listener const &src);
		Listener	&operator=(Listener const &rhs);
	
	public:
		Listener(char *hostname, char *port);
		~Listener();

		int	getListenerFd(void) const;
		int	acceptConnection(void);
};

#endif