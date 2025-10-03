#include "../../includes/webserv.hpp"

Block::Block()
{

}

Block::Block(const Block& obj) : _name(obj._name), _args(obj._args), _directives(obj._directives), _blocks(obj._blocks) 
{

}

Block& Block::operator=(const Block& obj)
{
    if (this != &obj)
    {
        _name = obj._name;
         _directives = obj._directives;
         _blocks = obj._blocks;
         _args = obj._args;
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

void Block::addArg(const std::string& arg)
{
    _args.push_back(arg);
}

const std::vector<std::string>& Block::getArgs(void) const
{
    return(_args);
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

