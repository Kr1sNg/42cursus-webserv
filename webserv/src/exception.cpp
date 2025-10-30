/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 14:45:38 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/10/30 08:58:09 by tat-nguy         ###   ########.fr       */
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

int	setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return (-1);
	return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

bool fileExists(const std::string &path)
{
    struct stat  st;
    return (stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode));
}

bool	isDirectory(const std::string &path)
{
	struct stat	st;
	
	if (stat(path.c_str(), &st) != 0)
		return (false);	// can't get access (maybe doesn't exist)
	else if (S_ISDIR(st.st_mode))
		return (true);
	else
		return (false);
}

std::string	createDirectoryListing(const std::string &path)
{
	std::string dir = "Directory Listing Path: " + path; 
	return dir;
}

std::string intToStr(int n)
{
    std::ostringstream  oss;
    oss << n;
    return (oss.str());
}