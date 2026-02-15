#pragma once
#include "ast.hpp"
#include "sema.fwd.hpp"

namespace compiler
{

class Sema
{
public:
    void check(ast::IfStmt const&);
    const ast::Type* check(ast::BinExpr const&);
    const ast::Type* check(ast::Iden const&);


    void add(ast::ObjDecl const&);
    void add(ast::FunctionDecl const&);
private:
    void push();
    void pop();

};

} // namespace compiler
