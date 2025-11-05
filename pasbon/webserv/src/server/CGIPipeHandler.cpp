#include "../../includes/server/CGIPipeHandler.hpp"
#include "../../includes/server/Connection.hpp"

CGIPipeHandler::CGIPipeHandler(Connection *conn, int fd, PipeType type):
        _connection(conn),
        _pipeFd(fd),
        _type(type)
{

}

CGIPipeHandler::~CGIPipeHandler()
{

}

int CGIPipeHandler::getFd(void) const
{
    return _pipeFd;
}
        
void    CGIPipeHandler::handleEvent(uint32_t events)
{
    if (_type == PIPE_READ)
        _connection->handleCgiRead(events);
    else if (_type == PIPE_WRITE)
        _connection->handleCgiWrite(events);
}



void	Connection::handleCgiWrite(uint32_t events)
{
    (void)events; 
    
    ssize_t n = write(_cgiWriteHandler->getFd(), 
                      _cgiWriteBuffer.c_str(), 
                      _cgiWriteBuffer.size());
    
    if (n < 0) {
        generateErrorResponse(500, "Permission Denied (handleCgiWrite)");
        cleanupCgi();
        return;
    }

    _cgiWriteBuffer.erase(0, n);

    // If we are done writing, remove this handler from the PollLoop
    if (_cgiWriteBuffer.empty())
    {
        _server->markForClose(_cgiWriteHandler->getFd());
        close(_cgiWriteHandler->getFd());
        delete _cgiWriteHandler;
        _cgiWriteHandler = NULL;
    }
}

void	Connection::handleCgiRead(uint32_t events)
{
    (void)events;
    char    buf[4096];
    ssize_t n = read(_cgiReadHandler->getFd(), buf, sizeof(buf));

    if (n > 0)
    {
        // Append data from CGI to our *internal* read buffer
        _bodyCGI.append(buf, n);
    }
    else if (n == 0) // EOF - CGI process is done
    {
        // 1. Clean up all CGI resources (handlers, FDs)
        cleanupCgi();
        
        // 2. Wait for the child (replaces your blocking waitpid)
        int status;
        waitpid(_cgiPid, &status, 0); // This is non-blocking now
        _cgiPid = -1;

        // 3. Parse the response from _cgiReadBuffer
        //    (You must write this!)
        // _parseCgiResponse(_cgiReadBuffer); 

        // 4. Set state to send the response to the client
        _connState = CONN_WRITING_RESPONSE;
        // _server->getPollLoop()->modHandler(this, POLLOUT);
        _server->setFdEvents(_cgiReadHandler->getFd(), POLLOUT);
    }
    else // n < 0
    {
        generateErrorResponse(500, "Permission Denied (handleCgiRead)");
        cleanupCgi();
    }
}

void	Connection::cleanupCgi(void)
{
    if (_cgiReadHandler)
    {
        _server->getPollLoop().removeHandler(_cgiReadHandler->getFd());
        close(_cgiReadHandler->getFd());
        delete _cgiReadHandler;
        _cgiReadHandler = NULL;
    }
    if (_cgiWriteHandler)
    {
        _server->getPollLoop().removeHandler(_cgiWriteHandler->getFd());
        close(_cgiWriteHandler->getFd());
        delete _cgiWriteHandler;
        _cgiWriteHandler = NULL;
    }
    
    // Ensure the child process is terminated
    if (_cgiPid > 0)
    {
        kill(_cgiPid, SIGTERM);
        waitpid(_cgiPid, NULL, 0);
        _cgiPid = -1;
    }
}

//theo
/*
std::string cgiHandle(const Request& req, const Locationconfig& location, const Serverconfig& config, const std::string& body)
{
    int pipe_in[2];
    int pipe_out[2];
    std::string path;

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
	{
        throw std::runtime_error("cgiHandle: pipe is not working.");
		return "";
	}

	// server.putFdinpoll(pipe_out);
	// server.putFdinpoll(pipe_in);
    
    char **env = cgiEnv(req);
 
    if (location.getRoot() != "")
        path = location.getRoot() + makepath(req.getUri());
    else
        path = config.getRoot() +  makepath(req.getUri());
    std::cout << path << std::endl;
    char* argv[] = {const_cast<char*>(location.getCgi_pass().c_str()), const_cast<char*>(path.c_str()), NULL};
	pid_t pid = fork();
    if (pid < 0)
	{
        throw std::runtime_error("cgiHandle: fork is not working.");
		return "";
	}
    if (pid == 0)
    {
        close(pipe_in[1]);
        close(pipe_out[0]);

        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[0]);
        close(pipe_out[1]);

		//serve.markforcloseFD(pipe_out);
		//serve.markforcloseFD(pipe_in);

      	int value = execve(location.getCgi_pass().c_str(), argv, env);
		if (value == -1)
		{
			freeVectorChar(env);
			throw std::runtime_error("cgiHandle: script was not executed.");
		}
        exit(1);
    }
    else
    {
        close(pipe_in[0]);
        close(pipe_out[1]);
        // std::cout << "cgi body : " << body << std::endl;
        if (req.getMethod() == "POST" && !body.empty()) {
            ssize_t total = 0;
            while (total < static_cast<ssize_t>(body.size())) {
                ssize_t written = write(pipe_in[1], body.c_str() + total, body.size() - total);
                if (written <= 0)
                {
					throw std::runtime_error("cgiHandle: write to CGI stdin failed.");        
                    break;
                }
            total += written;
            }
        }
        close(pipe_in[1]);

        std::string output;
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_out[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, bytesRead);
        }
        close(pipe_out[0]);

        waitpid(pid, NULL, 0);
		freeVectorChar(env);
		//serve.markforcloseFD(pipe_out);
		//serve.markforcloseFD(pipe_in);
        return output;
    }
}
*/


