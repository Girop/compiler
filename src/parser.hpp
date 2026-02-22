#pragma once
#include "ast.hpp"
#include "lexer/lexer.hpp"
#include "type.hpp"

namespace compiler
{

class Parser
{
public:
    explicit Parser(File const& file) :
        lexer_{ file }
    {
    }

    ast::Ptr<ast::TranslationUnit> parse();

private:
    ast::Ptr<ast::Declaration> declaration();

    ast::Ptr<ast::Expr> expr(uint8_t min_bp = 0);
    ast::Ptr<ast::Stmt> statement();
    ast::Ptr<ast::CompoundStmt> compound_stmt();
    ast::Ptr<ast::Iden> identifier();
    ast::Ptr<ast::Type> type();
    ast::Ptr<ast::IntLiteral> constant();

    bool match_consume(tokens::Punctuator punct);
    void expect(tokens::Punctuator punct);

    uint8_t prefix_binding_power(tokens::Punctuator punct) const;
    std::pair<uint8_t, uint8_t> binding_power(tokens::Punctuator punct) const;

    Lexer lexer_;
};

} // namespace compiler
