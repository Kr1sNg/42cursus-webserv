#include "generalTest.hpp"

Serverconfig::Serverconfig()
{

}

Serverconfig::Serverconfig(const Serverconfig& obj)
{
    _listen = obj._listen;
    _server_name = obj._server_name;
    _root = obj. _root;
    _error_pages = obj._error_pages;
    _client_max_body_size = obj._client_max_body_size;
    _locations = obj._locations;
}

Serverconfig& Serverconfig::operator=(const Serverconfig& obj)
{
    if (this != &obj)
    {
        _listen = obj._listen;
        _server_name = obj._server_name;
        _root = obj. _root;
        _error_pages = obj._error_pages;
        _client_max_body_size = obj._client_max_body_size;
       _locations = obj._locations;
    }
    return (*this);
}

Serverconfig::~Serverconfig()
{

}

void Serverconfig::setListen(const int& listen)
{
    _listen = listen;
}

const int& Serverconfig::getListen(void) const
{
    return (_listen);
}

void Serverconfig::setServer_name(const std::string& server_name)
{
    _server_name = server_name;
}

const std::string& Serverconfig::getServer_name(void) const
{
    return (_server_name);
}

void Serverconfig::setRoot(const std::string& root)
{
    _root = root;
}

const std::string& Serverconfig::getRoot(void) const
{
    return (_root);
}

void Serverconfig::setError_pages(const int& index, const std::string& error_pages)
{
    _error_pages[index] = error_pages;
}

const std::map<int, std::string>& Serverconfig::getError_pages(void) const
{
    return (_error_pages);
}

void Serverconfig::setClient_max_size(const size_t& client_max_body_size)
{
    _client_max_body_size = client_max_body_size;
}

const size_t& Serverconfig::getClient_max_size(void) const
{
    return (_client_max_body_size);
}

void Serverconfig::setlocation(const Locationconfig& location)
{
    _locations.push_back(location);
}

const std::vector<Locationconfig>& Serverconfig::getlocations(void) const
{
    return (_locations);
}