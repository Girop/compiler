#include "token.hpp"
#include "reflection.hpp"
#include <format>

namespace compiler::tokens
{
namespace
{

std::string const_to_string(Token::Value const& val)
{
    return std::to_string(std::get<int>(val));
}

} // namespace

std::string Token::format() const
{
    switch(tag)
        {
        case Tag::Keyword:
            return to_string(std::get<Keyword>(value));
        case Tag::Identifier:
        case Tag::StringLiteral: // TODO ensure that this is enquoteted
            return std::get<std::string>(value);
        case Tag::Punctuator:
            return to_string(std::get<Punctuator>(value));
        case Tag::Constant:
            return const_to_string(value);
        case Tag::EoF:
            return "<EoF>";
        }
    return "<unknown>";
}

std::string Token::specific_format() const
{
    return std::format("{}:{}:{}", loc.format(), to_string(tag), format());
}

} // namespace compiler::tokens
