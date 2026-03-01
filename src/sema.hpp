#pragma once
#include "ast.hpp"
#include "sema.fwd.hpp"
#include "type.hpp"

namespace compiler
{

class TypeCamp
{
public:
    TypeCamp();

    Type const* add(Type const&);
    Type const* get(BasicType) const;

private:
    std::vector<std::unique_ptr<Type>> types_;
};

class Sema
{
public:
    Type const* new_type(Type const& t) { return types_.add(t); }
    void add(ast::ObjDecl const& obj);
    void add(ast::FunctionDecl const& f);

    void push() { scope_.objs_.emplace_back(); }
    void pop() { scope_.objs_.pop_back(); }

    ast::FunctionDecl const& current_fuction();
    ast::ObjDecl const* lookup(ast::Iden const& iden) const;

    Type const* get_type(BasicType type) const { return types_.get(type); }
    // TODO after changing the type, reevaluate the value 
private:
    struct
    {
        std::vector<ast::FunctionDecl const*> functions_;
        std::vector<std::vector<ast::ObjDecl const*>> objs_;
    } scope_;
    TypeCamp types_;
};

} // namespace compiler
