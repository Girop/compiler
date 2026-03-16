#pragma once
#include <cassert>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>

namespace compiler::codegen
{

enum class Opcode : uint8_t
{
    JumpIf,
    Jump,
    Phi,
    Upsilon,
    Ret,
    Constant,
    Add,
    Sub,
    Mul,
    Div,
    Set,
    Cmp,
    LogicalNegate,
};

enum class Size
{
    Void,
    Int32,
};

using Iden = size_t;

class Inst
{
public:
    virtual ~Inst() = default;

    Opcode& op() { return op_; }
    Iden& name() { return name_; }
    std::span<Inst*> args() { return args_; }
    Size& type() { return type_; }

    template <typename T> T* as()
    {
        static_assert(std::is_base_of_v<Inst, T>, "Invalid downcast");
        return dynamic_cast<T*>(this);
    }

protected:
    Inst(Opcode op, Iden name, std::vector<Inst*> const& args = {}, Size type = Size::Int32) :
        op_{ op },
        name_{ name },
        type_{ type },
        args_{ args }
    {
    }

private:
    Opcode op_;
    Iden name_;
    Size type_;

protected:
    std::vector<Inst*> args_;
};

class MathInst : public Inst
{
public:
    explicit MathInst(Iden name, Opcode op, Inst* lhs, Inst* rhs) : Inst{ op, name, { lhs, rhs }, Size::Int32 } {}
};

class ConstInst : public Inst
{
public:
    explicit ConstInst(Iden name, int64_t value) : Inst(Opcode::Constant, name), value_{ value }
    {
        assert(value <= std::numeric_limits<int32_t>::max() && "Value to large");
    }

    int64_t value() { return value_; }

private:
    int64_t value_;
};

class Phi : public Inst
{
public:
    explicit Phi(Iden name, Iden shadow_id) : Inst{ Opcode::Phi, name, {}, Size::Void }, shadow_{ shadow_id } {}

    Iden shadow() { return shadow_; }

    void append_operand(Inst* operand) { args_.emplace_back(operand); }

private:
    Iden shadow_;
};

class Upsilon : public Inst
{
public:
    explicit Upsilon(Iden name, Iden shadow, Inst* value) :
        Inst{ Opcode::Upsilon, name, { value }, Size::Void },
        shadow_{ shadow }
    {
    }

    Iden shadow() { return shadow_; }

private:
    Iden shadow_;
};

class Jump : public Inst
{
public:
    explicit Jump(Iden name) : Inst{ Opcode::Jump, name, {}, Size::Void } {}
};

class Ret : public Inst
{
public:
    explicit Ret(Iden name, Inst* value = nullptr) :
        Inst{ Opcode::Ret, name,
              [&]
              {
                  std::vector<Inst*> args;
                  if (value) args.emplace_back(value);
                  return args;
              }(),
              Size::Void }
    {
    }
};

class JumpIf : public Inst
{
public:
    explicit JumpIf(Iden name, Inst* cond) : Inst{ Opcode::Jump, name, { cond }, Size::Void } {}
};

class Set : public Inst
{
public:
    explicit Set(Iden name, Inst* value) : Inst{ Opcode::Set, name, { value }, value->type() } {}
};

class Unary : public Inst
{
public:
    explicit Unary(Iden name, Opcode op, Inst* arg) : Inst{ op, name, { arg }, Size::Int32 } {}
};

} // namespace compiler::codegen
