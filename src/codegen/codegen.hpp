#pragma once
#include "cfg.hpp"

namespace compiler
{

class Codegen
{
public:
    explicit Codegen(ast::TranslationUnit const& tu) : tu_{ tu } {}

    std::vector<codegen::CFG> ssa();

private:
    ast::TranslationUnit const& tu_;
};

} // namespace compiler
