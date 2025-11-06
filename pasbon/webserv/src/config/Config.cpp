#include "../../includes/webserv.hpp"
#include "../../includes/config/Config.hpp"
#include "../../includes/config/Serverconfig.hpp"

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

size_t  Config::getServersSize(void) const
{
    return (_servers.size());
}

Serverconfig const  &Config::getServerConfig(size_t index) const
{
    if (index < _servers.size())
        return (_servers[index]);
    else
        throw std::length_error("Config: getServerConfig: Out of range");
}