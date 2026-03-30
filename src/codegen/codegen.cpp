#include "codegen.hpp"
#include "codegen/x86_64.hpp"
#include <sstream>

namespace compiler::codegen
{

void Codegen::run()
{
    for (auto& func : tu_.items()->items())
    {
        auto d = func->decl();
        assert(d);
        auto* f = dynamic_cast<ast::FunctionDecl const*>(d);
        assert(f);
        auto& cfg = cfgs_.emplace_back(codegen::CFG::construct(*f));
        cfg.add_labels();
    }

    std::stringstream ss;
    x64::Emitter backend{ cfgs_.at(0), ss };
    asm_ = ss.str();
}

} // namespace compiler::codegen
