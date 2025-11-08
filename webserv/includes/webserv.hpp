/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 17:42:05 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/11/07 16:42:45 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _WEBSERV_HPP_
# define _WEBSERV_HPP_
# define WWW "./www"
# define BUFFER 4096

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
# include <sstream> // std::istringstream
# include <cstdio> // std::remove
# include <ctime>
# include <algorithm>
# include <dirent.h>     // opendir, readdir, closedir


// for socket
# include <netdb.h>	// addrinfo
# include <csignal>
# include <sys/wait.h> // waitpid
# include <poll.h>	// epoll
# include <fcntl.h>	//fcntl
// # include <sys/epoll.h>
# include <cstdlib>

# include "config/generalconfig.hpp" 

# include "Request.hpp"
# include "Response.hpp"
# include "server/PollLoop.hpp"
extern volatile sig_atomic_t g_stop;

struct UploadedFile
{
	std::string filename;
	std::string contentType;
	std::string content;
};


// function
class PollLoop;
class Connection;

void	throwErrno(std::string const &reason);
void	throwGaiError(std::string const &reason, int code);
int		setNonBlocking(int fd);

bool fileExists(const std::string &path);
bool	isDirectory(const std::string &path);
std::string	createDirectoryListing(const std::string &path);
std::string intToStr(int n);

//void cgiHandle(Connection& parent, const Request& req, const Locationconfig& location, const std::string& body, PollLoop& _loop);

UploadedFile parseMultipartBody(const std::string &body, const std::string &contentTypeHeader);

#endif