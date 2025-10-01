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

void displayLocation(const Locationconfig& location)
{
	size_t i = 0;

	std::cout << "	Location {" << std::endl;
	std::cout << "		Root : " << location.getRoot() << std::endl;
	std::cout << "		Index : " << location.getIndex() << std::endl;
	std::cout << "		Autoindex : " << location.getAutoindex() << std::endl;
	std::cout << "		Methods : " ;
	while (i < location.getMethods().size())
	{
		std::cout << location.getMethods()[i] << " ";
		i++;
	}
	std::cout << std::endl;
	std::cout << "		Cgi_pass : " << location.getCgi_pass() << std::endl;
	std::cout << "		Redirect : " << location.getRedirect() << std::endl;
	std::cout << "	}" << std::endl;
}

void displayServer(const Serverconfig& server)
{
	size_t i = 0;

	std::cout << "Server {" << std::endl;
	std::cout << "	Listen : ";
	while (i < server.getListen().size())
	{
		std::cout << server.getListen()[i] << " ";
		i++;
	}
	std::cout << std::endl;
	i = 0;
	std::cout << "	Server_name : ";
	while (i < server.getServer_name().size())
	{
		std::cout << server.getServer_name()[i] << " ";
		i++;
	}
	std::cout << std::endl;
	i = 0;
	std::cout << "	Root : " << server.getRoot() << std::endl;

        // std::map<int, std::string> _error_pages;
	std::cout << "	Max body size : " << server.getClient_max_size() << std::endl;
	i = 0;
	std::cout << server.getLocations().size() << std::endl;
	while (i < server.getLocations().size())
	{
		displayLocation(server.getLocations()[i]);
		i++;
	}
	std::cout << "}" << std::endl;
}

void displayConfig(const Config& config)
{
	size_t i = 0;
	while (i < config.getServers().size())
	{
		displayServer(config.getServers()[i]);
		i++;
	}
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
    block = createMain(token_list, &i);
    display(block);
    config = checkConfig(block);
    displayConfig(config);
    return (0);
}