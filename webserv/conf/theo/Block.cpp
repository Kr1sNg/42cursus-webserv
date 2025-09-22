#include "Block.hpp"
#include <string>

Block::Block(const std::string& name) : _name(name)
{

}

Block::Block(const Block& obj) : _name(obj._name), _directives(obj._directives)
{

}

Block& Block::operator=(const Block& obj)
{
    if (this != &obj)
    {
        _name = obj._name;
         _directives = obj._directives;
    }
    return (*this);
}

Block::~Block()
{

}

void Block::setName(std::string name)
{
    _name = name;
}

const std::string& Block::getName(void) const
{
    return (_name);
}

void Block::addDirective(const Directive& directive)
{
    _directives.push_back(directive);
}

const std::vector<Directive>& Block::getDirectives(void) const
{
    return(_directives);
}

void Block::addBlock(const Block& block)
{
    _blocks.push_back(block);
}

const std::vector<Block>& Block::getBlocks(void) const
{
    return (_blocks);
}

