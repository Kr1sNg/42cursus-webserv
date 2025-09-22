#ifndef BLOCK_H
# define BLOCK_H

#include <string>
#include <vector>
#include "Directive.hpp"

class Block :
{
    private :
        std::string _name;
        std::vector<Directive> _directives;
        std::vector<Block> _blocks;
    public :
        Block(const std::string& name);
        Block(const Block& obj);
        Block& operator=(const Block& obj);
        ~Block();

        void setName(std::string name);
        const std::string& getName(void) const;
        void addDirective(const Directive& directive);
        const std::vector<Directive>& getDirectives(void) const;
        void addBlock(const Block& block);
        const std::vector<Block>& getBlocks(void) const;
};

#endif