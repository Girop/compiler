#pragma once
#include "ast.hpp"
#include "sema.fwd.hpp"
#include <stack>
#include "type.hpp"
#include <unordered_map>

namespace compiler
{

class Sema
{
    using EnvStack
        = std::stack<std::unordered_map<std::string, ast::Declaration const*>>;

public:
    void add(ast::ObjDecl const&);
    void add(ast::FunctionDecl const&);

private:
    void push();
    void pop();

    EnvStack scope_;
};

} // namespace compiler
