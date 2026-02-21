#include "ast.hpp"
#include "type.hpp"

namespace compiler::ast
{

std::ostream& TranslationUnit::stream(std::ostream& os) const
{
    for(auto& decl : decls_)
        {
            decl->stream(os) << '\n';
        }
    return os;
}

std::ostream& CompoundStmt::stream(std::ostream& os) const
{
    os << "{\n";
    for(auto& stmt : stmts_)
        {
            stmt->stream(os);
        }
    os << "}\n";
    return os;
}

std::ostream& ObjDecl::stream(std::ostream& os) const
{
    type_->stream(os);
    iden_->stream(os);
    os << "= ";
    if(init_ != nullptr)
        {
            init_->stream(os);
        }
    os << "; ";
    return os;
}

std::ostream& FunctionDecl::stream(std::ostream& os) const
{
    return_->stream(os);
    iden_->stream(os);
    os << "() {\n";

    body_->stream(os);

    os << "}\n";
    return os;
}

std::ostream& Iden::stream(std::ostream& os) const { return os << name_; }

} // namespace compiler::ast
