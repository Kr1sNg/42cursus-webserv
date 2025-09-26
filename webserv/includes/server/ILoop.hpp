/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ILoop.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 20:23:24 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/25 21:10:27 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// interface

#ifndef _ILOOP_HPP_
# define _ILOOP_HPP_

#include "IEventHandler.hpp"

class ILoop
{	
	public:
		virtual	~ILoop() {};
		
		virtual void	addHandler(IEventHandler *handler, uint32_t events) = 0;
		virtual void	removeHandler(int fd) = 0;
		virtual void	run() = 0;
};

#endif