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
	_pfds.clear();
	_handlers.clear();
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
	pfd.revents = 0;

	_pollFds.push_back(pfd);
	_handlers[fd] = handler;
}

void	PollLoop::modHandler(IEventHandler *handler, uint32_t events)
{
	int	fd = handler->getFd();
	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].fd == fd)
		{
			short ev = 0;
			if (events & POLLIN)
				ev |= POLLIN;
			if (events & POLLOUT)
				ev |= POLLOUT;
			_pollFds[i].events = ev;
			break ;
		}
	}
	// ensure map has pointer
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
	std::map<int, IEventHandler *>::iterator it = _handlers.find(fd);
	if (it != _handlers.end())
		_handlers.erase(it);
}

void	PollLoop::run()
{
	if (_pollFds.empty())
		return ;
		
	int	n = poll(&_pollFds[0], _pollFds.size(), -1);
	if (n < 0)
	{
		if (errno == EINTR)	// signal received
			return ;
		throwErrno("poll: ");
	}
	
	// copy fired events to a small list
	std::vector<std::pair<int, short> >	fired;

	for (size_t i = 0; i < _pollFds.size(); ++i)
	{
		if (_pollFds[i].revents != 0)
		{
			fired.push_back(std::make_pair(_pollFds[i].fd, _pollFds[i].revents));
			_pollFds[i].revents = 0;

		}
	}

	// dispatch
	for (size_t i = 0; i < fired.size(); ++i)
	{
		int	fd = fired[i].first;
		short	rev = fired[i].second;
		std::map<int, IEventHandler *>::iterator it = _handlers.find(fd);
		if (it != _handlers.end() && it->second)
			it->second->handleEvent((uint32_t)rev);
	}
}

