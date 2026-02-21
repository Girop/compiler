#pragma once
#include "utils.hpp"
#include <array>
#include <cstdint>
#include <limits>

namespace compiler
{
enum class DFMAState : uint8_t
{
    Error = 0,
    Initial,

    // DONT PUT ANY ADDITIONAL MEMBERS ABOVE
    // Punctuators
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

    Whitespace,
    Integer,
    Identifier,

    DotDot,
    _Count
};

constexpr bool is_final(DFMAState state)
{
    switch(state)
        {
        case DFMAState::Identifier:
        case DFMAState::Integer:
        case DFMAState::LBracket:
        case DFMAState::RBracket:
        case DFMAState::LParen:
        case DFMAState::RParen:
        case DFMAState::LBrace:
        case DFMAState::RBrace:
        case DFMAState::Dot:
        case DFMAState::Arrow:
        case DFMAState::PlusPlus:
        case DFMAState::MinusMinus:
        case DFMAState::Ampersand:
        case DFMAState::Star:
        case DFMAState::Plus:
        case DFMAState::Minus:
        case DFMAState::Tilde:
        case DFMAState::Exclaim:
        case DFMAState::Slash:
        case DFMAState::Percent:
        case DFMAState::LessLess:
        case DFMAState::GreaterGreater:
        case DFMAState::Less:
        case DFMAState::Greater:
        case DFMAState::LessEqual:
        case DFMAState::GreaterEqual:
        case DFMAState::EqualEqual:
        case DFMAState::ExclaimEqual:
        case DFMAState::Caret:
        case DFMAState::Pipe:
        case DFMAState::AmpersandAmpersand:
        case DFMAState::PipePipe:
        case DFMAState::Question:
        case DFMAState::Colon:
        case DFMAState::Semicolon:
        case DFMAState::Ellipsis:
        case DFMAState::Equal:
        case DFMAState::StarEqual:
        case DFMAState::SlashEqual:
        case DFMAState::PercentEqual:
        case DFMAState::PlusEqual:
        case DFMAState::MinusEqual:
        case DFMAState::LessLessEqual:
        case DFMAState::GreaterGreaterEqual:
        case DFMAState::AmpersandEqual:
        case DFMAState::CaretEqual:
        case DFMAState::PipeEqual:
            return true;
        default:
            break;
        }
    return false;
}

class DFMATable
{
    using Storage
        = std::array<std::array<DFMAState, std::numeric_limits<char>::max()>,
                     to_underlying(DFMAState::_Count)>;

public:
    consteval DFMATable()
    {
        fill_whitespace();
        fill_identifiers();
        fill_punctuators();
        fill_constants();
    }

    constexpr DFMAState operator()(DFMAState state, char c) const
    {
        return storage_.at(to_underlying(state)).at(c);
    }

private:
    consteval void fill_whitespace()
    {
        fill(DFMAState::Initial, { ' ', '\t', '\n', '\v', '\f', '\r' },
             DFMAState::Whitespace);
        fill(DFMAState::Whitespace, { ' ', '\t', '\n', '\v', '\f', '\r' },
             DFMAState::Whitespace);
    }

    consteval void fill_constants()
    {
        fill(DFMAState::Initial, '0', '9', DFMAState::Integer);
        fill(DFMAState::Integer, '0', '9', DFMAState::Integer);
    }

