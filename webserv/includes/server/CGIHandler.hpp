#ifndef CGIHANDLER_H
# define CGIHANDLER_H

# include "PollLoop.hpp"
# include "Connection.hpp"
# include "IEventHandler.hpp"
# include "../webserv.hpp"

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
		CgiConnection( Connection* parent, int fd, bool isOutput, pid_t pid, PollLoop &loop, std::string body);
    	~CgiConnection();
    	CgiConnection(const CgiConnection& other);
    	CgiConnection& operator=(const CgiConnection& other);

		int getFd() const;
		void handleEvent(uint32_t events);
		void handleRead();
		void handleWrite();
		void closeConnection();
};

#endif