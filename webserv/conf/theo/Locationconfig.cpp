#include "generalTest.hpp"

Locationconfig::Locationconfig()
{

}

Locationconfig::Locationconfig(const Locationconfig& obj) :
{
    _path = obj._path;
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
        _path = obj._path;
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

void Locationconfig::setPath(const std::string& path)
{
    _path = path;
}

const std::string& Locationconfig::getPath(void) const
{
    return (_path);
}


void Locationconfig::setRoot(const std::string& root)
{
    _root = root;
}

const std::string& Locationconfig::getRoot(void) const
{
    return (_root);
}

void Locationconfig::setIndex(const std::string& index)
{
    _index = index;
}

const std::string& Locationconfig::getIndex(void) const
{
    return (_index);
}

void Locationconfig::setAutoindex(const bool& autoindex)
{
    _autoindex = autoindex;
}

const bool& Locationconfig::getAutoindex(void) const
{
    return (_autoindex);
}

void Locationconfig::setMethod(const std::string& method)
{
    _methods.push_back(method);
}

const std::vector<std::string>& Locationconfig::getMethods(void) const
{
    return (_methods);
}

void Locationconfig::setCgi_pass(const std::string& cgi_pass)
{
    _cgi_pass = cgi_pass;
}

const std::string& Locationconfig::getCgi_pass(void) const
{
    return (_cgi_pass);
}


void Locationconfig::setRedirect(const std::string& redirect)
{
    _redirect = redirect;
}

const std::string& Locationconfig::getRedirect(void) const
{
    return (_redirect);
}
