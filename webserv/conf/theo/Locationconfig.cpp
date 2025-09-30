#include "generalTest.hpp"

std::map<std::string, Locationconfig::_directiveFlag> Locationconfig::_validDirective;
std::map<std::string, void(Locationconfig::*)(const std::vector<std::string>&)> Locationconfig::_directiveHandler;

void Locationconfig::initStatics()
{
    _validDirective["index"] = INDEX;
    _validDirective["root"] = ROOT;
    _validDirective["autoindex"] = AUTOINDEX;
    _validDirective["methods"] = METHODS;
    _validDirective["cgipass"] = CGIPASS;
    _validDirective["redirect"] = REDIRECT;

    _directiveHandler["index"] = &Locationconfig::setIndex;
    _directiveHandler["root"] = &Locationconfig::setRoot;
    _directiveHandler["autoindex"] = &Locationconfig::setAutoindex;
    _directiveHandler["methods"] = &Locationconfig::setMethods;
    _directiveHandler["cgipass"] = &Locationconfig::setCgi_pass;
    _directiveHandler["redirect"] = &Locationconfig::setRedirect;
}

Locationconfig::Locationconfig()
{
    initStatics();
    _flags = 0;
}

Locationconfig::Locationconfig(const Locationconfig& obj)
{
    initStatics();

    _root = obj._root;
    _index = obj._index;
    _autoindex = obj._autoindex;
    _methods = obj._methods;
    _cgi_pass = obj._cgi_pass;
    _redirect = obj._redirect;

    _flags = 0;
}

Locationconfig& Locationconfig::operator=(const Locationconfig& obj)
{
    if (this != &obj)
    {
        _root = obj._root;
        _index = obj._index;
        _autoindex = obj._autoindex;
        _methods = obj._methods;
        _cgi_pass = obj._cgi_pass;
        _redirect = obj._redirect;
    }
    return (*this);
}

Locationconfig::~Locationconfig()
{

}

void Locationconfig::setRoot(const std::vector<std::string>& root)
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

const std::string& Locationconfig::getRoot(void) const
{
    return (_root);
}

void Locationconfig::setIndex(const std::vector<std::string>& index)
{
    // if (index.size() > 1)
    //     //error
    if (_flags & _validDirective.at("index"))
    {
        std::cout << "Error : index directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("index");
        _index = index[0];
    }
}

const std::string& Locationconfig::getIndex(void) const
{
    return (_index);
}

void Locationconfig::setAutoindex(const std::vector<std::string>& autoindex)
{
    if (_flags & _validDirective.at("autoindex"))
    {
        std::cout << "Error : autoindex directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("autoindex");
        if (autoindex[0] == "on")
            _autoindex = true;
        // else (autoindex[0] != "off")
        //     //error
    }
}

const bool& Locationconfig::getAutoindex(void) const
{
    return (_autoindex);
}

void Locationconfig::setMethods(const std::vector<std::string>& methods)
{
    size_t i = 0;

    if (_flags & _validDirective.at("methods"))
    {
        std::cout << "Error : methods directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("methods");
        while (methods[i] == "GET" || methods[i] == "POST" || methods[i] == "DELETE")
        {
            _methods.push_back(methods[i]);
            i++;
        }
        // if (i != methods.size())
        //     //error
    }
}

const std::vector<std::string>& Locationconfig::getMethods(void) const
{
    return (_methods);
}

void Locationconfig::setCgi_pass(const std::vector<std::string>& cgi_pass)
{
    if (_flags & _validDirective.at("cgi_pass"))
    {
        std::cout << "Error : cgi_pass directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("cgi_pass");
        _cgi_pass = cgi_pass[1];
    }
}

const std::string& Locationconfig::getCgi_pass(void) const
{
    return (_cgi_pass);
}


void Locationconfig::setRedirect(const std::vector<std::string>& redirect)
{
    if (_flags & _validDirective.at("redirect"))
    {
        std::cout << "Error : redirect directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirective.at("redirect");
        _redirect = redirect[0];
    }
    _redirect = redirect[0];
}

const std::string& Locationconfig::getRedirect(void) const
{
    return (_redirect);
}

void Locationconfig::addDirective(const Directive& directive)
{
    std::map<std::string, void(Locationconfig::*)(const std::vector<std::string>&)>::const_iterator it;
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