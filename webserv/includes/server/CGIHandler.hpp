#ifndef CGIHANDLER_H
# define CGIHANDLER_H

# include "PollLoop.hpp"
# include "Connection.hpp"
# include "IEventHandler.hpp"
# include "../webserv.hpp"

// class CgiHandler : public IEventHandler
// {
// 	private:
// 		int _fd;
// 		pid_t _pid;
// 		std::string _output;
// 	public:
// 		CgiHandler(int fd, pid_t pid) : _fd(fd), _pid(pid)
// 		{

// 		}
// 		virtual int getFd() const
// 		{
// 			return _fd;
// 		}

// 		const std::string& getOutput() const
// 		{ 
// 			return _output; 
// 		}

// 		virtual void handleEvent(uint32_t revents)
// 		{
// 			if (revents & POLLIN)
// 			{
// 				char buffer[4096];
// 				ssize_t n = read(_fd, buffer, sizeof(buffer));
// 				if (n > 0)
// 				{
// 					_output.append(buffer, n);
// 				} 
// 				else
// 				{
// 					// EOF : fermeture du fd et kill si nécessaire
// 					close(_fd);
// 					int status;
// 					waitpid(_pid, &status, 0);
// 				}
// 			}
// 		}
// };

class Server;
class PollLoop;

class CgiConnection : public IEventHandler
{
	private:
		Connection* _parent;
		int _fd;
		bool _isOutput;
		std::string _writeBuffer;
		size_t _writeOffset;
		pid_t _pid;
		PollLoop &_loop;
	public:
		CgiConnection( Connection* parent, int fd, bool isOutput, pid_t pid, PollLoop &loop, std::string body)
			: _parent(parent), _fd(fd), _isOutput(isOutput), _writeOffset(0),  _pid(pid), _loop(loop)
		{
			if (!_isOutput)
				_writeBuffer = body;
		}

		int getFd() const { return _fd; }

		void handleEvent(uint32_t events)
		{
			if (events & POLLIN) handleRead();
			if (events & POLLOUT) handleWrite();
		}

		void handleRead()
		{
			if (!_isOutput) return;

			char buffer[4096];
			ssize_t n = read(_fd, buffer, sizeof(buffer));
			if (n > 0)
				_parent->getCgiOutput().append(buffer, n);
			else if (n == 0)
			{
				closeConnection();
				int status;
				waitpid(_pid, &status, 0); // récupérer le processus
				_parent->onCgiComplete();
			}
		}

		void handleWrite()
		{
			if (_isOutput) return;

			if (_writeOffset < _writeBuffer.size())
			{
				ssize_t n = write(_fd, _writeBuffer.c_str() + _writeOffset,
								_writeBuffer.size() - _writeOffset);
				if (n > 0) _writeOffset += n;
			}
			if (_writeOffset >= _writeBuffer.size())
				closeConnection();
		}

		void closeConnection()
		{
			close(_fd);
			_loop.removeHandler(_fd);
		}
};


#endif