#ifndef BLOCK_H
# define BLOCK_H

#include "generalTest.hpp"

class Block
{
    private :
        std::string _name;
        std::vector<std::string> _args;
        std::vector<Directive> _directives;
        std::vector<Block> _blocks;
    public :
        Block();
        Block(const Block& obj);
        Block& operator=(const Block& obj);
        ~Block();

        void setName(std::string name);
        const std::string& getName(void) const;
        void addArg(const std::string& arg);
        const std::vector<std::string>& getArgs(void) const;
        void addDirective(const Directive& directive);
        const std::vector<Directive>& getDirectives(void) const;
        void addBlock(const Block& block);
        const std::vector<Block>& getBlocks(void) const;
};

#endif