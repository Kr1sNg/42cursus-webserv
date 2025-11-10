#include "../../includes/server/CGIHandler.hpp"

#include "../../includes/server/CGIHandler.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

CgiConnection::CgiConnection( Connection* parent, int fd, int isOutput, pid_t pid, PollLoop &loop, std::string body)
	: _parent(parent), _fd(fd), _isOutput(isOutput), _writeBuffer(""), _writeOffset(0),  _pid(pid), _loop(loop)
// CgiConnection::CgiConnection(int fd, int isOutput, pid_t pid, PollLoop &loop, std::string body)
// 	: _fd(fd), _isOutput(isOutput), _writeBuffer(""), _writeOffset(0),  _pid(pid), _loop(loop)
{
	if (!_isOutput)
		_writeBuffer = body;
}

CgiConnection::~CgiConnection()
{
    if (_fd != -1)
        close(_fd);
}

CgiConnection::CgiConnection(const CgiConnection& other)
: _parent(other._parent), _fd(other._fd), _isOutput(other._isOutput), _writeBuffer(other._writeBuffer), _writeOffset(other._writeOffset), _pid(other._pid), _loop(other._loop)
{

}

CgiConnection& CgiConnection::operator=(const CgiConnection& other)
{
    if (this != &other)
    {
       _parent   = other._parent;
        _fd       = other._fd;
        _isOutput = other._isOutput;
        _pid      = other._pid;
        _loop     = other._loop;
		_writeOffset = other._writeOffset;
		_writeBuffer = other._writeBuffer;
    }
    return *this;
}

int CgiConnection::getFd() const
{
	return _fd;
}

void CgiConnection::closeConnection()
{
	close(_fd);
	_loop.removeHandler(_fd);
}

void CgiConnection::handleRead()
{
	int status;
	if (!_isOutput)
		return;

	char buffer[4096];
	ssize_t n = read(_fd, buffer, sizeof(buffer));
	if (n > 0 && _isOutput != 2)
		_parent->getCgiOutput().append(buffer, n);
	else if (n > 0 && _isOutput == 2)
	{
		_parent->getCGIError().append(buffer, n);
		closeConnection();
		waitpid(_pid, &status, 0);
	}	
	else if (n == 0)
	{
		closeConnection();
		waitpid(_pid, &status, 0);
		if (_isOutput != 2)
			_parent->onCgiComplete();
	}
}

void CgiConnection::handleWrite()
{
	if (_isOutput)
		return;

	if (_writeOffset < _writeBuffer.size())
	{
		ssize_t n = write(_fd, _writeBuffer.c_str() + _writeOffset,
		_writeBuffer.size() - _writeOffset);
		if (n > 0)
			_writeOffset += n;
		else
			return ;	// check n <= 0 after write()
	}
	if (_writeOffset >= _writeBuffer.size())
		closeConnection();
}

void CgiConnection::handleEvent(uint32_t events)
{
	if (std::time(NULL) - _parent->getStartTime() > 5)
	{
		kill(_pid, SIGKILL);
		_parent->getCGIError() = "Timeout";
	}
	if (_isOutput && (events & (POLLIN | POLLHUP)))
		handleRead();
	if (!_isOutput && (events & (POLLOUT | POLLHUP)))
		handleWrite();
	if (events & POLLERR)
		throw std::runtime_error("error of fd.");
}

