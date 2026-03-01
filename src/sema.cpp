#include "sema.hpp"
#include <algorithm>
#include <cassert>
#include <format>
#include <ranges>

namespace compiler
{

void Sema::add(ast::ObjDecl const& declared)
{
    auto& scope_decls = scope_.objs_.back();
    auto it = std::find_if(scope_decls.begin(), scope_decls.end(), [&declared](auto const* present)
                           { return declared.iden_->name() == present->iden_->name(); });
    if (it != scope_decls.end())
    {
        declared.loc().err() << std::format("Redefinition of {}", declared.iden_->name());
        return;
    }

    scope_decls.emplace_back(&declared);
}

void Sema::add(ast::FunctionDecl const& func)
{
    if (scope_.objs_.size() != 1)
    {
        func.loc().err() << "Function declaration is not possible here\n";
        return;
    }
    auto it = std::find_if(scope_.functions_.begin(), scope_.functions_.end(),
                           [&func](auto const* present) { return func.iden_->name() == present->iden_->name(); });
    if (it != scope_.functions_.end())
    {
        func.loc().err() << std::format("Redefinition of: {}", func.iden_->name());
        return;
    }
    scope_.functions_.emplace_back(&func);
}

ast::ObjDecl const* Sema::lookup(ast::Iden const& iden) const
{

    auto find_in_scope = [&] (auto& scope) -> ast::ObjDecl const*
    {
        auto it = std::find_if(scope.begin(), scope.end(), [&] (auto const* var) { return var->iden_->name() == iden.name();});
        if (it != scope.end())
        {
            return *it;
        }
        return nullptr;
    };
    
    for (auto& scope : scope_.objs_ | std::views::reverse)
    {
        if (auto found = find_in_scope(scope)) 
        {
            return found; 
        }
    }
    
    iden.loc().err() << "Usage of undefined identifier\n";
    return nullptr;
}

ast::FunctionDecl const& Sema::current_fuction() { return *scope_.functions_.back(); }

TypeCamp::TypeCamp()
{
    auto basic_types = { BasicType::SignedChar,  BasicType::ShortInt, BasicType::Int,    BasicType::LongInt,
                         BasicType::LongLongInt, BasicType::Float,    BasicType::Double, BasicType::LongDouble,
                         BasicType::Bool,        BasicType::Void };
    for (auto& t : basic_types)
    {
        add(Type{ t });
    }
}

Type const* TypeCamp::add(Type const& t)
{
    auto it = std::find_if(types_.begin(), types_.end(), [&t](auto const& present) { return *present == t; });
    if (it != types_.end()) return it->get();
    return types_.emplace_back(std::make_unique<Type>(t)).get();
}

Type const* TypeCamp::get(BasicType type) const
{
    auto const it{ std::find_if(types_.begin(), types_.end(),
                                [&type](auto const& item) { return item->basic_type() == type; }) };
    assert(it != types_.end());
    return it->get();
}

} // namespace compiler
