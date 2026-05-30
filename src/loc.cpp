#include "loc.hpp"
#include <format>

namespace compiler
{

std::string Loc::format() const { return std::format("{}:{}:{}", filename_, position_.line, position_.column); }

void Position::advance(char c)
{
    if (c == '\n')
    {
        ++line;
        column = 1;
    }
    else
    {
        ++column;
    }
}

} // namespace compiler
