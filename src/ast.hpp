#pragma once
#include "lexer/token.hpp"
#include "loc.hpp"
#include "sema.fwd.hpp"
#include "type.fwd.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace compiler::ast
{

template <typename T> using Ptr = std::unique_ptr<T>;

class Node
{
public:
    explicit Node(Loc loc) : loc_{ loc } {}
    virtual ~Node() = default;

    Loc loc() const { return loc_; }

    void dump() const { stream(std::cout) << std::endl; }
    virtual std::ostream& stream(std::ostream&) const = 0;
    // virtual bool accepted() const { return true; };

private:
    Loc loc_;
};

class Expr : public Node
{
public:
    using Node::Node;

    virtual Type const* check(Sema&) const = 0;

protected:
    Type const* type = nullptr;
};

class Stmt : public Node
{
public:
    using Node::Node;

    virtual void check(Sema&) const = 0;
};

class BinExpr : public Expr
{
public:
    explicit BinExpr(Loc loc, Ptr<Expr>&& lhs, tokens::Punctuator op, Ptr<Expr>&& rhs) :
        Expr(loc),
        op_{ op },
        lhs_{ std::move(lhs) },
        rhs_{ std::move(rhs) }
    {
    }

    const Type* check(Sema&) const override;

    std::ostream& stream(std::ostream&) const override;

private:
    tokens::Punctuator op_;
    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
};

class IntLiteral : public Expr
{
public:
    IntLiteral(Loc loc, int value) : Expr(loc), value_{ value } {}

    const Type* check(Sema&) const override;
    std::ostream& stream(std::ostream&) const override;

private:
    int value_;
};

class Iden : public Expr
{
public:
    Iden(Loc loc, std::string const& name) : Expr(loc), name_{ name } {}

    const Type* check(Sema&) const override;
    std::ostream& stream(std::ostream&) const override;

private:
    std::string name_;
};

class UnaryExpr : public Expr
{
public:
    UnaryExpr(Loc loc, tokens::Punctuator op, Ptr<Expr>&& operand) :
        Expr(loc),
        op_{ op },
        operand_{ std::move(operand) }
    {
    }

    Type const* check(Sema&) const override;
    std::ostream& stream(std::ostream&) const override;

private:
    tokens::Punctuator op_;
    Ptr<Expr> operand_;
};

class ExprStmt : public Stmt
{
public:
    ExprStmt(Loc loc, Ptr<Expr>&& expr) : Stmt(loc), expr_{ std::move(expr) } {}

    void check(Sema&) const override;
    std::ostream& stream(std::ostream&) const override;

private:
    Ptr<Expr> expr_;
};

class IfStmt : public Stmt
{
public:
    IfStmt(Loc loc, Ptr<Expr>&& cond, Ptr<Stmt>&& cons, Ptr<Stmt>&& alt) :
        Stmt(loc),
        cond_{ std::move(cond) },
        cons_{ std::move(cons) },
        alt_{ std::move(alt) }
    {
    }

    void check(Sema&) const override;
    std::ostream& stream(std::ostream&) const override;

private:
    Ptr<Expr> cond_;
    Ptr<Stmt> cons_;
    Ptr<Stmt> alt_;
};

class Declaration : public Node
{
public:
    using Node::Node;
    virtual void add(Sema&) const = 0;
};

class ObjDecl : public Declaration
{
public:
    ObjDecl(Loc loc, Ptr<Type>&& type, Ptr<Iden>&& iden, Ptr<Expr>&& init) :
        Declaration(loc),
        type_{ std::move(type) },
        iden_{ std::move(iden) },
        init_{ std::move(init) }
    {
    }

    std::ostream& stream(std::ostream&) const override;
    void add(Sema&) const override;

private:
    Ptr<Type> type_;
    Ptr<Iden> iden_;
    Ptr<Expr> init_;
};

using DeclOrStmt = std::variant<Ptr<Stmt>, Ptr<Declaration>>;

class CompoundStmt : public Stmt
{
public:
    explicit CompoundStmt(Loc loc, std::vector<DeclOrStmt>&& itms) : Stmt(loc), items_{ std::move(itms) } {}

    std::ostream& stream(std::ostream& os) const override;
    void check(Sema&) const override;

private:
    std::vector<DeclOrStmt> items_;
};

class FunctionDecl : public Declaration
{
public:
    FunctionDecl(Loc loc, Ptr<Type>&& type, Ptr<Iden>&& iden, std::vector<Ptr<ObjDecl>>&& args,
                 Ptr<CompoundStmt>&& body) :
        Declaration(loc),
        return_{ std::move(type) },
        iden_{ std::move(iden) },
        args_{ std::move(args) },
        body_{ std::move(body) }
    {
    }

    void add(Sema&) const override;
    std::ostream& stream(std::ostream& os) const override;

private:
    Ptr<Type> return_;
    Ptr<Iden> iden_;
    std::vector<Ptr<ObjDecl>> args_;
    Ptr<CompoundStmt> body_;
};

class ReturnStmt : public Stmt
{
public:
    ReturnStmt(Loc loc, Ptr<Expr>&& value) : Stmt(loc), value_{ std::move(value) } {}

    void check(Sema&) const override;
    std::ostream& stream(std::ostream& os) const override;

private:
    Ptr<Expr> value_;
};

class NullStmt : public Stmt
{
public:
    explicit NullStmt(Loc loc) : Stmt(loc) {}

    void check(Sema&) const override {};
    std::ostream& stream(std::ostream& os) const override { return os << ";\n"; }
};

class TranslationUnit : public Node
{
public:
    TranslationUnit(std::string_view filename, std::vector<DeclOrStmt>&& items) :
        Node(Loc{ filename }),
        items_{ std::move(items) }
    {
    }

    std::ostream& stream(std::ostream&) const override;

private:
    std::vector<DeclOrStmt> items_;
};

} // namespace compiler::ast
