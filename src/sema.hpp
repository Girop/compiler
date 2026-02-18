#pragma once
#include "ast.hpp"
#include "sema.fwd.hpp"
#include <stack>
#include <unordered_map>

namespace compiler
{

// TODO leave only bare minimum in Sema alone? 
class Sema
{
    using EnvStack = std::stack<std::unordered_map<std::string, ast::Declaration const*>>;
public:
    const ast::Type* check(ast::BinExpr const&);
    const ast::Type* check(ast::IntLiteral const&);
    const ast::Type* check(ast::Iden const&);
    const ast::Type* check(ast::UnaryExpr const&);

    void check(ast::IfStmt const&);
    void check(ast::ExprStmt const&);
    void check(ast::CmpndStmt const&);

    void add(ast::ObjDecl const&);
    void add(ast::FunctionDecl const&);

private:
    void push();
    void pop();
    

    EnvStack scope_;
};

} // namespace compiler
