#include "parser.hpp"
#include "lexer/reflection.hpp"
#include "type.hpp"
#include <cassert>

namespace compiler
{

bool Parser::match_consume(tokens::Punctuator punct)
{
    auto peeked = lexer_.peek();
    if(auto p = std::get_if<tokens::Punctuator>(&peeked.value);
       p != nullptr && *p == punct)
        {
            lexer_.advance();
            return true;
        }
    return false;
}

void Parser::expect(tokens::Punctuator punct)
{
    if(!match_consume(punct))
        {
            lexer_.loc().err() << "Unexpected token encountered, expected: "
                               << tokens::to_string(punct);
            exit(1);
        }
}

uint8_t Parser::prefix_binding_power(tokens::Punctuator punct) const
{
    using tokens::Punctuator;
    switch (punct)
    {
        case Punctuator::Exclaim:
        case Punctuator::Ampersand:
        case Punctuator::Star:
        case Punctuator::Plus: return 20;
        default: REPORT_ICE("Unexpected punctuator as an infix");
    }
}

std::pair<uint8_t, uint8_t> Parser::binding_power(tokens::Punctuator punct) const
{
    using tokens::Punctuator;
    switch (punct)
    {

        case Punctuator::Plus:
        case Punctuator::Minus: return {10, 11};
        case Punctuator::Star:
        case Punctuator::Slash: return {12, 13};
        default: REPORT_ICE("Unexpected punctuator: " << tokens::to_string(punct));
    }
}


ast::Ptr<ast::TranslationUnit> Parser::parse()
{
    std::vector<ast::Ptr<ast::Declaration>> decls;
    while(true)
        {
            auto decl = declaration();
            if(decl == nullptr) break;
            decls.emplace_back(std::move(decl));
        }
    return std::make_unique<ast::TranslationUnit>(lexer_.loc().filename(),
                                                  std::move(decls));
}

ast::Ptr<ast::Type> Parser::type()
{
    std::vector<tokens::Keyword> specifiers;
    auto const loc = lexer_.peek().loc;

    while(true)
        {
            auto res = lexer_.peek();

            assert(res.tag == tokens::Tag::Keyword);
            auto key = std::get<tokens::Keyword>(res.value);
            switch(key)
                {
                case tokens::Keyword::Typedef:
                    REPORT_ICE("Unsupported featured of typedefs");
                case tokens::Keyword::Register:
                case tokens::Keyword::Extern:
                case tokens::Keyword::Static:
                case tokens::Keyword::Auto:
                case tokens::Keyword::Void:
                case tokens::Keyword::Char:
                case tokens::Keyword::Unsigned:
                case tokens::Keyword::Signed:
                case tokens::Keyword::Inline:
                case tokens::Keyword::Short:
                case tokens::Keyword::Long:
                case tokens::Keyword::Int:
                case tokens::Keyword::Float:
                case tokens::Keyword::Double:
                case tokens::Keyword::Bool:
                    specifiers.emplace_back(key);
                    lexer_.advance();
                    continue;
                default:
                    break;
                }
        }

    if(specifiers.size() == 0)
        {
            lexer_.loc().err()
                << "A type specifier is required for all declarations";
            return nullptr;
        }
    return std::make_unique<ast::Type>(loc, std::move(specifiers));
}

ast::Ptr<ast::Declaration> Parser::declaration()
{
    auto ret_t = type();
    auto iden = identifier();
    auto const loc{ ret_t->loc() };
    if(match_consume(tokens::Punctuator::LParen))
        {
            // TODO Parse parameter list
            std::vector<ast::Ptr<ast::ObjDecl>> args;

            expect(tokens::Punctuator::RParen);
            auto compound = compound_stmt();
            return std::make_unique<ast::FunctionDecl>(
                loc, std::move(ret_t), std::move(iden), std::move(args),
                std::move(compound));
        }

    ast::Ptr<ast::Expr> initalizer{ match_consume(tokens::Punctuator::Equal)
                                        ? expr()
                                        : nullptr };
    expect(tokens::Punctuator::Semicolon);
    return std::make_unique<ast::ObjDecl>(
        loc, std::move(ret_t), std::move(iden), std::move(initalizer));
}

ast::Ptr<ast::Iden> Parser::identifier()
{
    auto token = lexer_.advance();
    if(token.tag != tokens::Tag::Identifier)
        {
            token.loc.err()
                << "Expected identifier, found: " << token.format();
            return nullptr;
        }
    return std::make_unique<ast::Iden>(token.loc,
                                       std::get<std::string>(token.value));
}

ast::Ptr<ast::Stmt> Parser::statement()
{
    return nullptr;
}

ast::Ptr<ast::CompoundStmt> Parser::compound_stmt()
{
    std::vector<ast::Ptr<ast::Stmt>> statements;
    auto const loc{ lexer_.peek().loc };
    expect(tokens::Punctuator::LBrace);
    for(auto stmnt = statement(); stmnt != nullptr; stmnt = statement())
        {
            statements.emplace_back(std::move(stmnt));
        }
    expect(tokens::Punctuator::RBrace);
    return std::make_unique<ast::CompoundStmt>(loc, std::move(statements));
}


ast::Ptr<ast::IntLiteral> Parser::constant()
{
    auto const tok = lexer_.advance();
    assert(tok.tag == tokens::Tag::Constant);

    return std::make_unique<ast::IntLiteral>(tok.loc, std::get<int>(tok.value));
}

ast::Ptr<ast::Expr> Parser::expr(uint8_t min_bp)
{
    auto get_atom = [&] () -> ast::Ptr<ast::Expr> {
        auto const atom {lexer_.peek()};
        switch (atom.tag) 
        {
            case tokens::Tag::Identifier: return identifier();
            case tokens::Tag::Constant: return constant();
            case tokens::Tag::Punctuator:
            {
                auto const p = std::get<tokens::Punctuator>(atom.value);
                return std::make_unique<ast::UnaryExpr>(atom.loc, p, expr(prefix_binding_power(p)));
            }
            default: REPORT_ICE("Unrecognized expression atom");
        }
    };

    auto lhs = get_atom();
    while (true)
    { 
        auto const tok = lexer_.peek();
        assert(tok.tag == tokens::Tag::Punctuator);
        auto const op = std::get<tokens::Punctuator>(tok.value);
        auto const [lbp, rbp] = binding_power(op);
        if (lbp < min_bp) break;
        auto const loc = lhs->loc();
        auto rhs = expr(rbp);
        lhs = std::make_unique<ast::BinExpr>(loc, std::move(lhs), op, std::move(rhs));
    }

    return lhs;
}

} // namespace compiler
