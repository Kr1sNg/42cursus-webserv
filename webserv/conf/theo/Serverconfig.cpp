#include "generalTest.hpp"

const std::map<std::string, Serverconfig::_directiveFlag> Serverconfig::_validDirective = {
    {"listen", Serverconfig::LISTEN},
    {"root", Serverconfig::ROOT},
    {"client_max_body_size", Serverconfig::CLIENT_MAX_BODY_SIZE},
    {"error_page", Serverconfig::ERROR_PAGE}
};

const std::map<std::string, void(Serverconfig::*)(const std::vector<std::string>&)> Serverconfig::_directiveHandler = {
    {"listen", &Serverconfig::setListen},
    {"server_name", &Serverconfig::setServer_name},
    {"root", &Serverconfig::setRoot},
    {"error_page", &Serverconfig::addError_page},
    {"client_max_body_size", &Serverconfig::setClient_max_size}
};

Serverconfig::Serverconfig()
{
    _flags = 0;
}

Serverconfig::Serverconfig(const Serverconfig& obj)
{
    _listen = obj._listen;
    _server_name = obj._server_name;
    _root = obj. _root;
    _error_pages = obj._error_pages;
    _client_max_body_size = obj._client_max_body_size;
    _locations = obj._locations;
    _flags = 0;
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
    if (!(_flags & _validDirective.at("listen")))
    {
        _flags |= _validDirective.at("listen");
    }
    _listen.push_back(listen);
}

const std::vector<int>& Serverconfig::getListen(void) const
{
    return (_listen);
}

void Serverconfig::setServer_name(const std::string& server_name)
{
    _server_name.push_back(server_name);
}

const std::vector<std::string>& Serverconfig::getServer_name(void) const
{
    return (_server_name);
}

void Serverconfig::setRoot(const std::string& root)
{
    if (_flags & _validDirective.at("root"))
    {
        std::cout << "Error : root directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("root");
        _root = root;
    }
}

const std::string& Serverconfig::getRoot(void) const
{
    return (_root);
}

void Serverconfig::addError_page(const int& index, const std::string& error_page)
{
    if (_error_pages.find(index) != _error_pages.end())
    {
        std::cout << "Error : Page" << index << "already defined in this location" << std::endl;
    }
    else
    {
        if (index == 404)
            _flags |= _validDirective.at("error_page");
        _error_pages[index] = error_page;
    }
}

const std::map<int, std::string>& Serverconfig::getError_pages(void) const
{
    return (_error_pages);
}

void Serverconfig::setClient_max_size(const size_t& client_max_body_size)
{
    if (_flags & _validDirective.at("_client_max_body_size"))
    {
        std::cout << "Error : _client_max_body_size directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("_client_max_body_size");
        _client_max_body_size = client_max_body_size;
    }
}

const size_t& Serverconfig::getClient_max_size(void) const
{
    return (_client_max_body_size);
}

void Serverconfig::addlocation(const Locationconfig& location)
{
    _locations.push_back(location);
}

const std::vector<Locationconfig>& Serverconfig::getlocations(void) const
{
    return (_locations);
}

bool Serverconfig::validDirective(const std::string& directive) const
{
    if (_validDirective.find(directive) == _validDirective.end())
    {
        std::cout << "Directive " << directive << " is invalid." << std::endl;
        return (true);
    }
    else
        return (false);
}

void Serverconfig::addDirective(const std::vector& directive)
{
    _directiveHandle
}
