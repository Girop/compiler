#pragma once
#include "cfg.hpp"

namespace compiler::codegen::x64
{

class Emitter
{
public:
    explicit Emitter(CFG const& cfg, std::ostream& os) : ir_{ cfg }, os_{ os } {}

    void run();

private:
    void line(std::string_view str);
    void label(std::string_view str);


    void preambule();
    void epilogue();

    CFG const& ir_;
    std::ostream& os_;
};

} // namespace compiler::codegen::x64
