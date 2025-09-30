#ifndef LOCATIONCONFIG_H
# define LOCATIONCONFIG_H

#include "generalTest.hpp"

class Locationconfig
{
    private :
        std::string _root;
        std::string _index;
        bool _autoindex;
        std::vector<std::string> _methods;
        std::string _cgi_pass;
        std::string _redirect;

        enum _directiveFlag
        {
            INDEX = 1 << 0,
            ROOT = 1 << 1,
            AUTOINDEX = 1 << 2,
            METHODS = 1 << 3,
            CGIPASS = 1 << 4,
            REDIRECT = 1 << 5,
        };

        int _flags;
        const std::map<std::string, _directiveFlag> _validDirective;

    public :
        Locationconfig();
        ~Locationconfig();
        Locationconfig(const Locationconfig& obj);
        Locationconfig& operator=(const Locationconfig& obj);

        void setRoot(const std::string& root);
        const std::string& getRoot(void) const;
        void setIndex(const std::string& index);
        const std::string& getIndex(void) const;
        void setAutoindex(const bool& autoindex);
        const bool& getAutoindex(void) const;
        void setMethod(const std::vector<std::string>& methods);
        const std::vector<std::string>& getMethods(void) const;
        void setCgi_pass(const std::string& cgi_pass);
        const std::string& getCgi_pass(void) const;
        void setRedirect(const std::string& redirect);
        const std::string& getRedirect(void) const;

        bool Locationconfig::validDirective(const std::string& directive) const;
};

#endif