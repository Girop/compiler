#include "parser.hpp"
#include "sema.hpp"
#include "util/ice.hpp"
#include <cassert>

namespace compiler
{

int8_t Parser::prefix_binding_power(tokens::Punctuator punct) const
{
    using tokens::Punctuator;
    switch (punct)
    {
    case Punctuator::PlusPlus:
    case Punctuator::MinusMinus:
    case Punctuator::Exclaim:
    case Punctuator::Ampersand:
    case Punctuator::Star:
    case Punctuator::Plus: return 100;
    default: return -1;
    }
}

std::pair<int8_t, int8_t> Parser::binding_power(tokens::Punctuator punct) const
{
    using tokens::Punctuator;
    switch (punct)
    {
    case Punctuator::Equal:
    case Punctuator::StarEqual:
    case Punctuator::SlashEqual:
    case Punctuator::PercentEqual:
    case Punctuator::PlusEqual:
    case Punctuator::MinusEqual:
    case Punctuator::LessLessEqual:
    case Punctuator::GreaterGreaterEqual:
    case Punctuator::AmpersandEqual:
    case Punctuator::CaretEqual:
    case Punctuator::PipeEqual: return { 5, 4 };
    case Punctuator::PipePipe: return { 6, 7 };
    case Punctuator::AmpersandAmpersand: return { 8, 9 };
    case Punctuator::Pipe: return { 10, 11 };
    case Punctuator::Caret: return { 12, 13 };
    case Punctuator::Ampersand: return { 14, 15 };
    case Punctuator::EqualEqual:
    case Punctuator::ExclaimEqual: return { 16, 17 };
    case Punctuator::Less:
    case Punctuator::Greater:
    case Punctuator::LessEqual:
    case Punctuator::GreaterEqual: return { 18, 19 };
    case Punctuator::GreaterGreater:
    case Punctuator::LessLess: return { 20, 21 };
    case Punctuator::Plus:
    case Punctuator::Minus: return { 22, 23 };
    case Punctuator::Star:
    case Punctuator::Slash:
    case Punctuator::Percent: return { 24, 25 };
    default: return { -1, -1 };
    }
}

ast::Ptr<ast::TranslationUnit> Parser::parse()
{
    return std::make_unique<ast::TranslationUnit>(lexer_.loc().filename(), items());
}

bool Parser::is_type_keyword(tokens::Keyword k) const
{
    switch (k)
    {
    case tokens::Keyword::Typedef:
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
    case tokens::Keyword::Bool: return true;
    default: break;
    }
    return false;
}

ast::Storage Parser::storage(std::vector<tokens::Keyword> const& keywords) const
{
    using ast::Storage, tokens::Keyword;
    Storage strg{ Storage::Unspecified };
    size_t specifier_count{ 0 };
    for (auto& k : keywords)
    {
        switch (k)
        {
        case Keyword::Extern:
            strg = Storage::Extern;
            ++specifier_count;
            break;
        case Keyword::Auto:
            strg = Storage::Auto;
            ++specifier_count;
            break;
        case Keyword::Static:
            strg = Storage::Static;
            ++specifier_count;
            break;
        case Keyword::Register:
            strg = Storage::Register;
            ++specifier_count;
            break;
        default: continue;
        }
    }
    if (specifier_count > 1)
    {
        lexer_.loc().err() << "Mutliple storage specifiers are dissallowed\n";
    }
    return strg;
}

ast::Ptr<ast::TypeDecl> Parser::type()
{
    std::vector<tokens::Keyword> specifiers;
    auto const loc = lexer_.peek().loc;

    while (true)
    {
        auto res = lexer_.peek();
        auto key = std::get_if<tokens::Keyword>(&res.value);
        if (key == nullptr || !is_type_keyword(*key)) break;
        specifiers.emplace_back(*key);
        lexer_.advance();
    }

    if (specifiers.size() == 0)
    {
        return nullptr;
    }
    auto strg = storage(specifiers);

    Type typ{ loc, std::move(specifiers) };
    return std::make_unique<ast::TypeDecl>(loc, sema_.new_type(typ), strg);
}

ast::Ptr<ast::Declaration> Parser::declaration()
{
    auto const loc = lexer_.peek().loc;
    auto ret_t = type();
    if (ret_t == nullptr)
    {
        loc.err() << "Each declarations requires type\n";
        return nullptr;
    }

    auto iden = identifier();
    if (match_consume(tokens::Punctuator::LParen))
    {
        // TODO Parse parameter list
        std::vector<ast::Ptr<ast::ObjDecl>> args;
        expect(tokens::Punctuator::RParen);
        auto compound = compound_stmt();
        return std::make_unique<ast::FunctionDecl>(loc, std::move(ret_t), std::move(iden), std::move(args),
                                                   std::move(compound));
    }

    ast::Ptr<ast::Expr> initalizer{ match_consume(tokens::Punctuator::Equal) ? expr() : nullptr };
    expect(tokens::Punctuator::Semicolon);
    return std::make_unique<ast::ObjDecl>(loc, std::move(ret_t), std::move(iden), std::move(initalizer));
}

ast::Ptr<ast::Iden> Parser::identifier()
{
    auto token = lexer_.advance();
    if (token.tag != tokens::Tag::Identifier)
    {
        token.loc.err() << "Expected identifier, found: " << token.format();
        return nullptr;
    }
    return std::make_unique<ast::Iden>(token.loc, std::get<std::string>(token.value));
}

ast::Ptr<ast::Stmt> Parser::selection_statement()
{
    auto const tok = lexer_.peek();
    expect(tokens::Keyword::If);
    expect(tokens::Punctuator::LParen);
    auto cond = expr();
    expect(tokens::Punctuator::RParen);
    auto cons = statement();
    auto alt = match_consume(tokens::Keyword::Else) ? statement() : nullptr;
    return std::make_unique<ast::IfStmt>(tok.loc, std::move(cond), std::move(cons), std::move(alt));
}

ast::Ptr<ast::Stmt> Parser::statement()
{
    // labeled-statement
    // expression-statement
    // selection-statement
    // iteration-statement
    // jump-statement

    auto const tok = lexer_.peek();
    if (match(tokens::Punctuator::LBrace))
    {
        return compound_stmt();
    }

    if (match(tokens::Keyword::If))
    {
        return selection_statement();
    }

    if (match_consume(tokens::Punctuator::Semicolon))
    {
        return std::make_unique<ast::NullStmt>(tok.loc);
    }

    if (match_consume(tokens::Keyword::Return))
    {
        ast::Ptr<ast::Expr> ret_val{ match(tokens::Punctuator::Semicolon) ? nullptr : expr() };
        expect(tokens::Punctuator::Semicolon);
        return std::make_unique<ast::ReturnStmt>(tok.loc, std::move(ret_val));
    }

    auto exp = expr();
    expect(tokens::Punctuator::Semicolon);
    return std::make_unique<ast::ExprStmt>(tok.loc, std::move(exp));
}

ast::Ptr<ast::Items> Parser::items()
{
    Loc loc{ lexer_.loc() };
    auto get_item = [&]() -> ast::Ptr<ast::Item>
    {
        auto const tok = lexer_.peek();
        if (auto k = std::get_if<tokens::Keyword>(&tok.value); k != nullptr && is_type_keyword(*k))
        {
            return std::make_unique<ast::Item>(declaration());
        }
        return std::make_unique<ast::Item>(statement());
    };

    std::vector<ast::Ptr<ast::Item>> items;
    while (!match(tokens::Punctuator::RBrace) && !match(tokens::Tag::EoF))
    {
        items.emplace_back(get_item());
    }

    return std::make_unique<ast::Items>(loc, std::move(items));
}

ast::Ptr<ast::CompoundStmt> Parser::compound_stmt()
{
    auto const loc{ lexer_.peek().loc };
    expect(tokens::Punctuator::LBrace);
    auto its = items();
    expect(tokens::Punctuator::RBrace);
    return std::make_unique<ast::CompoundStmt>(loc, std::move(its));
}

ast::Ptr<ast::IntLiteral> Parser::constant()
{
    auto const tok = lexer_.advance();
    assert(tok.tag == tokens::Tag::Constant);
    return std::make_unique<ast::IntLiteral>(tok.loc, std::get<int64_t>(tok.value));
}

ast::Ptr<ast::Expr> Parser::unary_expr()
{
    auto const atom{ lexer_.peek() };
    switch (atom.tag)
    {
    case tokens::Tag::Identifier: return identifier();
    case tokens::Tag::Constant: return constant();
    case tokens::Tag::Punctuator:
    {
        if (match_consume(tokens::Punctuator::LParen))
        {
            auto e = expr();
            expect(tokens::Punctuator::RParen);
            return e;
        }
        auto const p = std::get<tokens::Punctuator>(atom.value);
        if (auto bp = prefix_binding_power(p); bp != -1)
        {
            lexer_.advance();
            return std::make_unique<ast::UnaryExpr>(atom.loc, p, expr(bp));
        }
    }
        [[fallthrough]];
    default: REPORT_ICE("Unrecognized expression atom ");
    }
}

ast::Ptr<ast::Expr> Parser::expr(int8_t min_bp)
{
    auto lhs = unary_expr();
    while (true)
    {
        auto const tok = lexer_.peek();
        auto const op = std::get<tokens::Punctuator>(tok.value);
        auto const [lbp, rbp] = binding_power(op);
        if (lbp < min_bp) break;
        lexer_.advance();
        auto const loc = lhs->loc();
        auto rhs = expr(rbp);
        lhs = std::make_unique<ast::BinExpr>(loc, std::move(lhs), op, std::move(rhs));
    }
    return lhs;
}

} // namespace compiler
