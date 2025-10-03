#include "../../includes/config/generalconfig.hpp"
#include "../../includes/webserv.hpp"

void directiveLocation(const Block& block, Locationconfig& locationconfig)
{
    size_t i = 0;

    if (i < block.getDirectives().size())
    {
        locationconfig.addDirective(block.getDirectives()[i]);
        i++;
    }
}

void directiveServer(const Block& block, Serverconfig& serverconfig)
{
    size_t i = 0;

    while (i < block.getDirectives().size())
    {
        serverconfig.addDirective(block.getDirectives()[i]);
        i++;
    }
}

Locationconfig checkLocation(const Block& block)
{
    Locationconfig locationconfig;
    // if (block.getBlocks().size() != 0)
    // {
    //     //error
    // }
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
            break;
            //return error
        else
           serverconfig.addlocation(checkLocation(block.getBlocks()[i]));
        
        i++;
    }
    // if (block.getBlocks().size() != 0)
    //     //return error
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
            break;
            //return error
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