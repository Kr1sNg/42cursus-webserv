#include "../../includes/config/generalconfig.hpp"
#include "../../includes/webserv.hpp"

// void defaultLocation(Locationconfig& config)
// {
//     if (config.getMethods() == "")
//     {
//         std::vector<std::string> 
//         if ()
//     }
//     if (config.getRoot() == "")
//     {
//         if ()
//     }
// }

void directiveLocation(const Block& block, Locationconfig& locationconfig)
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
    // defaultLocation(locationconfig);
    //remplir les elements par default et verification de root
}

void directiveServer(const Block& block, Serverconfig& serverconfig)
{
    size_t i = 0;

    while (i < block.getDirectives().size())
    {
        serverconfig.addDirective(block.getDirectives()[i]);
        i++;
    }
    //verifier la presence du minimum listen et page 404
    if (block.getArgs().size() != 0)
        throw std::invalid_argument("Error Server : A server block can't have an argument.");
}

Locationconfig checkLocation(const Block& block)
{
    Locationconfig locationconfig;
    if (block.getBlocks().size() != 0)
    {
        throw std::invalid_argument("Error locationconfig: invalid block in location.");
    }
    directiveLocation(block, locationconfig);
    return (locationconfig);
}

Serverconfig checkServer(const Block& block)
{
    size_t i = 0;
    Serverconfig serverconfig;
    while (i < block.getBlocks().size())
    {
        if (block.getBlocks()[i].getName() != "location")
            throw  std::invalid_argument("Error serverconfig: invalid block name.");
        else
           serverconfig.addlocation(checkLocation(block.getBlocks()[i]));
        
        i++;
    }
    directiveServer(block, serverconfig);
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
    // if (i != block.getBlocks().size())
    //     //return error
    // if (block.getDirectives().size() != 0)
    //     //return error
    return (config);
}