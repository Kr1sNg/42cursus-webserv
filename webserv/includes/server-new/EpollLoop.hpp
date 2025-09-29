/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:52:15 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:58:24 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _EPOLLLOOP_HPP_
# define _EPOLLLOOP_HPP_

# include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "ILoop.hpp"

class EpollLoop: public ILoop
{
	private:
		int	_epollFd;
		std::map<int, IEventHandler * >	_handlers;
	
		EpollLoop(EpollLoop const &src);
		EpollLoop	&operator=(EpollLoop const &rhs);
		
	public:
		EpollLoop(void);
		~EpollLoop();

		void	addHandler(IEventHandler *handler, uint32_t events);
		void	removeHandler(int fd);
		void	run();
		
};

#endif