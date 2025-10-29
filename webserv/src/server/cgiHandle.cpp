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

char **cgiEnv(const Request& req, Locationconfig location)
{
	std::vector<std::string> env;

	env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("QUERY_STRING=" + getQuery(req.getUri()));

	std::map<std::string, std::string>::const_iterator it = req.getHeaders().find("Content-Length");

    env.push_back("CONTENT_LENGTH=" + it->second);

	it = req.getHeaders().find("Content-Type");
    if (it != req.getHeaders().end() && !it->second.empty())
        env.push_back("CONTENT_TYPE=" + it->second);
    else
        env.push_back("CONTENT_TYPE=");
    env.push_back("SCRIPT_FILENAME=" + location.getCgi_pass());
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=" + req.getVersion());
    env.push_back("REDIRECT_STATUS=200");

    return (vectorToChar(env));
}

std::string cgiHandle(const Request& req, const Locationconfig& location, const Serverconfig& config)
{
    int pipe_in[2];
    int pipe_out[2];

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
        return "";

    char **env = cgiEnv(req, location);

    pid_t pid = fork();
    if (location.getRoot() != "")
        std::string path = location.getRoot() + location.getArg() + location.getCgi_pass();
    else
        std::string path = config.getRoot() + location.getArg() + location.getCgi_pass();
    if (pid < 0)
        return "";

    if (pid == 0)
    {
      
        char* argv[] = {const_cast<char*>(location.getCgi_pass().c_str()), NULL};

        close(pipe_in[1]);
        close(pipe_out[0]);

        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);

        close(pipe_in[0]);
        close(pipe_out[1]);

        execve(location.getCgi_pass().c_str(), argv, env);
        exit(1);
    }
    else
    {
        close(pipe_in[0]);
        close(pipe_out[1]);

        if (req.getMethod() == "POST" && !req.getBody().empty()) {
            ssize_t total = 0;
            const std::string& body = req.getBody();
            while (total < static_cast<ssize_t>(body.size())) {
                ssize_t written = write(pipe_in[1], body.c_str() + total, body.size() - total);
                if (written <= 0)
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

        return output;
    }
}
