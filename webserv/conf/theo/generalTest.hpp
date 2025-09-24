#ifndef GEN_H
# define GEN_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include "Directive.hpp"
#include "Block.hpp"

Block  createBlock(std::vector<std::string> list, size_t *i);
void display(const Block& block);
void displayInfos(const Block& block);
#endif