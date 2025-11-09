/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tat-nguy <tat-nguy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/20 13:06:40 by tat-nguy          #+#    #+#             */
/*   Updated: 2025/11/09 13:21:09 by tat-nguy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// will take from conf file which IPs and ports to listen on

#include "../../includes/server/Server.hpp"
#include "../../includes/webserv.hpp"
#include "../../includes/server/Listener.hpp"
#include "../../includes/server/PollLoop.hpp"
#include "../../includes/config/Config.hpp"


Server::Server(Config const &config): _config(config)
{
	size_t noofservers = _config.getServersSize();
	
	for (size_t i = 0; i < noofservers; ++i)
	{
		const Serverconfig &servconf = _config.getServerConfig(i);	//get ServerConfig information with indicator(i)
		size_t nooflistens = servconf.getListenSize();
		for (size_t j = 0; j < nooflistens; ++j)
		{
			Listener *l = new Listener(this, servconf.getListen(j).first.c_str(), servconf.getListen(j).second.c_str(), servconf);
			_loop.addHandler(l, POLLIN);
			_listeners.push_back(l);
		}
	}
}

Server::~Server()
{
	quit();
}

void Server::quit(void)
{
	for (size_t i = 0; i < _listeners.size(); ++i)
	{
		_loop.removeHandler(_listeners[i]->getFd());
		delete _listeners[i];
	}

	// close and delete all connections
	for (std::map<int, Connection *>::iterator it = _connects.begin(); it != _connects.end(); ++it)
	{
		int fd = it->first;
		_loop.removeHandler(fd);
		delete it->second;
	}
	_connects.clear();
	std::cout << "Server is successfully shutdown!" << std::endl; 
}

void	Server::markForClose(int clientFd)
{
	if (clientFd <= 0)
		return ;
	_fdToClose.push_back(clientFd);
}

void	Server::setFdEvents(int fd, uint32_t events)
{
	std::map<int, Connection *>::iterator it = _connects.find(fd);
	if (it == _connects.end())
		return ;
	_loop.modHandler(it->second, events);
}

void	Server::run(void)	// create poll, listener, connection
{
	while (!g_stop) // or signal!!!
	{
		// perform the signle poll iteration
		_loop.run();
		
		// process removals requested during handlers
		if (!_fdToClose.empty())
		{
			for (size_t i = 0; i < _fdToClose.size(); ++i)
			{
				int fd = _fdToClose[i];
				std::map<int, Connection *>::iterator it = _connects.find(fd);
				if (it != _connects.end())
				{
					_loop.removeHandler(fd);
					delete it->second;
					_connects.erase(it);
				}
				// std::cout << "Connection: socket closed fd[" << fd << "]." << std::endl;
			}
			_fdToClose.clear();
		}

		if (!_cgiActive.empty())
		{
			for (size_t i = 0; i < _cgiActive.size(); i++)
			{
				// 1. Check timeout
				if (std::time(NULL) - _cgiActive[i].start_time > CGI_TIMEOUT_SECONDS)
				{
					kill(_cgiActive[i].pid, SIGKILL);
					_connects[_cgiActive[i].fd]->getCGIError() = "Timeout";
					_cgiActive.clear();
					// _connects[_cgiActive[i].fd]->generateErrorResponse(504, "Gateway Timeout");
					break ;
				}
				
				// // 2. Check if it finished
				// int status;
				// pid_t finished_pid = waitpid(_cgiActive[i].pid, &status, WNOHANG);

				// // process just finish
				// if (finished_pid == _cgiActive[i].pid)
				// {
				// 	// handle_cgi_exit(); // => je dois faire ou tu a deja ?
				// }
				// if (finished_pid == 0) // process still running
				// 	continue ;
				// if (finished_pid == -1) // there was en error
				// 	_connects[_cgiActive[i].fd]->generateErrorResponse(500, "Internal Server Error");
			}
		}
	}
}

void	Server::acceptNewConnection(int clientFd, const Serverconfig &conf)
{
	if (clientFd < 0)
		return ;
	// create connection object and register with the loop
	Connection	*c = new Connection(this, clientFd, conf);
	
	_connects[clientFd] = c;
	_loop.addHandler(c, POLLIN);
	// std::cout << "New connection accepted through fd=[" << clientFd << "]..." << std::endl;
}

void	Server::setSessionCount(std::string id)
{
	_sessions[id] = 1;
}

void	Server::increaseSessionCount(std::string id)
{
	std::map<std::string, int>::iterator it = _sessions.begin();
	
	for (; it != _sessions.end(); ++it)
	{
		if (it->first == id)
			it->second += 1;
	}
}

int		Server::getSessionCount(std::string id)
{
	std::map<std::string, int>::iterator it = _sessions.find(id);
	if (it != _sessions.end())
		return it->second;
	return (0);
}

PollLoop&	Server::getLoop(void)
{
	return (_loop);
}

void	Server::addCgiActive(CgiJob job)
{
	_cgiActive.push_back(job);
}

/*
void	Server::handle_cgi_exit(CgiJob &job, int status)
{
    
    if (WIFEXITED(status)) {
        // Case 1: The script exited normally.
        int exit_code = WEXITSTATUS(status);
        
        if (exit_code == 0) {
            // SUCCESS!
            // The script ran and finished without error.
            // You're done. Send 200 OK (or whatever CGI header said).
        } else {
            // SCRIPT ERROR!
            // The script (py, bash, etc.) failed and returned a non-zero code.
            // This is a 500 Internal Server Error.
            send_http_response(job.client_fd, 500);
        }

    } else if (WIFSIGNALED(status)) {
        // Case 2: The script was killed by a signal.
        int signal_number = WTERMSIG(status);
        
        // This could be from a crash (like SIGSEGV)
        // or from *our own timeout* (SIGKILL).
        
        // This is also a 500 Internal Server Error
        // (unless it was our timeout, which we already handled).
        send_http_response(job.client_fd, 500);
    }
    
    // ... clean up the job, close pipes ...
}
*/