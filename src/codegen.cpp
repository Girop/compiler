#include "codegen.hpp"

namespace compiler
{

std::vector<ssa::CFG> Codegen::ssa()
{
    std::vector<ssa::CFG> cfgs;
    for (auto& func : tu_.items())
    {
        auto d = std::get_if<ast::Ptr<ast::Declaration>>(&func);
        assert(d);
        auto* f = dynamic_cast<ast::FunctionDecl const*>(d->get());
        assert(f);
        cfgs.emplace_back(ssa::CFG::construct(*f));
    }
    return cfgs;
}
} // namespace compiler
