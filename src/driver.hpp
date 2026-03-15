#pragma once
#include "parser.hpp"
#include "sema.hpp"

namespace compiler
{

struct Flags
{
    std::string filename;
    bool lex{ false };
    bool parse{ false };
    bool ssa{ false };
};

class Driver
{
public:
    explicit Driver(Flags const& flag) : flags_{ flag }, file_{ flag.filename }, parser_{ file_, sema_ } {}

    void compile();
    bool success() const;

private:
    void lexems();
    void analyze(ast::TranslationUnit& tu);

    Flags const flags_;
    File const file_;
    Sema sema_;
    Parser parser_;
};

} // namespace compiler
