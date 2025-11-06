#ifndef CGIHANDLER_H
# define CGIHANDLER_H

#include "../webserv.hpp"
# include "IEventHandler.hpp"
# include "PollLoop.hpp"

class CgiHandler : public IEventHandler
{
	private:
		int _fd;
		pid_t _pid;
		std::string _output;
	public:
		CgiHandler(int fd, pid_t pid) : _fd(fd), _pid(pid)
		{

		}
		virtual int getFd() const
		{
			return _fd;
		}

		const std::string& getOutput() const
		{ 
			return _output; 
		}

		virtual void handleEvent(uint32_t revents)
		{
			if (revents & POLLIN)
			{
				char buffer[4096];
				ssize_t n = read(_fd, buffer, sizeof(buffer));
				if (n > 0)
				{
					_output.append(buffer, n);
				} 
				else
				{
					// EOF : fermeture du fd et kill si nécessaire
					close(_fd);
					int status;
					waitpid(_pid, &status, 0);
				}
			}
		}
};

#endif