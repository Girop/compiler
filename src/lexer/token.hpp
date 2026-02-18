#pragma once
#include "loc.hpp"
#include <cstdint>
#include <string>
#include <variant>

namespace compiler::tokens
{

enum class Tag : uint8_t
{
    Keyword,
    Identifier,
    Constant,
    StringLiteral,
    Punctuator,
    EoF,
};

enum class Keyword : uint8_t
{
    Auto,
    Break,
    Case,
    Char,
    Const,
    Continue,
    Default,
    Do,
    Double,
    Else,
    Enum,
    Extern,
    Float,
    For,
    Goto,
    If,
    Inline,
    Int,
    Long,
    Register,
    Restrict,
    Return,
    Short,
    Signed,
    Sizeof,
    Static,
    Struct,
    Switch,
    Typedef,
    Union,
    Unsigned,
    Void,
    Volatile,
    While,
    Bool,
    Complex,
    Imaginary,
};

// Value indexing of this enum starts from 4 because the DFMA states
// corresponding to the Punctuators are later directly cast to this enum
enum class Punctuator : uint8_t
{
    LBracket = 4,        // [
    RBracket,            // ]
    LParen,              // (
    RParen,              // )
    LBrace,              // {
    RBrace,              // }
    Dot,                 // .
    Arrow,               // ->
    PlusPlus,            // ++
    MinusMinus,          // --
    Ampersand,           // &
    Star,                // *
    Plus,                // +
    Minus,               // -
    Tilde,               // ~
    Exclaim,             // !
    Slash,               // /
    Percent,             // %
    LessLess,            // <<
    GreaterGreater,      // >>
    Less,                //
    Greater,             // >
    LessEqual,           // <=
    GreaterEqual,        // >=
    EqualEqual,          // ==
    ExclaimEqual,        // !=
    Caret,               // ^
    Pipe,                // |
    AmpersandAmpersand,  // &&
    PipePipe,            // ||
    Question,            // ?
    Colon,               // :
    Semicolon,           // ;
    Ellipsis,            // ...
    Equal,               // =
    StarEqual,           // *=
    SlashEqual,          // /=
    PercentEqual,        // %=
    PlusEqual,           // +=
    MinusEqual,          // -=
    LessLessEqual,       // <<=
    GreaterGreaterEqual, // >>=
    AmpersandEqual,      // &=
    CaretEqual,          // ^=
    PipeEqual = 48,      // |=
};

struct Token
{
    using Value
        = std::variant<std::monostate, Punctuator, Keyword, std::string, int>;

    Tag tag;
    Value value;
    Loc loc;
};

} // namespace compiler::tokens
