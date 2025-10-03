#include "../../includes/webserv.hpp"

bool checkSpecial(std::string str)
{
    if (str == "{" || str == "}" || str == ";")
        return (false);
    else
        return (true);
}

std::string searchSpecial(std::vector<std::string> list, size_t i)
{
    while (i < list.size() && checkSpecial(list[i]))
        i++;
    return (list[i]);
}

Directive createDirective(std::vector<std::string> list, size_t *i)
{
    Directive directive;

    if (checkSpecial(list[*i]))
    {
        directive.setName(list[*i]);
        (*i)++;
    }
    while (checkSpecial(list[*i]))
    {
        directive.addArg(list[*i]);
        (*i)++;
    }
    return (directive);
}

void infosBlock(Block *block, std::vector<std::string> list, size_t *i)
{

    if (checkSpecial(list[*i]))
    {
        block->setName(list[*i]);
        (*i)++;
    }
    while (checkSpecial(list[*i]))
    {
         block->addArg(list[*i]);
        (*i)++;
    }
    (*i)++;
}

Block createBlock(std::vector<std::string> list, size_t *i)
{
    Block block;
    if (searchSpecial(list, *i) == "{")
        infosBlock(&block, list , i);
    while (*i < list.size() && list[*i] != "}")
    {

        if (searchSpecial(list, *i) == ";")
           block.addDirective(createDirective(list, i));
        else if (searchSpecial(list, *i) == "{")
        {

            block.addBlock(createBlock(list, i));
        }
        else
            std::cout << "error" << std::endl;
        (*i)++;
    }
    return (block);
}

Block createMain(std::vector<std::string> list, size_t *i)
{
    Block block;
    while (*i < list.size() && list[*i] != "}")
    {
        if (searchSpecial(list, *i) == "{")
        {

            block.addBlock(createBlock(list, i));
        }
        else
            std::cout << "error" << std::endl;
        (*i)++;
    }
    return (block);
}

