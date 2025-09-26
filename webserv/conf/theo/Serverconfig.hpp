#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

#include "generalTest.hpp"

class Serverconfig
{
    private :
        int _listen;
        std::string _server_name;
        std::string _root;
        std::map<int, std::string> _error_pages;
        size_t _client_max_body_size;
        std::vector<Locationconfig> _locations;
    public :
        Serverconfig();
        ~Serverconfig();
        Serverconfig(const Serverconfig& obj);
        Serverconfig& operator=(const Serverconfig& obj);

        void setListen(const int& listen);
        const int& getListen(void) const;
        void setServer_name(const std::string& server_name);
        const std::string& getServer_name(void) const;
        void setRoot(const std::string& root);
        const std::string& getRoot(void) const;
        void setError_pages(const int& index, const std::string& error_pages);
        const std::map<int, std::string>& getError_pages(void) const;
        void setClient_max_size(const size_t& client_max_body_size);
        const size_t& getClient_max_size(void) const;
        void setlocation(const Locationconfig& location);
        const std::vector<Locationconfig>& getlocations(void) const;
};

#endif