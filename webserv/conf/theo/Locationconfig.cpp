#include "generalTest.hpp"

Locationconfig::Locationconfig()
{

}

Locationconfig::Locationconfig(const Locationconfig& obj)
{
    _root = obj._root;
    _index = obj._index;
    _autoindex = obj._autoindex;
    _methods = obj._methods;
    _cgi_pass = obj._cgi_pass;
    _redirect = obj._redirect;
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

void Locationconfig::setRoot(const std::string& root)
{
    if (_flags & _validDirectives.at("root"))
    {
        std::cout << "Error : root directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirectives.at("root");
        _root = root;
    }
}

const std::string& Locationconfig::getRoot(void) const
{
    return (_root);
}

void Locationconfig::setIndex(const std::string& index)
{
    if (_flags & _validDirectives.at("index"))
    {
        std::cout << "Error : index directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirectives.at("index");
        _index = index;
    }
}

const std::string& Locationconfig::getIndex(void) const
{
    return (_index);
}

void Locationconfig::setAutoindex(const bool& autoindex)
{
    if (_flags & _validDirectives.at("autoindex"))
    {
        std::cout << "Error : autoindex directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirectives.at("autoindex");
        _autoindex = autoindex;
    }
}

const bool& Locationconfig::getAutoindex(void) const
{
    return (_autoindex);
}

void Locationconfig::setMethod(const std::vector<std::string>& methods)
{
    if (_flags & _validDirectives.at("methods"))
    {
        std::cout << "Error : methods directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirectives.at("methods");
        _methods = methods;
    }
}

const std::vector<std::string>& Locationconfig::getMethods(void) const
{
    return (_methods);
}

void Locationconfig::setCgi_pass(const std::string& cgi_pass)
{
    if (_flags & _validDirectives.at("cgi_pass"))
    {
        std::cout << "Error : cgi_pass directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirectives.at("cgi_pass");
        _cgi_pass = cgi_pass;
    }
}

const std::string& Locationconfig::getCgi_pass(void) const
{
    return (_cgi_pass);
}


void Locationconfig::setRedirect(const std::string& redirect)
{
    if (_flags & _validDirectives.at("redirect"))
    {
        std::cout << "Error : redirect directive already defined in this location" << std::endl;
    }
    else
    {
        _flags |= _validDirectives.at("redirect");
        _redirect = redirect;
    }
    _redirect = redirect;
}

const std::string& Locationconfig::getRedirect(void) const
{
    return (_redirect);
}

bool Locationconfig::validDirective(const std::string& directive) const
{
    if (_validDirectives.find(directive) == _validDirectives.end())
    {
        std::cout << "Directive " << directive << " is invalid." << std::endl;
        return (true);
    }
    else
        return (false);
}