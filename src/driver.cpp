#include "driver.hpp"

namespace compiler
{

void Driver::lexems()
{
    Lexer lexer{ file_ };

    for (auto tok = lexer.advance(); tok.tag != compiler::tokens::Tag::EoF; tok = lexer.advance())
    {
        std::cout << tok.specific_format() << '\n';
    }
}

void Driver::compile()
{
    if (flags_.lex)
    {
        lexems();
    }

    if (flags_.parse)
    {
        auto tu = parser_.parse();
        tu->dump();
    }
}

} // namespace compiler
