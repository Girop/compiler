#pragma once
#include "parser.hpp"

namespace compiler
{

struct Flags
{
    std::string filename;
    bool lex{false};
    bool parse{false};
};

class Driver
{
public:
    explicit Driver(Flags const& flag) :
        flags_{flag},
        file_{ flag.filename },
        parser_{ file_ }
    {
    }

    void compile();
private:
    void lexems();

    Flags const flags_;
    File const file_;
    Parser parser_;
};

} // namespace compiler
