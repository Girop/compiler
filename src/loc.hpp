#pragma once
#include <cstddef>
#include <ostream>

namespace compiler
{

class Loc
{
// TODO: struct Wrn / Err pattern
public:
    Loc(size_t col, size_t row) : column_(col), row_(row) {}

    friend std::ostream& operator<<(std::ostream&, Loc const& loc);

private:
    size_t column_;
    size_t row_;
};

} // namespace compiler
