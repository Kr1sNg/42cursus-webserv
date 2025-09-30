#include "generalTest.hpp"

void displayInfos(const Block& block)
{
    size_t j = 0;

    std::cout << std::endl;
    std::cout << "Block : " << block.getName();
    while (j < block.getArgs().size())
    {
        std::cout << " " << block.getArgs()[j];
        j++;
    }
    std::cout << " {" << std::endl;
}

void display(const Block& block)
{
    size_t i = 0;
    size_t j = 0;

    while (i < block.getDirectives().size())
    {
        std::cout << "Directive : " << block.getDirectives()[i].getName();
        j = 0;
        while (j < block.getDirectives()[i].getArgs().size())
        {
            std::cout << " " << block.getDirectives()[i].getArgs()[j];
            j++;
        }
        std::cout << std::endl;
        i++;
    }
    i = 0;
    while (i < block.getBlocks().size())
    {
        displayInfos(block.getBlocks()[i]);
        display(block.getBlocks()[i]);
        std::cout << "}" << std::endl;
        i++;
    }
}

void displayConfig(const Config& config)
{
    
}

void display_list(std::vector<std::string> list)
{
       size_t i = 0;

       while (i < list.size())
       {
            std::cout << list[i] << std::endl;
            i++;
       }
}

void tokenization(std::vector<std::string> &token_list, std::string &token)
{
    if (!token.empty())
    {
        token_list.push_back(token);
        token.clear();
    }
}

int main(int ac, char **av)
{
    std::vector<std::string> token_list;
    std::ifstream file(av[1]);
    std::string line;
    std::string token;
    size_t i;
    Block block;
    Config config;
    if (ac != 2)
        std::cout << "Eroor : not enough arguments" << std::endl;
    if (!file.is_open())
    {
        std::cout << "Error : failed open file" << std::endl;
        return (1);
    }
    while (std::getline(file, line))
    {
        i = 0;
        while (i < line.size())
        {
            if (line[i] == '#')
                break;
            else if (isspace(line[i]))
            {
                tokenization (token_list, token);
                i++;
                continue;
            }
            else if (line[i] == '{' || line[i] == '}' || line[i] == ';')
            {
                tokenization(token_list, token);
                token_list.push_back(std::string(1, line[i]));
                i++;
                continue;
            }   
            else
                token += line[i];

            i++;
        }
        tokenization (token_list, token);
    }
    i = 0;
    // display_list(token_list);
    // createBlock(token_list, &i);
    block = createBlock(token_list, &i);
    display(block);
    config = checkConfig(block);
    displayConfig(config);
    return (0);
}