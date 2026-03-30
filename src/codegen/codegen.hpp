#pragma once
#include "cfg.hpp"

namespace compiler::codegen
{

class Codegen
{
public:
    explicit Codegen(ast::TranslationUnit const& tu) : tu_{ tu } {}

    void run();
    std::ostream& assembly(std::ostream& os) const { return os << asm_; }
    std::vector<codegen::CFG> const& ssa() const { return cfgs_; }

private:
    ast::TranslationUnit const& tu_;
    std::vector<codegen::CFG> cfgs_;
    std::string asm_;
};

} // namespace compiler