    consteval void fill_punctuators()
    {
        fill(DFMAState::Initial, '[', DFMAState::LBracket);
        fill(DFMAState::Initial, ']', DFMAState::RBracket);
        fill(DFMAState::Initial, '(', DFMAState::LParen);
        fill(DFMAState::Initial, ')', DFMAState::RParen);
        fill(DFMAState::Initial, '{', DFMAState::LBrace);
        fill(DFMAState::Initial, '}', DFMAState::RBrace);
        fill(DFMAState::Initial, '.', DFMAState::Dot);
        fill(DFMAState::Initial, '~', DFMAState::Tilde);
        fill(DFMAState::Initial, '?', DFMAState::Question);
        fill(DFMAState::Initial, ':', DFMAState::Colon);
        fill(DFMAState::Initial, ';', DFMAState::Semicolon);

        // Multi-character operators starting with '-'
        fill(DFMAState::Initial, '-', DFMAState::Minus);
        fill(DFMAState::Minus, '>', DFMAState::Arrow);
        fill(DFMAState::Minus, '-', DFMAState::MinusMinus);
        fill(DFMAState::Minus, '=', DFMAState::MinusEqual);

        // Multi-character operators starting with '+'
        fill(DFMAState::Initial, '+', DFMAState::Plus);
        fill(DFMAState::Plus, '+', DFMAState::PlusPlus);
        fill(DFMAState::Plus, '=', DFMAState::PlusEqual);

        // Multi-character operators starting with '&'
        fill(DFMAState::Initial, '&', DFMAState::Ampersand);
        fill(DFMAState::Ampersand, '&', DFMAState::AmpersandAmpersand);
        fill(DFMAState::Ampersand, '=', DFMAState::AmpersandEqual);

        // Multi-character operators starting with '*'
        fill(DFMAState::Initial, '*', DFMAState::Star);
        fill(DFMAState::Star, '=', DFMAState::StarEqual);

        // Multi-character operators starting with '!'
        fill(DFMAState::Initial, '!', DFMAState::Exclaim);
        fill(DFMAState::Exclaim, '=', DFMAState::ExclaimEqual);

        // Multi-character operators starting with '/'
        fill(DFMAState::Initial, '/', DFMAState::Slash);
        fill(DFMAState::Slash, '=', DFMAState::SlashEqual);

        // Multi-character operators starting with '%'
        fill(DFMAState::Initial, '%', DFMAState::Percent);
        fill(DFMAState::Percent, '=', DFMAState::PercentEqual);

        // Multi-character operators starting with '<'
        fill(DFMAState::Initial, '<', DFMAState::Less);
        fill(DFMAState::Less, '<', DFMAState::LessLess);
        fill(DFMAState::Less, '=', DFMAState::LessEqual);
        fill(DFMAState::LessLess, '=', DFMAState::LessLessEqual);

        // Multi-character operators starting with '>'
        fill(DFMAState::Initial, '>', DFMAState::Greater);
        fill(DFMAState::Greater, '>', DFMAState::GreaterGreater);
        fill(DFMAState::Greater, '=', DFMAState::GreaterEqual);
        fill(DFMAState::GreaterGreater, '=', DFMAState::GreaterGreaterEqual);

        // Multi-character operators starting with '^'
        fill(DFMAState::Initial, '^', DFMAState::Caret);
        fill(DFMAState::Caret, '=', DFMAState::CaretEqual);

        // Multi-character operators starting with '|'
        fill(DFMAState::Initial, '|', DFMAState::Pipe);
        fill(DFMAState::Pipe, '|', DFMAState::PipePipe);
        fill(DFMAState::Pipe, '=', DFMAState::PipeEqual);

        // Multi-character operators starting with '='
        fill(DFMAState::Initial, '=', DFMAState::Equal);
        fill(DFMAState::Equal, '=', DFMAState::EqualEqual);

        // Ellipsis (...)
        fill(DFMAState::Dot, '.', DFMAState::DotDot);
        fill(DFMAState::DotDot, '.', DFMAState::Ellipsis);
    }

    consteval void fill_identifiers()
    {
        fill(DFMAState::Initial, 'a', 'z', DFMAState::Identifier);
        fill(DFMAState::Initial, 'A', 'Z', DFMAState::Identifier);
        fill(DFMAState::Initial, { '_' }, DFMAState::Identifier);

        fill(DFMAState::Identifier, 'a', 'z', DFMAState::Identifier);
        fill(DFMAState::Identifier, 'A', 'Z', DFMAState::Identifier);
        fill(DFMAState::Identifier, '_', DFMAState::Identifier);
        fill(DFMAState::Identifier, '0', '9', DFMAState::Identifier);
    }

    consteval void fill(DFMAState state, char c, DFMAState value)
    {
        storage_.at(to_underlying(state)).at(c) = value;
    }

    consteval void fill(DFMAState state, std::initializer_list<char> elements,
                        DFMAState value)
    {
        for(auto&& c : elements)
            fill(state, c, value);
    }

    consteval void fill(DFMAState state, char from, char to, DFMAState value)
    {
        for(char c = from; c < to; ++c)
            {
                storage_.at(to_underlying(state)).at(c) = value;
            }
    }

    Storage storage_{};
};

} // namespace compiler
