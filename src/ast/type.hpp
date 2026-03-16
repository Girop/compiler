#pragma once
#include "lexer/token.hpp"
#include "util/underlying.hpp"
#include <bitset>
#include <climits>
#include <cstdint>
#include <vector>

namespace compiler
{

enum class BasicType : uint8_t
{
    SignedChar,
    ShortInt,
    Int,
    LongInt,
    LongLongInt,
    Float,
    Double,
    LongDouble,
    Bool,
    Void,
};

enum class Qualifier : uint8_t
{
    Const,
    Volatile,
    Restrict,
};

class Type
{
public:
    using Qualifiers = std::bitset<to_underlying(Qualifier::Restrict) + 1>;

    explicit Type(Loc loc, std::vector<tokens::Keyword>&& keywords);
    explicit Type(BasicType basic) : basic_{ basic } {}

    bool operator==(Type const&) const = default;
    BasicType basic_type() { return basic_; }

    std::string format() const;

    bool is_artithmetic() const;
    bool is_scalar() const;
    bool is_void() const { return basic_ == BasicType::Void; }
    bool is_modifyable_lvalue() const { return !is_void() && quals_.test(to_underlying(Qualifier::Const)); }

    static Type implicit_conversion(Loc const& loc, Type const& lhs, Type const& rhs);

private:
    Qualifiers quals_;
    BasicType basic_;
    bool signed_{ true };
};

} // namespace compiler
