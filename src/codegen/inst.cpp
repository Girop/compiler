#include "inst.hpp"
#include "util/ice.hpp"
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

std::string format_args(std::span<Inst* const> args)
{
    std::string arg_str;

    for (auto* arg : args)
    {
        arg_str += std::to_string(arg->name());

        if (args.back() != arg)
        {
            arg_str += ",";
        }
    }

    return arg_str;
}

} // namespace

std::string Inst::to_string() const
{
    return std::format("{} {} = {}", ::compiler::codegen::to_string(type_), std::to_string(name_), op_string());
}

std::string MathInst::op_string() const
{
    auto ops = [&]
    {
        switch (op())
        {
        case Opcode::Add: return "Add";
        case Opcode::Sub: return "Sub";
        case Opcode::Mul: return "Mul";
        case Opcode::Div: return "Div";
        case Opcode::Cmp: return "Cmp";
        default: REPORT_ICE("Invalid op");
        }
    }();

    return std::format("{}({})", ops, format_args(args_));
}

std::string Phi::op_string() const { return std::format("Phi({})", format_args(args_)); }

std::string Ret::op_string() const { return std::format("Ret({})", format_args(args_)); }

std::string ConstInst::op_string() const { return std::format("Const({})", value_); }

std::string JumpIf::op_string() const { return std::format("JumpIf({})", args_.at(0)->name()); }

std::string Jump::op_string() const { return "Jump()"; }

std::string Set::op_string() const { return std::format("Set({})", args_.at(0)->name()); }

std::string Unary::op_string() const
{
    switch (op())
    {
    case Opcode::LogicalNegate: return std::format("Negate({})", args_.at(0)->name());
    default: REPORT_ICE("Invalid op");
    }
}

} // namespace compiler::codegen
