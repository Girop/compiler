#include "loc.hpp"
#include <format>

namespace compiler
{

std::string Loc::format() const 
{
    return std::format("{}:{}:{}", filename_, row_, column_);
}


void Loc::advance(char c)
{
    if (c == '\n')
    {
        ++row_;
        column_ = 1;
        return;
    } 

    ++column_;
}

}
