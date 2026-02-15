#pragma once
#include "loc.hpp"
#include "sema.fwd.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace compiler::ast
{

template <typename T> using Ptr = std::unique_ptr<T>;

class Node
{
public:
    explicit Node(Loc loc) :
        loc_{ loc }
    {
    }
    virtual ~Node() {}

    Loc loc() const { return loc_; }

    void dump() const { stream(std::cout) << std::endl; }
    virtual std::ostream& stream(std::ostream&) const = 0;

private:
    Loc loc_;
};

struct Type;

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
    enum class Op : uint8_t
    {
        Add,
        Sub,
        Mul,
        Div,
    };

    explicit BinExpr(Loc loc, Ptr<Expr>&& lhs, Op op, Ptr<Expr>&& rhs) :
        Expr(loc),
        op_{ op },
        lhs_{ std::move(lhs) },
        rhs_{ std::move(rhs) }
    {
    }

    const Type* check(Sema&) const override;

private:
    Op op_;
    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
};

class IntLiteral : public Expr
{
public:
    IntLiteral(Loc loc, int value) :
        Expr(loc),
        value_{ value }
    {
    }

    const Type* check(Sema&) const override;

private:
    int value_;
};

class Iden : public Expr
{
public:
    Iden(Loc loc, std::string const& name) :
        Expr(loc),
        name_{ name }
    {
    }

    const Type* check(Sema&) const override;

private:
    std::string name_;
};

class UnaryExpr : public Expr
{
public:
    enum class Op : uint8_t
    {
        Negate,
        Not,
    };

    UnaryExpr(Loc loc, Ptr<Expr>&& operand) :
        Expr(loc),
        operand_{ std::move(operand) }
    {
    }

    virtual Type const* check(Sema&) const = 0;

private:
    Op op_;
    Ptr<Expr> operand_;
};

class ExprStmt : public Stmt
{
public:
    ExprStmt(Loc loc, Ptr<Expr>&& expr) :
        Stmt(loc),
        expr_{ std::move(expr) }
    {
    }

private:
    Ptr<Expr> expr_;
};

class CmpndStmt : public Stmt
{
public:
    explicit CmpndStmt(Loc loc, std::vector<Ptr<Stmt>>&& stmts) :
        Stmt(loc),
        stmts_{ std::move(stmts) }
    {
    }

private:
    std::vector<Ptr<Stmt>> stmts_;
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

private:
    Ptr<Expr> cond_;
    Ptr<Stmt> cons_;
    Ptr<Stmt> alt_;
};

class ObjDecl : public Node
{
public:
    ObjDecl(Loc loc, Type const* type, Ptr<Iden>&& iden, Ptr<Expr>&& init) :
        Node(loc),
        type_{ type },
        iden_{ std::move(iden) },
        init_{ std::move(init) }
    {
    }

    void add(Sema&) const;
private:
    Type const* type_;
    Ptr<Iden> iden_;
    Ptr<Expr> init_;
};

class FunctionDecl : public Node
{
public:
    FunctionDecl(Loc loc, Type const* type, Ptr<Iden>&& iden,
                 std::vector<Ptr<ObjDecl>>&& args, Ptr<CmpndStmt>&& body) :
        Node(loc),
        return_t_{ type },
        iden_{ std::move(iden) },
        args_{ std::move(args) },
        body_{ std::move(body) }
    {
    }

private:
    Type const* return_t_;
    Ptr<Iden> iden_;
    std::vector<Ptr<ObjDecl>> args_;
    Ptr<CmpndStmt>&& body_;
};

} // namespace compiler::ast
