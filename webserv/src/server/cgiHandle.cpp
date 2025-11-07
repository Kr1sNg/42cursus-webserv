#include "../../includes/server/CGIHandler.hpp"
#include "../../includes/webserv.hpp"
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

void cgiHandle(Connection& parent, const Request& req, const Locationconfig& location, const std::string& body, PollLoop& _loop)
{
    int pipe_in[2];
    int pipe_out[2];
    std::string path;

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
	{
        throw std::runtime_error("cgiHandle: pipe is not working.");
		return ;
	}
    char **env = cgiEnv(req);
 
    path = location.getRoot() +  makepath(req.getUri());
    // std::cout << path << std::endl;
    char* argv[] = {const_cast<char*>(location.getCgi_pass().c_str()), const_cast<char*>(path.c_str()), NULL};
	pid_t pid = fork();
    if (pid < 0)
	{
        throw std::runtime_error("cgiHandle: fork is not working.");
		return ;
	}
    if (pid == 0)
    {
        close(pipe_in[1]);
        close(pipe_out[0]);

        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[0]);
        close(pipe_out[1]);

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

        CgiConnection *cgiIn  = new CgiConnection(&parent, pipe_in[1], false, pid, _loop, body);
        CgiConnection *cgiOut = new CgiConnection(&parent, pipe_out[0], true, pid, _loop, body);
    
        fcntl(pipe_in[1], F_SETFL, O_NONBLOCK);
        fcntl(pipe_out[0], F_SETFL, O_NONBLOCK);

        _loop.addHandler(cgiIn, POLLOUT);
        _loop.addHandler(cgiOut, POLLIN);
		freeVectorChar(env);
    }
}
