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

class Server;

class Connection: public IEventHandler
{
	private:
		Server	*_server; // non-owning pointer back to server for marking close/mod events	
		int		_clientFd;
		std::string	_inBuf;
		std::string	_outBuf;
	
		
		Connection	&operator=(Connection const &rhs);
		Connection(Connection const &src);
	
	public:
		Connection(void);
		Connection(Server *server, int cfd);
		
		virtual ~Connection();

		virtual int		getFd(void) const;
		virtual void	handleEvent(uint32_t events);

		bool	recvIntoBuffer();
		bool	flushOutBuffer();

};

#endif