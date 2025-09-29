/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollLoop.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 20:33:01 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:58:40 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _POLLLOOP_HPP_
# define _POLLLOOP_HPP_

# include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "ILoop.hpp"

class PollLoop
{
	private:
		std::vector<pollfd>				_pollFds;
		std::map<int, IEventHandler * >	_handlers;
		
	public:
		PollLoop(void);
		PollLoop(PollLoop const &src);
		PollLoop	&operator=(PollLoop const &rhs);
		~PollLoop();

		// add handler and start monitoring events (POLLIN | POLLOUT flags)
		void	addHandler(IEventHandler *handler, uint32_t events);

		// change interest mask for an fd already registered
		void	modHandler(IEventHandler *handler, uint32_t events);

		// remove handler from poll and handlers map
		void	removeHandler(int fd);

		// execute ONE poll() iteration and dispatch events to handlers
		// this method returns after dispatch so caller (Server) can process removals safely
		// single iteration, call repeatedly from Server::sun()
		void	run();
		
};

#endif