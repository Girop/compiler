#pragma once
#include "ast.hpp"
#include "lexer/lexer.hpp"
#include "lexer/reflection.hpp"

namespace compiler
{

class Parser
{
public:
    explicit Parser(File const& file, Sema& sema) : lexer_{ file }, sema_{ sema } {}

    ast::Ptr<ast::TranslationUnit> parse();

private:
    ast::Ptr<ast::Declaration> declaration();

    ast::Ptr<ast::Expr> unary_expr();
    ast::Ptr<ast::Expr> expr(int8_t min_bp = 0);
    ast::Ptr<ast::Stmt> statement();
    ast::Ptr<ast::Stmt> selection_statement();
    ast::Ptr<ast::Items> items();
    ast::Ptr<ast::CompoundStmt> compound_stmt();
    ast::Ptr<ast::Iden> identifier();
    ast::Ptr<ast::TypeDecl> type();
    ast::Ptr<ast::IntLiteral> constant();

    ast::Storage storage(std::vector<tokens::Keyword> const& keywords) const;

    template <typename T> bool match(T expected) const
    {
        auto const peeked = lexer_.peek();
        if (auto p = std::get_if<T>(&peeked.value); p != nullptr && *p == expected)
        {
            return true;
        }
        return false;
    }

    bool match(tokens::Tag expected) const { return lexer_.peek().tag == expected; }

    template <typename T> bool match_consume(T tok)
    {
        if (match(tok))
        {
            lexer_.advance();
            return true;
        }
        return false;
    }

    template <typename T> void expect(T tok)
    {
        if (!match_consume(tok))
        {
            lexer_.loc().err() << "Unexpected token encountered, expected: " << tokens::to_string(tok) << '\n';
            exit(1);
        }
    }

    bool is_type_keyword(tokens::Keyword k) const;
    int8_t prefix_binding_power(tokens::Punctuator punct) const;
    std::pair<int8_t, int8_t> binding_power(tokens::Punctuator punct) const;

    Lexer lexer_;
    Sema& sema_;
};

} // namespace compiler
