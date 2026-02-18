#include "lexer.hpp"
#include <cassert>
#include <charconv>
#include <unordered_map>

namespace compiler
{
using tokens::Token;

namespace
{

// TODO try implementing comptime map container
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

Lexer::BufferedPeek Lexer::peek_with_offset() const
{
    Loc new_loc{ loc_ }; // TODO this is fucked up
    size_t new_index{ index_ };

    auto next_char = [&]()
        {
            char c{ file_content_.at(!is_eof() ? new_index++ : '\0') };
            new_loc.advance(c);
            return c;
        };

    DFMAState whitespace_state {DFMAState::Initial};
    while (whitespace_state == DFMAState::Initial || whitespace_state == DFMAState::Whitespace) 
    {
       whitespace_state = table(whitespace_state, next_char());
    }

    size_t const starting_index{ new_index };
    DFMAState result{ DFMAState::Error };
    DFMAState state{ DFMAState::Initial };

    char c = next_char();

    while(true)
        {
            if(is_final(state)) result = state;
            state = table(state, c);
            if(state == DFMAState::Error) break;
            c = next_char();
        }

    assert(result != DFMAState::Initial && "DFMA state is initial");
    assert(result != DFMAState::Error && "DFMA state is error");
    assert(is_final(result) && "Result not final");

    return BufferedPeek{ new_loc, new_index,
                         create(starting_index, new_index, state) };
}

Token Lexer::peek() const
{
    buffered_ = peek_with_offset();
    return buffered_->token;
}

Token Lexer::advance()
{
    if(!buffered_.has_value())
        {
            buffered_ = peek_with_offset();
        }
    loc_ = buffered_->loc;
    index_ = buffered_->index;
    auto const res = buffered_->token;
    buffered_ = std::nullopt;
    return res;
}

tokens::Token Lexer::create(size_t starting, size_t current,
                            DFMAState state) const
{
    auto const lexem = file_content_.substr(starting, current - starting + 1);
    switch(state)
        {
        case DFMAState::Identifier:
            {
                if(auto it = KEYWORDS.find(lexem); it != KEYWORDS.end())
                    {
                        return Token{ tokens::Tag::Keyword, it->second, loc_ };
                    }
                return Token{ tokens::Tag::Keyword, std::string{ lexem },
                              loc_ };
            }

        case DFMAState::Integer:
            {
                int value;
                auto res = std::from_chars(lexem.data(),
                                           lexem.data() + lexem.size(), value);
                assert(res.ec != std::errc{});
                return Token{ tokens::Tag::Constant, value, loc_ };
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
                return Token{
                    tokens::Tag::Punctuator,
                    tokens::Punctuator{ complier::to_underlying(state) }, loc_
                };
            }
        case DFMAState::Initial:
        case DFMAState::Whitespace:
        case DFMAState::Error:
        case DFMAState::DotDot:
        case DFMAState::_Count:
            break;
        }
    REPORT_ICE("Lexer ended in an invalid state");
}

} // namespace compiler
