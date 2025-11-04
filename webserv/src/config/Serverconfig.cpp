#include "../../includes/webserv.hpp"

std::map<std::string, Serverconfig::_directiveFlag> Serverconfig::_validDirective;
std::map<std::string, void(Serverconfig::*)(const std::vector<std::string>&)> Serverconfig::_directiveHandler;

void Serverconfig::initStatics()
{
    _validDirective["listen"] = LISTEN;
    _validDirective["root"] = ROOT;
    _validDirective["client_max_body_size"] = CLIENT_MAX_BODY_SIZE;
    _validDirective["error_page"] = ERROR_PAGE;

    _directiveHandler["listen"] = &Serverconfig::setListen;
    _directiveHandler["server_name"] = &Serverconfig::setServer_name;
    _directiveHandler["root"] = &Serverconfig::setRoot;
    _directiveHandler["error_page"] = &Serverconfig::addError_page;
    _directiveHandler["client_max_body_size"] = &Serverconfig::setClient_max_size;
}

Serverconfig::Serverconfig()
{
    _flags = 0;
    initStatics();

}

Serverconfig::Serverconfig(const Serverconfig& obj)
{
    initStatics(); 
    _flags = 0;
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

void Serverconfig::setListen(const std::vector<std::string>& listen)
{
    size_t i = 0;
    size_t pos;

    if (listen.size() < 1)
        throw std::invalid_argument ("Error directive listen : The listen directive must have enough arguments.");
    if (!(_flags & _validDirective.at("listen")))
    {
        _flags |= _validDirective.at("listen");
    }
    while (i < listen.size())
    {
        pos = listen[i].find(':');
        std::pair<std::string, std::string> id;
    
        if (pos != std::string::npos)
        {
            id.first = listen[i].substr(0, pos);
            id.second = listen[i].substr(pos + 1);
        }
        else
        {
            id.first = "127.0.0.1";
            id.second = listen[i];
        }
        _listen.push_back(id);
        i++;
    }
}

const std::pair<std::string, std::string>& Serverconfig::getListen(size_t index) const
{
    if (index < _listen.size())
        return (_listen[index]);
    throw std::length_error("Serverconfig: ListenSize: Out of range");
}

void Serverconfig::setServer_name(const std::vector<std::string>& server_name)
{
    if (server_name.size() < 1)
        throw std::invalid_argument ("Error directive server_name : The server_name directive must have enough arguments.");
    size_t i = 0;

    while (i < server_name.size())
    {
        _server_name.push_back(server_name[i]);
        i++;
    }
}

const std::vector<std::string>& Serverconfig::getServer_name(void) const
{
    return (_server_name);
}

void Serverconfig::setRoot(const std::vector<std::string>& root)
{
    if (root.size() != 1)
        throw std::invalid_argument ("Error directive root : The root directive must have only one argument.");
    if (_flags & _validDirective.at("root"))
    {
        throw std::invalid_argument ("Error directive root : the root directive is already defined in this server.");
    }
    else
    {
        _flags |= _validDirective.at("root");
        _root = root[0];
    }
}

const std::string& Serverconfig::getRoot(void) const
{
    return (_root);
}

void Serverconfig::addError_page(const std::vector<std::string>& error_page)
{
    std::stringstream ss(error_page[0]);
    int index;
    
    ss >> index;
    if (_error_pages.find(index) != _error_pages.end())
    {
        throw std::invalid_argument ("Error directive : This page is already defined in this location.");
    }
    else
    {
        if (index == 404)
            _flags |= _validDirective.at("error_page");
        _error_pages[index] = error_page[1];
    }
}

// const std::map<int, std::string>& Serverconfig::getError_pages(void) const
// {
//     return (_error_pages); // c'est quoi cet logique ???
// }

std::string Serverconfig::getError_pages(int code) const   // it helps to get Error page location
{
    std::map<int, std::string>::const_iterator it = _error_pages.find(code);
    if (it != _error_pages.end())
        return (_root + it->second);
    return (_root);
}

void Serverconfig::setClient_max_size(const std::vector<std::string>& client_max_body_size)
{

    //  if (client_max_body_size.size() != 1)
    //     //error
    size_t size = static_cast<size_t>(std::strtoul(client_max_body_size[0].c_str(), NULL, 10));
    if (_flags & _validDirective.at("client_max_body_size"))
    {
        throw std::runtime_error("client_max_body_size directive already defined in this location");
    }
    else
    {
        _flags |= _validDirective.at("client_max_body_size");
        _client_max_body_size = size;
    }
}

size_t Serverconfig::getClient_max_size(void)
{
    return (_client_max_body_size);
}

void Serverconfig::addlocation(const Locationconfig& location)
{
    _locations.push_back(location);
}

const std::vector<Locationconfig>& Serverconfig::getLocations(void) const
{
    return (_locations);
}

void Serverconfig::addDirective(const Directive& directive)
{
    std::map<std::string, void(Serverconfig::*)(const std::vector<std::string>&)>::const_iterator it;
    it = _directiveHandler.find(directive.getName());

    if (it != _directiveHandler.end())
    {
        (this->*(it->second))(directive.getArgs());
    }
    else
    {
        std::string message =  "Directive " + directive.getName() + " is invalid.";
        throw std::runtime_error (message);
    }
}

size_t  Serverconfig::getListenSize(void) const
{
    return (_listen.size());
}


Locationconfig *Serverconfig::matchLocation(const std::string &url)
{
    int     index = -1; 
    size_t  i = 0;
    size_t  bestLength = 0;

    while (i < _locations.size())
    {
        if (url.rfind(_locations[i].getArg(), 0) == 0 && bestLength < _locations[i].getArg().size())
        {
            bestLength = _locations[i].getArg().size();
            index = i;
        }
        i++;
    }
    if (index != -1)
        return (&_locations[index]);
    else
        return (NULL);
}

