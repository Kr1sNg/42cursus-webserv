/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 14:45:38 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/20 14:47:42 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

void	throwErrno(std::string const &reason)
{
	throw std::runtime_error(reason + ": " + strerror(errno));
}

void	throwGaiError(std::string const &reason, int code)
{
	throw std::runtime_error(reason + ": " + gai_strerror(code));
}