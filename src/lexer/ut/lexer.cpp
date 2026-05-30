#include "lexer/lexer.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace compiler
{

using namespace ::testing;

compiler::File make_file(std::string content) { return compiler::File{ "<test>", std::move(content) }; }

class LexerTest : public Test
{
protected:
    Lexer make(std::string const& src)
    {
        files_.push_back({ "<test>", src });
        return Lexer(files_.back());
    }

    std::vector<tokens::Token> lex_all(std::string const& src)
    {
        auto lex = make(src);
        std::vector<tokens::Token> out;
        while (true)
        {
            auto t = lex.advance();
            out.push_back(t);
            if (t.tag == tokens::Tag::EoF) break;
        }
        return out;
    }

private:
    std::vector<compiler::File> files_;
};

TEST_F(LexerTest, PeekIsIdempotent)
{
    auto lex = make("foo");
    EXPECT_EQ(lex.peek(), lex.peek());
}

TEST_F(LexerTest, AdvanceReturnsPeekedToken)
{
    auto lex = make("foo");
    auto peeked = lex.peek();
    EXPECT_EQ(lex.advance(), peeked);
}

TEST_F(LexerTest, EmptyInputIsEof)
{
    auto lex = make("");
    EXPECT_EQ(lex.advance().tag, tokens::Tag::EoF);
}

TEST_F(LexerTest, UnknownSymbolProducesError)
{
    Lexer lex(make_file("@"));
    auto tok = lex.advance();
    EXPECT_EQ(tok.tag, tokens::Tag::Error);
}

TEST_F(LexerTest, LocationTracking)
{
    Lexer lex(make_file("ab\nc"));
    auto const inital_loc = lex.loc();

    lex.advance();
    lex.advance();
    lex.advance();

    auto const loc = lex.loc();
    auto expected_loc = [](Loc const& baseline)
    {
        auto pos = baseline.pos();
        pos.column = 1;
        pos.line = 2;
        return Loc {baseline.filename(), pos.line, pos.column};
    }(inital_loc);

    EXPECT_EQ(expected_loc, loc);
}

} // namespace compiler
