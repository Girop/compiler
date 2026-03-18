#include "inst.hpp"
#include <format>

namespace compiler::codegen
{

namespace
{
std::string to_string(Size sz)
{
    switch (sz)
    {
    case Size::Void: return "Void";
    case Size::Int32: return "Int32";
    }
    return "Unknown";
}

std::string format_inst_ref(Inst const* arg) { return std::format("v{}", arg->name()); }

std::string format_args(std::span<Inst* const> args)
{
    std::string arg_str;

    for (auto* arg : args)
    {
        arg_str += format_inst_ref(arg);
        if (args.back() != arg) arg_str += ", ";
    }

    return arg_str;
}

} // namespace

std::string Inst::to_string() const
{
    return std::format("{} {} = {}({})", ::compiler::codegen::to_string(type_), format_inst_ref(this),
                       ::compiler::codegen::op_string(op_), format_args(args_));
}

std::string ConstInst::to_string() const
{
    return std::format("{} {} = Cons({})", ::compiler::codegen::to_string(type()), format_inst_ref(this), value_);
}

} // namespace compiler::codegen
