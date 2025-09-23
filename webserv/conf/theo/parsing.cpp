#include <vector>
#include <string>
#include "Block.hpp"
#include "Directive.hpp"

bool checkSpecial(std::string str)
{
    if (str == "{" || str == "}" || str == ";")
        return (false);
    else
        return (true);
}

Directive createDirective(std::vector<std::string> list, size_t *i)
{
    Directive directive;
    if (checkSpecial(list[*i]))
    {
        directive.setName(list[*i]);
        *i++;
    }
    while (checkSpecial(list[*i]))
    {
        directive.addArg(list[*i]);
        *i++;
    }
    return (directive);
}

Block  parsing(std::vector<std::string> list, size_t i)
{
    int check_arg = 0;

    Block block;
    Directive directive();
    while (list[i] != "}")
    {
        if (list[i] == "{")
            block.addBlock(parsing(list, i));
        else if (list [i] != ";")
           block.addDirective(createDirective(list, &i));
        i++;
    }
    return (block);
}