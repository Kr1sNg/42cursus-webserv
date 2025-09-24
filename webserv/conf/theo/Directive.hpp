#ifndef DIRECTIVE_H
# define DIRECTIVE_H

#include "generalTest.hpp"

class Directive
{
    private :
        std::string _name;
        std::vector<std::string> _args;
    public :
        Directive();
        Directive(const Directive& obj);
        Directive& operator=(const Directive& obj);
        ~Directive();

        void setName(std::string name);
        const std::string& getName(void) const;
        void addArg(const std::string& arg);
        const std::vector<std::string>& getArgs(void) const;
};

#endif