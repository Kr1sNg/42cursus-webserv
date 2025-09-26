#include "generalTest.hpp"

void checkLocation(const Block& block)
{
    size_t i = 0;

    while (i < block.getBlocks().size())
    {
        i++;
    }
}

void checkServer(const Block& block)
{
    size_t i = 0;

    while (i < block.getBlocks().size())
    {
        if (block.getBlocks()[i].getName() != "location")
            //return error
        else
            checkLocation(block.getBlocks()[i]);
        
        i++;
    }
    if (block.getBlocks().size() != 0)
        //return error
}

void checkConfig(const Block& block)
{
    size_t i = 0;

    while (i < block.getBlocks().size())
    {
        if (block.getBlocks()[i].getName() != "server")
            //return error
        else
            checkServer(block.getBlocks()[i]);
        i++;
    }
    if (block.getBlocks().size() != 0)
        //return error
}