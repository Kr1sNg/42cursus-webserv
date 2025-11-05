#include "../../includes/webserv.hpp"

Directive::Directive()
{

}
Directive::Directive(const Directive& obj) : _name(obj._name), _args(obj._args)
{
                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
}

Directive& Directive::operator=(const Directive& obj)
{
    if (this != &obj)
    {
        _name = obj._name;
         _args = obj._args;
    }
    return (*this);
}

Directive::~Directive()
{

}

void Directive::setName(std::string const name)
{
    _name = name;
}

const std::string& Directive::getName(void) const
{
    return (_name);
}

const std::vector<std::string>& Directive::getArgs(void) const
{
    return(_args);
}

void Directive::addArg(const std::string& arg)
{
    _args.push_back(arg);
}

