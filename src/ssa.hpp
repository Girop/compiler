#pragma once
#include "ast.hpp"

// SSA constructions using algorithm by Braun et al.

namespace compiler
{

// Variables in SSA can be represented as a number alone



struct SSA {};

struct Block
{
    std::vector<SSA> ins;
    std::vector<Block*> succs;
};

class CFG
{
public: 
 
private:
    std::vector<Block> blocks;
};

}
