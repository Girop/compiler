#include "codegen.hpp"

namespace compiler
{

std::vector<codegen::CFG> Codegen::ssa()
{
    std::vector<codegen::CFG> cfgs;
    for (auto& func : tu_.items()->items())
    {
        auto d = func->decl();
        assert(d);
        auto* f = dynamic_cast<ast::FunctionDecl const*>(d);
        assert(f);
        cfgs.emplace_back(codegen::CFG::construct(*f));
    }
    return cfgs;
}
} // namespace compiler
