#pragma once
#include "ast.hpp"
#include "type.hpp"
#include "lexer/lexer.hpp"

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

    ast::Ptr<ast::Expr> expr();
    ast::Ptr<ast::Stmt> statement();
    ast::Ptr<ast::CompoundStmt> compound_stmt();
    ast::Ptr<ast::Iden> identifier();
    ast::Ptr<ast::Type> type();

    bool match_consume(tokens::Punctuator punct);
    void expect(tokens::Punctuator punct);

    Lexer lexer_;
};

} // namespace compiler
