#include "lexer.hpp"
#include <cassert>
#include <charconv>
#include <unordered_map>

namespace compiler
{
using tokens::Token;

namespace
{

// TODO try implementing comptime map container / string interning
static const std::unordered_map<std::string_view, tokens::Keyword> KEYWORDS{
    { "auto", tokens::Keyword::Auto },
    { "break", tokens::Keyword::Break },
    { "case", tokens::Keyword::Case },
    { "char", tokens::Keyword::Char },
    { "const", tokens::Keyword::Const },
    { "continue", tokens::Keyword::Continue },
    { "default", tokens::Keyword::Default },
    { "do", tokens::Keyword::Do },
    { "double", tokens::Keyword::Double },
    { "else", tokens::Keyword::Else },
    { "enum", tokens::Keyword::Enum },
    { "extern", tokens::Keyword::Extern },
    { "float", tokens::Keyword::Float },
    { "for", tokens::Keyword::For },
    { "goto", tokens::Keyword::Goto },
    { "if", tokens::Keyword::If },
    { "inline", tokens::Keyword::Inline },
    { "int", tokens::Keyword::Int },
    { "long", tokens::Keyword::Long },
    { "register", tokens::Keyword::Register },
    { "restrict", tokens::Keyword::Restrict },
    { "return", tokens::Keyword::Return },
    { "short", tokens::Keyword::Short },
    { "signed", tokens::Keyword::Signed },
    { "sizeof", tokens::Keyword::Sizeof },
    { "static", tokens::Keyword::Static },
    { "struct", tokens::Keyword::Struct },
    { "switch", tokens::Keyword::Switch },
    { "typedef", tokens::Keyword::Typedef },
    { "union", tokens::Keyword::Union },
    { "unsigned", tokens::Keyword::Unsigned },
    { "void", tokens::Keyword::Void },
    { "volatile", tokens::Keyword::Volatile },
    { "while", tokens::Keyword::While },
    { "_Bool", tokens::Keyword::Bool },
    { "_Complex", tokens::Keyword::Complex },
    { "_Imaginary", tokens::Keyword::Imaginary },
};

} // namespace

Lexer::FilePos Lexer::skip_whitespace() const
{
    FilePos new_pos = position_;
    DFMAState state{ DFMAState::Initial };
    while (true)
    {
        if (is_eof(new_pos)) break;
        char c{ peek_char(new_pos) };
        state = table(state, c);
        if (state != DFMAState::Whitespace) break;
        new_pos.advance(c);
    }
    return new_pos;
}

Lexer::BufferedPeek Lexer::peek_with_offset() const
{
    FilePos new_position{ skip_whitespace() };
    if (is_eof(new_position))
    {
        return BufferedPeek{ position_, { tokens::Tag::EoF, std::monostate{}, position_.loc } };
    }

    FilePos const starting{ new_position };

    DFMAState result{ DFMAState::Error };
    DFMAState state{ DFMAState::Initial };

    while (true)
    {
        char c{ peek_char(new_position) };
        if (is_final(state)) result = state;
        state = table(state, c);
        if (state == DFMAState::Error) break;
        new_position.advance(c);
    }

    assert(result != DFMAState::Initial && "DFMA state is initial");
    assert(result != DFMAState::Error && "DFMA state is error");
    assert(is_final(result) && "Result not final");

    return BufferedPeek{ new_position, create(starting, new_position.index, result) };
}

Token Lexer::peek() const
{
    buffered_ = peek_with_offset();
    return buffered_->token;
}

Token Lexer::advance()
{
    if (!buffered_.has_value())
    {
        buffered_ = peek_with_offset();
    }

    position_ = buffered_->pos;
    auto const res = buffered_->token;
    buffered_ = std::nullopt;
    return res;
}

tokens::Token Lexer::create(FilePos const& start, size_t current, DFMAState state) const
{
    auto const lexem = file_content_.substr(start.index, current - start.index);
    switch (state)
    {
    case DFMAState::Identifier:
    {
        if (auto it = KEYWORDS.find(lexem); it != KEYWORDS.end())
        {
            return Token{ tokens::Tag::Keyword, it->second, start.loc };
        }
        return Token{ tokens::Tag::Identifier, std::string{ lexem }, start.loc };
    }

    case DFMAState::Integer:
    {
        int value;
        auto res = std::from_chars(lexem.data(), lexem.data() + lexem.size(), value);
        assert(res.ec == std::errc{});
        return Token{ tokens::Tag::Constant, value, start.loc };
    }
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
    {
        return Token{ tokens::Tag::Punctuator, tokens::Punctuator{ to_underlying(state) }, start.loc };
    }
    case DFMAState::Initial:
    case DFMAState::Whitespace:
    case DFMAState::Error:
    case DFMAState::DotDot:
    case DFMAState::_Count: break;
    }
    REPORT_ICE("Lexer ended in an invalid state: " << static_cast<int>(state));
}

} // namespace compiler
