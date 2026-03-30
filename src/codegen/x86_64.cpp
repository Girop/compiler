#include "x86_64.hpp"
#include <format>
#include <unordered_map>

namespace compiler::codegen::x64
{

struct RegAlloc
{
    std::unordered_map<int, int> slot;
    int next_slot{};

    int get_slot(int vreg)
    {
        if (!slot.count(vreg)) slot[vreg] = next_slot++;
        return slot[vreg];
    }

    std::string mem(int vreg) { return "[rbp - " + std::to_string((get_slot(vreg) + 1) * 8) + "]"; }

    int frame_size() { return ((next_slot * 8) + 15) & ~15; }
};

void Emitter::line(std::string_view str) { os_ << str << '\n'; }

void Emitter::label(std::string_view str) { line(std::string{ str } + ":"); }

void Emitter::preambule()
{
    line("push rbp");
    line("mov rbp, rsp");
}

void Emitter::epilogue() { line("pop rbp"); }

void Emitter::run()
{
    label(ir_.name());
    preambule();
}

} // namespace compiler::codegen::x64
