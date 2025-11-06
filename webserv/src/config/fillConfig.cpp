#include "../../includes/config/generalconfig.hpp"
#include "../../includes/webserv.hpp"

void defaultLocation(Locationconfig& config, const Serverconfig& server)
{
    if (!config.getMethods().size())
    {
        Directive methods;
        methods.setName("methods");
        methods.addArg("GET");
        methods.addArg("POST");
        methods.addArg("DELETE");
        config.addDirective(methods);
    }
    if (config.getRoot() == "")
    {
        if (server.getRoot() == "")
            throw std::invalid_argument("Error location : A location must have a root directive.");
        else
        {
            std::vector<std::string> root;
            root.push_back(server.getRoot());
            config.setRoot(root);
        }
    }
}

void checkLocationName(const Serverconfig& server)
{
    std::vector<std::string> checkName;
    size_t i = 1;
    size_t j;
    if (server.getLocations().size())
        checkName.push_back(server.getLocations()[0].getArg());
    while (i < server.getLocations().size())
    {
        j = 0;
        while (j < checkName.size())
        {
            if (checkName[j] == server.getLocations()[i].getArg())
                throw std::invalid_argument ("Location configuration error: Two locations cannot have the same directory.");              
            j++;
        }
        checkName.push_back(server.getLocations()[i].getArg());
        i++;
    }
}

void directiveLocation(const Block& block, Locationconfig& locationconfig, const Serverconfig& server)
{
    size_t i = 0;

    while (i < block.getDirectives().size())
    {
        locationconfig.addDirective(block.getDirectives()[i]);
        i++;
    }
    if (block.getArgs().size() != 1)
        throw std::invalid_argument("Error Location : A location block must have exactly one argument.");
    locationconfig.setArg(block.getArgs()[0]);
    defaultLocation(locationconfig, server);
}

void directiveServer(const Block& block, Serverconfig& serverconfig)
{
    size_t i = 0;

    while (i < block.getDirectives().size())
    {
        serverconfig.addDirective(block.getDirectives()[i]);
        i++;
    }
    serverconfig.checkDirective();
    //verifier la presence du minimum listen et page 404
    if (block.getArgs().size() != 0)
        throw std::invalid_argument("Error Server : A server block can't have an argument.");
}

Locationconfig checkLocation(const Block& block, const Serverconfig& server)
{
    Locationconfig locationconfig;
    if (block.getBlocks().size() != 0)
    {
        throw std::invalid_argument("Error locationconfig: invalid block in location.");
    }
    directiveLocation(block, locationconfig, server);
    return (locationconfig);
}

Serverconfig checkServer(const Block& block)
{
    size_t i = 0;
    Serverconfig serverconfig;
    directiveServer(block, serverconfig);
    while (i < block.getBlocks().size())
    {
        if (block.getBlocks()[i].getName() != "location")
            throw  std::invalid_argument("Error serverconfig: invalid block name.");
        else
           serverconfig.addlocation(checkLocation(block.getBlocks()[i], serverconfig));
        i++;
    }
    checkLocationName(serverconfig);
    return (serverconfig);
}

Config checkConfig(const Block& block)
{
    Config config;
    size_t i = 0;

    while (i < block.getBlocks().size())
    {
        if (block.getBlocks()[i].getName() != "server")
            throw  std::invalid_argument("Error serverconfig: invalid server name.");
        else
            config.addServer(checkServer(block.getBlocks()[i]));
        i++;
    }
    return (config);
}