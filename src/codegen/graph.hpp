#pragma once
#include "cfg.fwd.hpp"

namespace compiler::codegen
{

// TODO generalize it for AST
struct GraphWriter
{
    static void dump(CFG const&);
};

} // namespace compiler::codegen
