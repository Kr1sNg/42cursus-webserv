/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cfiachet <cfiachet@student.42perpignan.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 13:36:38 by cfiachet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _WEBSERV_HPP_
# define _WEBSERV_HPP_

# include <iostream>
# include <string>
# include <exception>
# include <stdexcept>	// runtime_error
# include <cerrno>	// errno
# include <cstring>	// strerror
# include <unistd.h> // close
# include <sstream> // std::istringstream


// for socket
# include <netdb.h>	// addrinfo
# include <csignal>
# include <sys/wait.h> // waitpid

# include "Response.hpp"
# include "Request.hpp"


// function
void	throwErrno(std::string const &reason);
void	throwGaiError(std::string const &reason, int code);

#endif