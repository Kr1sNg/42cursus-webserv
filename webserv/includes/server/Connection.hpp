/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 10:22:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 15:58:18 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _CONNECTION_HPP_
# define _CONNECTION_HPP_

# include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "ILoop.hpp"
# include "PollLoop.hpp"

class Connection: public IEventHandler
{
	private:
		PollLoop	*_loop;		// take from Server
		int			_clientFd;	// from Listener
		std::string	_buffer;
		
		Connection	&operator=(Connection const &rhs);
	
	public:
		Connection(void);
		Connection(PollLoop &loop, int cfd);
		Connection(Connection const &src);
		~Connection();

		int		getFd(void) const;
		void	handleEvent(uint32_t events);
};

#endif