/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 14:45:38 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/11/12 16:19:36 by tat-nguy         ###   ########.fr       */
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

std::string intToStr(int n)
{
    std::ostringstream  oss;
    oss << n;
    return (oss.str());
}

std::string	createDirectoryListing(const std::string &path)
{
	DIR *dir;
    struct dirent *entry;
    std::ostringstream html;

    dir = opendir(path.c_str());
    if (!dir)
        return "<html><body><h1>Forbidden</h1></body></html>";

    html << "<html><head><title>Index of " << path
         << "</title></head><body>\n";
    html << "<h1>Index of " << path << "</h1><hr><pre>\n";

    // Optional: link to parent directory if not root
    if (path != "/" && path != ".")
        html << "<a href=\"../\">../</a>\n";

    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // Skip "." and ".."
        if (name == "." || name == "..")
            continue;

        std::string fullPath = path;
        if (fullPath[fullPath.size() - 1] != '/')
            fullPath += '/';
        fullPath += name;

        struct stat info;
        if (stat(fullPath.c_str(), &info) == 0)
        {
            if (S_ISDIR(info.st_mode))
                html << "<a href=\"" << name << "/\">" << name << "/</a>\n";
            else
                html << "<a href=\"" << name << "\">" << name << "</a>\n";
        }
    }

    closedir(dir);
    html << "</pre><hr>";
    html << "<button onclick=\"window.location.href='/'\">Back to Home</button>\r\n";
    html << "</body></html>\n";
    return html.str();
}

long	getCurrentTimeMs(void)
{
	struct timeval	tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000L + tv.tv_usec / 1000L);
}