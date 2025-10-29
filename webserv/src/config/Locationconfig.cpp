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
    initStatics();
    _flags = 0;
    // _root = "";
    // _index = "";
    _autoindex = false;
    // _methods = ;
    // _cgi_pass = "";
    // _redirect = "";
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
        std::cout << "Locationconfig::setIndex:" << _index << std::endl; // it doesn't work !???
    }
}

const std::string& Locationconfig::getIndex(void) const
{
    std::cout << "getIndex: " << _index << std::endl;   // there's nothing inside index !??
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
    	throw std::out_of_range("Config: getLocationConfig: methods directive already defined in this location");
	}
    _flags |= _validDirective.at("methods");
    while (i < methods.size())
    {
        std::string m = methods[i];
        if (m == "GET" || m == "POST" || m == "DELETE")
        {
            _methods.push_back(m);
            std::cout << "setMethods: " << _methods[i] << std::endl;
        }
        else
            throw std::invalid_argument("Config: getLocationConfig: invalid HTTP methods");
        ++i;
    }
    std::cout << "Locationconfig::setMethods: _methods.size(): " << _methods.size() << std::endl;
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
        _cgi_pass = cgi_pass[0];
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
        throw  std::invalid_argument("Error locationconfig: invalid directive.");
        std::cout << "Directive " << directive.getName() << " is invalid." << std::endl;
    }
}

// Tat: Je dois ajouter pour verifier method, redirection, cgi allowed?...

bool    Locationconfig::isMethodAllowed(const std::string &method)
{
    for (size_t i = 0; i < _methods.size(); ++i)
    {
        std::cout << method << " vs methods[i]: " << _methods[i] << std::endl;
        if (method == _methods[i])
            return true;
    }
    std::cout << "There's no METHODS in _methods" << std::endl;
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