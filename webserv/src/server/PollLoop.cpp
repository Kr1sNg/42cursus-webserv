/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollLoop.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 21:28:48 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:57:29 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/server/PollLoop.hpp"

PollLoop::PollLoop(void)
{
}
PollLoop::~PollLoop()
{
}

PollLoop::PollLoop(PollLoop const &src): _pollFds(src._pollFds), _handlers(src._handlers)
{
}
PollLoop	&PollLoop::operator=(PollLoop const &rhs)
{
	if (this != &rhs)
	{
		_pollFds = rhs._pollFds;
		_handlers = rhs._handlers;
	}
	return (*this);
}

void	PollLoop::addHandler(IEventHandler *handler, uint32_t events)
{
	int	fd = handler->getFd();
	struct pollfd	pfd;
	
	pfd.fd = fd;
	pfd.events = 0;
	if (events & POLLIN)
		pfd.events |= POLLIN;
	if (events & POLLOUT)
		pfd.events |= POLLOUT;
	if (events & POLLHUP)
		pfd.events |= POLLHUP;
	pfd.revents = 0;

	_pollFds.push_back(pfd);
	_handlers[fd] = handler;
}

void	PollLoop::removeHandler(int fd)
{
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break ;
		}
	}
	_handlers.erase(fd);
}

void	PollLoop::run()
{
	while (true)
	{
		if (_pollFds.empty())
			break ;
		
		int	n = poll(&_pollFds[0], _pollFds.size(), -1);
		if (n < 0)
			throwErrno("poll: ");
		
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].revents != 0)
			{
				int fd = _pollFds[i].fd;
				std::map<int, IEventHandler *>::iterator it = _handlers.find(fd);
				if (it != _handlers.end())
					it->second->handleEvent(_pollFds[i].revents);
				_pollFds[i].revents = 0;
			}
		}
	}
	
}