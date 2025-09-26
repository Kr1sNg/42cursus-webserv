/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/09/26 14:24:34 by tat-nguy         ###   ########.fr       */
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
# include <vector>
# include <map>
# include <utility> //pair


// for socket
# include <netdb.h>	// addrinfo
# include <csignal>
# include <sys/wait.h> // waitpid
# include <poll.h>	// epoll
# include <fcntl.h>	//fcntl
// # include <sys/epoll.h>


// function
void	throwErrno(std::string const &reason);
void	throwGaiError(std::string const &reason, int code);

#endif