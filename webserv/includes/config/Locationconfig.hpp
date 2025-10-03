#ifndef LOCATIONCONFIG_H
# define LOCATIONCONFIG_H

#include "generalconfig.hpp"

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
        static std::map<std::string, _directiveFlag> _validDirective;
        static std::map<std::string, void(Locationconfig::*)(const std::vector<std::string>&)> _directiveHandler;
    public :
        Locationconfig();
        ~Locationconfig();
        Locationconfig(const Locationconfig& obj);
        Locationconfig& operator=(const Locationconfig& obj);

        void initStatics();
        void setRoot(const std::vector<std::string>& root);
        const std::string& getRoot(void) const;
        void setIndex(const std::vector<std::string>& index);
        const std::string& getIndex(void) const;
        void setAutoindex(const std::vector<std::string>& autoindex);
        const bool& getAutoindex(void) const;
        void setMethods(const std::vector<std::string>& methods);
        const std::vector<std::string>& getMethods(void) const;
        void setCgi_pass(const std::vector<std::string>& cgi_pass);
        const std::string& getCgi_pass(void) const;
        void setRedirect(const std::vector<std::string>& redirect);
        const std::string& getRedirect(void) const;

        void addDirective(const Directive& directive);
};

#endif