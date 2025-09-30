#include "generalTest.hpp"

void directiveServer(const Block& block, Serverconfig serverconfig)
{
    size_t i = 0;

    while (i < block.getDirectives().size())
    {
        if (serverconfig.validDirective(block.getDirectives()[i].getName()));
        {
            
        }
        i++;
    }
}

Locationconfig checkLocation(const Block& block)
{
    size_t i = 0;

    while (i < block.getBlocks().size())
    {
        i++;
    }
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
            checkLocation(block.getBlocks()[i]);
        
        i++;
    }
    if (block.getBlocks().size() != 0)
        //return error
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
    if (i != block.getBlocks().size())
        //return error
    if (block.getDirectives().size() != 0)
        //return error
}