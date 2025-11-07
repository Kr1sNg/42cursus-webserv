#include "../../includes/webserv.hpp"

std::map<std::string, Locationconfig::_directiveFlag> Locationconfig::_validDirective;
std::map<std::string, void(Locationconfig::*)(const std::vector<std::string>&)> Locationconfig::_directiveHandler;

void Locationconfig::initStatics()
{
    _validDirective["index"] = INDEX;
    _validDirective["root"] = ROOT;
    _validDirective["autoindex"] = AUTOINDEX;
    _validDirective["methods"] = METHODS;
    _validDirective["cgi_pass"] = CGIPASS;
    _validDirective["redirect"] = REDIRECT;

    _directiveHandler["index"] = &Locationconfig::setIndex;
    _directiveHandler["root"] = &Locationconfig::setRoot;
    _directiveHandler["autoindex"] = &Locationconfig::setAutoindex;
    _directiveHandler["methods"] = &Locationconfig::setMethods;
    _directiveHandler["cgi_pass"] = &Locationconfig::setCgi_pass;
    _directiveHandler["redirect"] = &Locationconfig::setRedirect;
}

Locationconfig::Locationconfig()
{
    _autoindex = false;
    initStatics();
    _flags = 0;
}

Locationconfig::Locationconfig(const Locationconfig& obj)
{
    initStatics();

    _flags = 0;

    _root = obj._root;
    _index = obj._index;
    _autoindex = obj._autoindex;
    _methods = obj._methods;
    _cgi_pass = obj._cgi_pass;
    _redirect = obj._redirect;
    _arg = obj._arg;


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
        _arg = obj._arg;
    }
    return (*this);
}

Locationconfig::~Locationconfig()
{

}

void Locationconfig::setArg(std::string arg)
{
    _arg = arg;
}

const std::string& Locationconfig::getArg(void) const
{
    return (_arg);
}


void Locationconfig::setRoot(const std::vector<std::string>& root)
{
    if (root.size() != 1)
        throw std::invalid_argument ("Error directive root : The root directive must have only one argument.");
    if (_flags & _validDirective.at("root"))
    {
        throw std::invalid_argument ("Error directive root : the root directive is already defined in this location.");
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
    if (index.size() != 1)
        throw std::invalid_argument ("Error directive index : The index directive must have only one argument.");
    if (_flags & _validDirective.at("index"))
    {
        throw std::invalid_argument ("Error directive index : The index directive is already defined in this location.");
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
    if (autoindex.size() != 1)
        throw std::invalid_argument ("Error directive autoindex : The autoindex directive must have only one argument.");
    if (_flags & _validDirective.at("autoindex"))
    {
        throw std::invalid_argument ("Error directive autoindex : The autoindex directive is already defined in this location.");
    }
    else
    {
        _flags |= _validDirective.at("autoindex");
        if (autoindex[0] == "on")
            _autoindex = true;
        else if (autoindex[0] != "off")
            throw std::invalid_argument ("Error directive autoindex : The autoindex directive has an invalid argument.");
        else
            _autoindex = false;
    }
}

const bool& Locationconfig::getAutoindex(void) const
{
    return (_autoindex);
}

void Locationconfig::setMethods(const std::vector<std::string>& methods)
{
    size_t i = 0;
    if (methods.size() < 1 || methods.size() > 3)
        throw std::invalid_argument ("Error directive methods : The methods directive must have beetween one and three arguments.");
    if (_flags & _validDirective.at("methods"))
    {
        throw std::invalid_argument ("Error directive methods : The methods directive is already defined in this location.");
	}
    else
    {
        _flags |= _validDirective.at("methods");
        while (i < methods.size())
        {
            std::string m = methods[i];
            if (m == "GET" || m == "POST" || m == "DELETE")
            {
                _methods.push_back(m);
            }
            else
                throw std::invalid_argument ("Error directive methods : The methods directive has an invalid argument.");
            ++i;
        }
    }
}

const std::vector<std::string>& Locationconfig::getMethods(void) const
{
    return (_methods);
}

void Locationconfig::setCgi_pass(const std::vector<std::string>& cgi_pass)
{
    if (cgi_pass.size() != 1)
        throw std::invalid_argument ("Error directive cgi_pass : The cgi_pass directive must have only one argument.");
    if (_flags & _validDirective.at("cgi_pass"))
    {
        throw std::invalid_argument ("Error directive cgi_pass : The cgi_pass directive is already defined in this location.");
    }
    else
    {
        _flags |= _validDirective.at("cgi_pass");
        _cgi_pass = cgi_pass[0];
    }
}

const std::string& Locationconfig::getCgi_pass(void) const
{
    return (_cgi_pass);
}


void Locationconfig::setRedirect(const std::vector<std::string>& redirect)
{
    if (redirect.size() != 1)
        throw std::invalid_argument ("Error directive redirect : The redirect directive must have only one argument.");
    if (_flags & _validDirective.at("redirect"))
    {
        throw std::invalid_argument ("Error directive redirect : The redirect directive is already defined in this location.");
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
        throw  std::invalid_argument("Error locationconfig: invalid directive.");
        // std::cout << "Directive " << directive.getName() << " is invalid." << std::endl;
    }
}

// Tat: Je dois ajouter pour verifier method, redirection, cgi allowed?...

bool    Locationconfig::isMethodAllowed(const std::string &method)
{
    for (size_t i = 0; i < _methods.size(); ++i)
    {
        // std::cout << method << " vs methods[i]: " << _methods[i] << std::endl;
        if (method == _methods[i])
            return true;
    }
    // std::cout << "There's no METHODS in _methods" << std::endl;
    return (false);
}

bool    Locationconfig::hasRedirect(void)
{
    return (_redirect != "");
}

bool    Locationconfig::hasCgi(void)
{
    return (_cgi_pass != "");
}