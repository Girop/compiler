#pragma once
#include "ast.hpp"
#include "lexer/token.hpp"
#include "utils.hpp"
#include <bitset>
#include <climits>
#include <cstdint>
#include <vector>

namespace compiler::ast
{

enum class BasicType : uint8_t
{
    SignedChar,  // int8_t
    ShortInt,    // int16_t
    Int,         // int32_t
    LongInt,     // int64_t
    LongLongInt, // Also int64_t on linux
    Float,       // f32
    Double,      // f64
    LongDouble,  // f128
    Bool,        // bool
    Void,        // Void
};

enum class Qualifier : uint8_t
{
    Const,
    Volatile,
    Restrict,
};

enum class Storage : uint8_t
{
    Unspecified,
    Extern,
    Auto,
    Static,
    Register
};

class Type : public Node
{
public:
    using Qualifiers = std::bitset<to_underlying(Qualifier::Restrict) + 1>;

    explicit Type(Loc loc, std::vector<tokens::Keyword>&& keywords);
    virtual std::ostream& stream(std::ostream&) const override;
    void set_default_storage(Storage storage);

private:
    Qualifiers quals_;
    Storage storage_;
    BasicType basic_;
    bool signed_;

    // conversion rank
};

} // namespace compiler::ast
