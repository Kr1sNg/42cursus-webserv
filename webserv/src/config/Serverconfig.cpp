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

    //  _listen = ;
    // _server_name = ;
    _root = "Default";
    // _error_pages = ;
    _client_max_body_size = 0;
    // _locations = ;
}

Serverconfig::Serverconfig(const Serverconfig& obj)
{
    _listen = obj._listen;
    _server_name = obj._server_name;
    _root = obj. _root;
    _error_pages = obj._error_pages;
    _client_max_body_size = obj._client_max_body_size;
    _locations = obj._locations;

    //  _listen = ;
    // _server_name = ;
    _root = "Default";
    // _error_pages = ;
    _client_max_body_size = 0;
    // _locations = ;
    _flags = 0;

    initStatics();
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

    if (listen.size() > 1)
        //error
    if (!(_flags & _validDirective.at("listen")))
    {
        _flags |= _validDirective.at("listen");
    }
    while (i < listen.size())
    {
        pos = listen[i].find(':');
        std::pair<const char *, const char *> id;
    
        if (pos != std::string::npos)
        {
            id.first = listen[i].substr(0, pos).c_str();
            id.second = listen[i].substr(pos + 1).c_str();
        }
        else
        {
            id.first = "127.0.0.1";
            id.second = listen[i].c_str();
        }
        _listen.push_back(id);
        i++;
    } 
}

const std::vector<std::pair<const char *, const char *> >& Serverconfig::getListen(void) const
{
    return (_listen);
}

void Serverconfig::setServer_name(const std::vector<std::string>& server_name)
{
    //  if (server_name.size() > 1)
    //     //error
    _server_name.push_back(server_name[0]);
}

const std::vector<std::string>& Serverconfig::getServer_name(void) const
{
    return (_server_name);
}

void Serverconfig::setRoot(const std::vector<std::string>& root)
{
    // if (root.size() > 1)
    //     //error
    if (_flags & _validDirective.at("root"))
    {
        std::cout << "Error : root directive already defined in this location" << std::endl;
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
    // if (error_page.size() != 2)
    //     //error
    if (_error_pages.find(index) != _error_pages.end())
    {
        std::cout << "Error : Page" << index << "already defined in this location" << std::endl;
    }
    else
    {
        if (index == 404)
            _flags |= _validDirective.at("error_page");
        _error_pages[index] = error_page[1];
    }
}

const std::map<int, std::string>& Serverconfig::getError_pages(void) const
{
    return (_error_pages);
}

void Serverconfig::setClient_max_size(const std::vector<std::string>& client_max_body_size)
{

    //  if (client_max_body_size.size() != 1)
    //     //error
    size_t size = static_cast<size_t>(std::strtoul(client_max_body_size[0].c_str(), NULL, 10));
    if (_flags & _validDirective.at("_client_max_body_size"))
    {
        std::cout << "Error : _client_max_body_size directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("_client_max_body_size");
        _client_max_body_size = size;
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
        std::cout << "Directive " << directive.getName() << " is invalid." << std::endl;
    }
}
