#ifndef LOCATIONCONFIG_H
# define LOCATIONCONFIG_H

#include "generalTest.hpp"

class Locationconfig
{
    private :
        std::string _path;
        std::string _root;
        std::string _index;
        bool _autoindex;
        std::vector<std::string> _methods;
        std::string _cgi_pass;
        std::string _redirect;
    public :
        Locationconfig();
        ~Locationconfig();
        Locationconfig(const Locationconfig& obj);
        Locationconfig& operator=(const Locationconfig& obj);

        void setPath(const std::string& path);
        const std::string& getPath(void) const;

        void setRoot(const std::string& root);
        const std::string& getRoot(void) const;

        void setIndex(const std::string& index);
        const std::string& getIndex(void) const;

        void setAutoindex(const bool& autoindex);
        const bool& getAutoindex(void) const;

        void setMethod(const std::string& method);
        const std::vector<std::string>& getMethods(void) const;

        void setCgi_pass(const std::string& cgi_pass);
        const std::string& getCgi_pass(void) const;

        void setRedirect(const std::string& redirect);
        const std::string& getRedirect(void) const;
};

#endif