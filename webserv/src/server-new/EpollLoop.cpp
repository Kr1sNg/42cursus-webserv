/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:51:19 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:57:06 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/server/EpollLoop.hpp"

EpollLoop::EpollLoop()
{
	_epollFd = epoll_create(1);
	if (_epollFd < 1)
		throwErrno("epoll: ");
}
EpollLoop::~EpollLoop()
{
	close(_epollFd);
};

void	EpollLoop::addHandler(IEventHandler *handler, uint32_t events)
{
	struct epoll_event	ev;

	ev.events = events;
	ev.data.fd = handler->getFd();

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, handler->getFd(), &ev) < 0)
		throwErrno("epoll: ");
	
	_handlers[handler->getFd()] = handler;	//map
}

void	EpollLoop::removeHandler(int fd)
{
	if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throwErrno("epoll: del: ");
	
	_handlers.erase(fd);
}

void	EpollLoop::run()
{
	int const	MAX_EVENTS = 64;
	struct epoll_event	events[MAX_EVENTS];

	while (true)
	{
		int	n = epoll_wait(_epollFd, events, MAX_EVENTS, -1);
		if (n < 0)
		{
			if (errno == EINTR)
				continue ;
			throwErrno("epoll: wait: ");
			break ;
		}
		
		for (int i = 0; i < n; ++i)
		{
			int fd = events[i].data.fd;
			if (_handlers.count(fd))
				_handlers[fd]->handleEvent(events[i].events);
		}
	}
}
