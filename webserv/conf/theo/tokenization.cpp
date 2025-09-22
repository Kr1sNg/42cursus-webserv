#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>

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
    return (0);
}