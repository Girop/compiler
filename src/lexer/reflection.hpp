#pragma once
#include "token.hpp"

namespace compiler::tokens
{

constexpr std::string to_string(Tag tag)
{
    switch(tag)
        {

        case Tag::Keyword:
            return "<keyword>";
        case Tag::Identifier:
            return "<identifier>";
        case Tag::Constant:
            return "<constant>";
        case Tag::StringLiteral:
            return "<stringLiteral>";
        case Tag::Punctuator:
            return "<punctuator>";
        case Tag::EoF:
            return "<EoF>";
        }
    return "<unkown>";
}

constexpr std::string to_string(Keyword keyword)
{
    switch(keyword)
        {
        case Keyword::Auto:
            return "auto";
        case Keyword::Break:
            return "break";
        case Keyword::Case:
            return "case";
        case Keyword::Char:
            return "char";
        case Keyword::Const:
            return "const";
        case Keyword::Continue:
            return "continue";
        case Keyword::Default:
            return "default";
        case Keyword::Do:
            return "do";
        case Keyword::Double:
            return "double";
        case Keyword::Else:
            return "else";
        case Keyword::Enum:
            return "enum";
        case Keyword::Extern:
            return "extern";
        case Keyword::Float:
            return "float";
        case Keyword::For:
            return "for";
        case Keyword::Goto:
            return "goto";
        case Keyword::If:
            return "if";
        case Keyword::Inline:
            return "inline";
        case Keyword::Int:
            return "int";
        case Keyword::Long:
            return "long";
        case Keyword::Register:
            return "register";
        case Keyword::Restrict:
            return "restrict";
        case Keyword::Return:
            return "return";
        case Keyword::Short:
            return "short";
        case Keyword::Signed:
            return "signed";
        case Keyword::Sizeof:
            return "sizeof";
        case Keyword::Static:
            return "static";
        case Keyword::Struct:
            return "struct";
        case Keyword::Switch:
            return "switch";
        case Keyword::Typedef:
            return "typedef";
        case Keyword::Union:
            return "union";
        case Keyword::Unsigned:
            return "unsigned";
        case Keyword::Void:
            return "void";
        case Keyword::Volatile:
            return "volatile";
        case Keyword::While:
            return "while";
        case Keyword::Bool:
            return "_Bool";
        case Keyword::Complex:
            return "_Complex";
        case Keyword::Imaginary:
            return "_Imaginary";
        }
    return "<unknown>";
}

constexpr std::string to_string(Punctuator punct)
{
    switch(punct)
        {
        case Punctuator::LBracket:
            return "[";
        case Punctuator::RBracket:
            return "]";
        case Punctuator::LParen:
            return "(";
        case Punctuator::RParen:
            return ")";
        case Punctuator::LBrace:
            return "{";
        case Punctuator::RBrace:
            return "}";
        case Punctuator::Dot:
            return ".";
        case Punctuator::Arrow:
            return "->";
        case Punctuator::PlusPlus:
            return "++";
        case Punctuator::MinusMinus:
            return "--";
        case Punctuator::Ampersand:
            return "&";
        case Punctuator::Star:
            return "*";
        case Punctuator::Plus:
            return "+";
        case Punctuator::Minus:
            return "-";
        case Punctuator::Tilde:
            return "~";
        case Punctuator::Exclaim:
            return "!";
        case Punctuator::Slash:
            return "/";
        case Punctuator::Percent:
            return "%";
        case Punctuator::LessLess:
            return "<<";
        case Punctuator::GreaterGreater:
            return ">>";
        case Punctuator::Less:
            return "<";
        case Punctuator::Greater:
            return ">";
        case Punctuator::LessEqual:
            return "<=";
        case Punctuator::GreaterEqual:
            return ">=";
        case Punctuator::EqualEqual:
            return "==";
        case Punctuator::ExclaimEqual:
            return "!=";
        case Punctuator::Caret:
            return "^";
        case Punctuator::Pipe:
            return "|";
        case Punctuator::AmpersandAmpersand:
            return "&&";
        case Punctuator::PipePipe:
            return "||";
        case Punctuator::Question:
            return "?";
        case Punctuator::Colon:
            return ":";
        case Punctuator::Semicolon:
            return ";";
        case Punctuator::Ellipsis:
            return "...";
        case Punctuator::Equal:
            return "=";
        case Punctuator::StarEqual:
            return "*=";
        case Punctuator::SlashEqual:
            return "/=";
        case Punctuator::PercentEqual:
            return "%=";
        case Punctuator::PlusEqual:
            return "+=";
        case Punctuator::MinusEqual:
            return "-=";
        case Punctuator::LessLessEqual:
            return "<<=";
        case Punctuator::GreaterGreaterEqual:
            return ">>=";
        case Punctuator::AmpersandEqual:
            return "&=";
        case Punctuator::CaretEqual:
            return "^=";
        case Punctuator::PipeEqual:
            return "|=";
        }
    return "<unknown>";
}

} // namespace compiler::tokens
