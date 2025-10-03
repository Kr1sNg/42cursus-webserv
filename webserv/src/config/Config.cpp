#include "../../includes/webserv.hpp"

Config::Config()
{

}

Config::Config(const Config& obj) : _servers(obj._servers)
{

}

Config& Config::operator=(const Config& obj)
{
    if (this != &obj)
    {
        _servers = obj._servers;
    }
    return (*this);
}

Config::~Config()
{

}

void Config::addServer(const Serverconfig& server)
{
    _servers.push_back(server);
}

const std::vector<Serverconfig>& Config::getServers(void) const
{
    return (_servers);
}
