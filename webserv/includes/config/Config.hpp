#ifndef CONFIG_H
# define CONFIG_H

#include "generalconfig.hpp"
#include "Serverconfig.hpp"

class Config
{
    private :
        std::vector<Serverconfig> _servers;
    public :
        Config();
        ~Config();
        Config(const Config& obj);
        Config& operator=(const Config& obj);

        void addServer(const Serverconfig& server);
        const std::vector<Serverconfig>& getServers(void) const;

        size_t getServersSize(void) const;
        Serverconfig const  &getServerConfig(size_t index) const;
};

#endif