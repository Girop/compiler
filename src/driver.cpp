#include "driver.hpp"
#include "codegen.hpp"

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

    auto tu = parser_.parse();
    if (flags_.parse)
    {
        tu->dump();
    }
    analyze(*tu);
    if (!success()) return;

    Codegen codegen{ *tu };
    if (flags_.ssa)
    {
        auto ssa = codegen.ssa();
        ssa[0].dump();
    }
}

void Driver::analyze(ast::TranslationUnit& tu) { tu.check(sema_); }

bool Driver::success() const { return !Loc::has_error(); }

} // namespace compiler
