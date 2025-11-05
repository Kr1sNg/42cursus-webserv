#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

#include "generalconfig.hpp"

class Serverconfig
{
    private :
        std::vector<std::pair<std::string, std::string> > _listen;
        std::vector<std::string> _server_name;
        std::string _root;
        std::map<int, std::string> _error_pages;
        size_t _client_max_body_size;
        std::vector<Locationconfig> _locations;

        enum _directiveFlag
        {
            LISTEN = 1 << 0,
            ROOT = 1 << 1,
            CLIENT_MAX_BODY_SIZE = 1 << 2,
            ERROR_PAGE = 1 << 3,
        };

        int _flags;
        static std::map<std::string, _directiveFlag> _validDirective;
        static std::map<std::string, void(Serverconfig::*)(const std::vector<std::string>&)> _directiveHandler;
    public :
        Serverconfig();
        ~Serverconfig();
        Serverconfig(const Serverconfig& obj);
        Serverconfig& operator=(const Serverconfig& obj);

        void initStatics();
        void setListen(const std::vector<std::string>& listen);
        // const std::vector<std::pair<const char *, const char *> >& getListen(void) const;
        std::pair<std::string, std::string> const &getListen(size_t index) const;
        size_t  getListenSize(void) const;

        void setServer_name(const std::vector<std::string>& server_name);
        const std::vector<std::string>& getServer_name(void) const;
        // const std::string& getServer_name(void) const;
        void setRoot(const std::vector<std::string>& root);
        const std::string& getRoot(void) const;
        void addError_page(const std::vector<std::string>& error_page);
        // const std::map<int, std::string>& getError_pages(void) const;
        std::string getError_pages(int code) const;
        void setClient_max_size(const std::vector<std::string>& client_max_body_size);
        
        size_t getClient_max_size(void);
        void addlocation(const Locationconfig& location);
        const std::vector<Locationconfig>& getLocations(void) const;

        void addDirective(const Directive& directive);

        // to get Locationconfig
        Locationconfig *matchLocation(const std::string &url);
};

#endif