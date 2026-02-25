#include "ast.hpp"
#include "lexer/reflection.hpp"
#include "type.hpp"

namespace compiler::ast
{

namespace
{

std::ostream& stream(std::ostream& os, DeclOrStmt const& item)
{
    auto format = [&os](auto const& i) -> std::ostream& { return i->stream(os); };
    return std::visit(format, item);
}

} // namespace

std::ostream& TranslationUnit::stream(std::ostream& os) const
{
    for (auto const& item : items_)
    {
        ::compiler::ast::stream(os, item);
    }
    return os;
}

std::ostream& CompoundStmt::stream(std::ostream& os) const
{
    os << "{\n";
    for (auto const& item : items_)
    {
        ::compiler::ast::stream(os, item);
    }
    os << "}\n";
    return os;
}

std::ostream& ObjDecl::stream(std::ostream& os) const
{
    type_->stream(os);
    iden_->stream(os);
    if (init_ != nullptr)
    {
        os << " = ";
        init_->stream(os);
    }
    os << ";\n";
    return os;
}

std::ostream& FunctionDecl::stream(std::ostream& os) const
{
    return_->stream(os);
    iden_->stream(os);
    os << "()\n";
    body_->stream(os);
    return os;
}

std::ostream& Iden::stream(std::ostream& os) const { return os << name_; }

std::ostream& UnaryExpr::stream(std::ostream& os) const
{
    os << "( " << tokens::to_string(op_) << " ";
    operand_->stream(os);
    return os << " )";
}

std::ostream& BinExpr::stream(std::ostream& os) const
{
    os << "(";
    lhs_->stream(os);
    os << " " << tokens::to_string(op_) << " ";
    rhs_->stream(os);
    os << ")";
    return os;
}

std::ostream& IfStmt::stream(std::ostream& os) const
{
    os << "if (";
    cond_->stream(os);
    os << ")\n";
    cons_->stream(os);

    if (alt_ != nullptr)
    {
        os << "else\n";
        alt_->stream(os);
    }
    return os;
}

std::ostream& ReturnStmt::stream(std::ostream& os) const
{
    os << "return ";
    value_->stream(os);
    return os << "\n";
}

std::ostream& ExprStmt::stream(std::ostream& os) const
{
    expr_->stream(os);
    return os << ";\n";
}

void ExprStmt::check(Sema&) const {}

std::ostream& IntLiteral::stream(std::ostream& os) const { return os << std::to_string(value_); }

Type const* UnaryExpr::check(Sema&) const { return nullptr; }

const Type* IntLiteral::check(Sema&) const { return nullptr; }

void IfStmt::check(Sema&) const {}

void ObjDecl::add(Sema&) const {}

void CompoundStmt::check(Sema&) const {}

const Type* BinExpr::check(Sema&) const { return nullptr; }

const Type* Iden::check(Sema&) const { return nullptr; }

void ReturnStmt::check(Sema&) const {}

void FunctionDecl::add(Sema&) const {}

} // namespace compiler::ast
