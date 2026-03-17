#include "ast.hpp"
#include "lexer/reflection.hpp"
#include "sema.hpp"
#include "type.hpp"
#include "util/ice.hpp"
#include <limits>

namespace compiler::ast
{

// stream implementations

namespace
{

constexpr bool is_assign_op(tokens::Punctuator op)
{
    using tokens::Punctuator;
    switch (op)
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
    case Punctuator::PipeEqual: return true;
    default: break;
    }
    return false;
}

} // namespace

std::ostream& Item::stream(std::ostream& os) const
{
    value_->stream(os);
    return os;
}

std::ostream& TranslationUnit::stream(std::ostream& os) const { return items_->stream(os); }

std::ostream& Items::stream(std::ostream& os) const
{
    for (auto& item : items_)
    {
        item->stream(os);
    }
    return os;
}

std::ostream& CompoundStmt::stream(std::ostream& os) const
{
    os << "{\n";
    items_->stream(os);
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
    expr_->stream(os);
    return os << "\n";
}

std::ostream& ExprStmt::stream(std::ostream& os) const
{
    expr_->stream(os);
    return os << ";\n";
}

std::ostream& IntLiteral::stream(std::ostream& os) const { return os << std::to_string(value_); }

std::ostream& TypeDecl::stream(std::ostream& os) const
{
    std::string type;
    switch (storage_)
    {
    case Storage::Unspecified: REPORT_ICE("Type without set storage");
    case Storage::Extern: type += "extern "; break;
    case Storage::Auto: type += "auto "; break;
    case Storage::Static: type += "static "; break;
    case Storage::Register: type += "register "; break;
    }
    type += type_->format();
    return os << type;
}

// end stream implementations
// Expr checks
Type const* UnaryExpr::check(Sema&)
{
    switch (op_)
    {
    case tokens::Punctuator::PlusPlus:
    case tokens::Punctuator::MinusMinus:
    case tokens::Punctuator::Ampersand:
    case tokens::Punctuator::Star:
    case tokens::Punctuator::Plus:
    case tokens::Punctuator::Minus:
    case tokens::Punctuator::Tilde:
    case tokens::Punctuator::Exclaim:
    default: REPORT_ICE("Unexpected operator for UnaryExpression");
    }
}

Type const* IntLiteral::check(Sema& sema)
{
    // TODO only decimal constant without suffixes support now
    if (value_ <= std::numeric_limits<int>::max())
    {
        return type_ = sema.get_type(BasicType::Int);
    }

    if (value_ <= std::numeric_limits<long int>::max())
    {
        return type_ = sema.get_type(BasicType::LongInt);
    }

    if (value_ <= std::numeric_limits<long long int>::max())
    {
        return type_ = sema.get_type(BasicType::LongLongInt);
    }
    REPORT_ICE("Unsupported constant");
}

Type const* BinExpr::check(Sema& sema)
{
    auto lhs = lhs_->check(sema);
    auto rhs = rhs_->check(sema);

    if (is_assign_op(op_))
    {

        Type::implicit_conversion(loc(), *lhs, *rhs);
        // TODO lhs convert rhs to lhs,
        if (!lhs->is_modifyable_lvalue()) // TODO shouldn't this also check whether underlying expr is Iden / table
                                          // access?
        {
            loc().err() << "Cannot assign to non modifyable lvalue\n";
            return type_;
        }
        return type_ = lhs;
    }

    auto converted = Type::implicit_conversion(loc(), *lhs, *rhs);
    return type_ = sema.new_type(converted);
}

Type const* Iden::check(Sema& sema)
{
    referenced_ = sema.lookup(*this);
    return type_ = referenced_->type().type();
}
// End expr checks
// Stmnt checks

void Items::check(Sema& sema)
{
    for (auto& item : items_)
    {
        item->check(sema);
    }
}

void Item::check(Sema& sema)
{
    if (auto st = stmt())
    {
        st->check(sema);
    }
    else
    {
        decl()->add(sema);
    }
}

void ExprStmt::check(Sema& sema) { expr_->check(sema); }

void IfStmt::check(Sema& sema)
{
    auto expr_t = cond_->check(sema);
    if (!expr_t->is_scalar())
    {
        loc().err() << "Value of the expression is not convertible to bool\n";
    }
}

void CompoundStmt::check(Sema& sema) { items_->check(sema); }

void ReturnStmt::check(Sema& sema)
{
    auto& func = sema.current_fuction();
    if (func.type().type()->is_void())
    {
        if (expr_ != nullptr)
        {
            expr_->check(sema);
            loc().err() << "Function declared with \'void\' cannot return a value\n";
            return;
        }
    }

    if (expr_ == nullptr)
    {
        loc().err() << "Function should return a value\n";
        return;
    }

    auto ret_expr_t = expr_->check(sema);
    auto converted = Type::implicit_conversion(loc(), *func.type().type(), *ret_expr_t);
    sema.new_type(converted);
}

void TranslationUnit::check(Sema& sema) { items_->check(sema); }

// end stmnt checks

void ObjDecl::add(Sema& sema) const { sema.add(*this); }

void FunctionDecl::add(Sema& sema) const { sema.add(*this); }

} // namespace compiler::ast
