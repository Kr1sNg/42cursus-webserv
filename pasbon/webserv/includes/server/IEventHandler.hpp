/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IEventHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:01:19 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/25 21:18:59 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Interface

#ifndef _IEVENTHANDLER_HPP_
# define _IEVENTHANDLER_HPP_

# include "../webserv.hpp"
# include "../config/Serverconfig.hpp"

class IEventHandler
{	
	public:

		virtual ~IEventHandler() {};
		
		virtual int		getFd(void) const = 0;
		virtual void	handleEvent(uint32_t events) = 0;
};

#endif