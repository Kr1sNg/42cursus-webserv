
#include "../../includes/server/CGIPipeHandler.hpp"
#include "../../includes/server/Connection.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



std::string getQuery(std::string url)
{
	size_t pos = url.find('?');
    if (pos == std::string::npos) {
        return ("");
    }
    size_t pos_hash = url.find('#', pos);
    if (pos_hash != std::string::npos) {
        return url.substr(pos + 1, pos_hash - pos - 1);
    }
    return url.substr(pos + 1);
}

void freeVectorChar(char **tab)
{
	int i = 0;

    if (!tab)
        return;
    while (tab[i])
	{
        free(tab[i]);
		i++;
	}
    delete[] tab;
}

char **vectorToChar(std::vector<std::string> vector)
{
	size_t i = 0;
	char ** tab = new char*[vector.size() + 1];
	while (i < vector.size())
	{
		tab[i] = strdup(vector[i].c_str());
		i++;
	}
	tab[i] = NULL;
	return (tab);
}

std::string makepath(std::string url)
{
	size_t pos = url.find('?');
    if (pos == std::string::npos) {
        return (url);
    }
    return (url.substr(0, pos));
}
std::string namescript(std::string url)
{
	std::string name;
	size_t pos;

	name = makepath(url);
	pos = name.find_last_of('/');
	return (name.substr(pos + 1));
}

char **cgiEnv(const Request& req)
{
	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("QUERY_STRING=" + getQuery(req.getUri()));

	std::map<std::string, std::string>::const_iterator it = req.getHeaders().find("Content-Length");

    
    if (it != req.getHeaders().end() && !it->second.empty())
        env.push_back("CONTENT_LENGTH=" + it->second);
    else
    {
        env.push_back("CONTENT_LENGTH=");
    }

	it = req.getHeaders().find("Content-Type");
    if (it != req.getHeaders().end() && !it->second.empty())
        env.push_back("CONTENT_TYPE=" + it->second);
    else
        env.push_back("CONTENT_TYPE=");
    env.push_back("SCRIPT_FILENAME=" + namescript(req.getUri()));
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=" + req.getVersion());
    env.push_back("REDIRECT_STATUS=200");

    // size_t i = 0;
    // while (i < env.size())
    // {
    //     std::cout << env[i] << std::endl;
    //     i++;
    // }

    return (vectorToChar(env));
}

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

//GG

void Connection::handleCgi(const Locationconfig &location)
{
    int pipe_in[2];
    int pipe_out[2];
    std::string path;

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        generateErrorResponse(500, "Permission Denied (handleCgi)");
        return;
    }

    // --- Get the body to send to CGI ---
    _cgiWriteBuffer = _bodyCGI; // Assumes body is buffered

    // --- Your existing setup logic ---
    char **env = cgiEnv(_request); // (This must be a member of Connection)
    // ... logic to set 'path' ...

    char* argv[] = {const_cast<char*>(location.getCgi_pass().c_str()), const_cast<char*>(path.c_str()), NULL};

    _cgiPid = fork();
    if (_cgiPid < 0) {
        generateErrorResponse(500, "Permission Denied (handleCgi)");
        return;
    }
    
    if (_cgiPid == 0) // --- Child Process ---
    {
        // --- This logic is from your cgiHandle and is correct ---
        close(pipe_in[1]);
        close(pipe_out[0]);
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        execve(location.getCgi_pass().c_str(), argv, env);
        
        // This MUST be std::exit, not throw
        // freeVectorChar(env);
        std::exit(EXIT_FAILURE); 
    }
    else // --- Parent Process (Non-Blocking) ---
    {
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        int readFd = pipe_out[0];
        int writeFd = pipe_in[1];

        // **CRITICAL: Set pipes to NON-BLOCKING**
        fcntl(readFd, F_SETFL, O_NONBLOCK);
        fcntl(writeFd, F_SETFL, O_NONBLOCK);

        // --- Create Handlers (as shown in previous answer) ---
        _cgiReadHandler = new CGIPipeHandler(this, readFd, PIPE_READ);
        
        // We only add a write handler if there is a body to send
        if (!_cgiWriteBuffer.empty()) {
            _cgiWriteHandler = new CGIPipeHandler(this, writeFd, PIPE_WRITE);
        } else {
            close(writeFd); // No body, close our end
        }

        // --- ADD HANDLERS TO THE POLL LOOP ---
        _server->getPollLoop().addHandler(_cgiReadHandler, POLLIN);
        if (_cgiWriteHandler) {
            _server->getPollLoop().addHandler(_cgiWriteHandler, POLLOUT);
        }

        // --- Stop watching the client socket, start watching CGI ---
        // _connState = CONN_CGI_WRITE; // Or CONN_CGI_READ
        // _server->getPollLoop()->modHandler(this, 0); // Stop watching client
        // freeVectorChar(env);
        _server->markForClose(_clientFd);
    }
}

