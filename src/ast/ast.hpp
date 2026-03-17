#pragma once
#include "lexer/token.hpp"
#include "loc.hpp"
#include "sema.fwd.hpp"
#include "type.hpp"
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

private:
    Loc loc_;
};

class Expr : public Node
{
public:
    using Node::Node;

    virtual Type const* check(Sema&) = 0;

protected:
    Type const* type_ = nullptr;
};

class Stmt : public Node
{
public:
    using Node::Node;

    virtual void check(Sema&) = 0;
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

    const Type* check(Sema&) override;
    std::ostream& stream(std::ostream&) const override;

    tokens::Punctuator op() const { return op_; }
    Expr const& lhs() const { return *lhs_; }
    Expr const& rhs() const { return *rhs_; }

private:
    tokens::Punctuator op_;
    Ptr<Expr> lhs_;
    Ptr<Expr> rhs_;
};

class IntLiteral : public Expr
{
public:
    IntLiteral(Loc loc, int64_t value) : Expr(loc), value_{ value } {}

    const Type* check(Sema&) override;
    std::ostream& stream(std::ostream&) const override;
    int64_t value() const { return value_; }

private:
    int64_t value_;
};

class Iden : public Expr
{
public:
    Iden(Loc loc, std::string const& name) : Expr(loc), name_{ name } {}

    std::string_view name() const { return name_; }

    const Type* check(Sema&) override;
    std::ostream& stream(std::ostream&) const override;

    class ObjDecl const* referenced() const { return referenced_; }

private:
    std::string name_;
    ObjDecl const* referenced_{ nullptr };
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

    Type const* check(Sema&) override;
    std::ostream& stream(std::ostream&) const override;

    tokens::Punctuator op() const { return op_; }
    Expr& expr() const { return *operand_; }

private:
    tokens::Punctuator op_;
    Ptr<Expr> operand_;
};

class ExprStmt : public Stmt
{
public:
    ExprStmt(Loc loc, Ptr<Expr>&& expr) : Stmt(loc), expr_{ std::move(expr) } {}

    void check(Sema&) override;
    std::ostream& stream(std::ostream&) const override;
    Expr const& expr() const { return *expr_; }

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

    void check(Sema&) override;
    std::ostream& stream(std::ostream&) const override;

    Expr const& cond() const { return *cond_; }
    Stmt const& cons() const { return *cons_; };
    Stmt const* alt() const { return alt_.get(); };

private:
    Ptr<Expr> cond_;
    Ptr<Stmt> cons_;
    Ptr<Stmt> alt_;
};

enum class Storage : uint8_t
{
    Unspecified,
    Extern,
    Auto,
    Static,
    Register
};

class TypeDecl : public Node
{
public:
    TypeDecl(Loc loc, Type const* type, Storage storage) : Node(loc), storage_{ storage }, type_{ type } {}
    virtual std::ostream& stream(std::ostream&) const override;

    void set_default_storage(Storage implicit) { storage_ = storage_ == Storage::Unspecified ? implicit : storage_; }

    Storage storage() const { return storage_; }
    Type const* type() const { return type_; }

private:
    Storage storage_;
    Type const* type_;
};

class Declaration : public Node
{
public:
    using Node::Node;
    virtual void add(Sema&) const = 0;
};

class ObjDecl : public Declaration
{
    friend Sema;

public:
    ObjDecl(Loc loc, Ptr<TypeDecl>&& type, Ptr<Iden>&& iden, Ptr<Expr>&& init) :
        Declaration(loc),
        type_{ std::move(type) },
        iden_{ std::move(iden) },
        init_{ std::move(init) }
    {
        // TODO this only works for variables inside functions
        type_->set_default_storage(Storage::Auto);
    }

    std::ostream& stream(std::ostream&) const override;
    void add(Sema&) const override;
    TypeDecl const& type() const { return *type_; }
    Iden const& iden() const { return *iden_; };
    Expr const* initalizer() const { return init_.get(); }

private:
    Ptr<TypeDecl> type_;
    Ptr<Iden> iden_;
    Ptr<Expr> init_;
};

class Item : public Node
{
public:
    explicit Item(Ptr<Declaration>&& decl) : Node(decl->loc()), value_{ std::move(decl) } {}
    explicit Item(Ptr<Stmt>&& stmt) : Node(stmt->loc()), value_{ std::move(stmt) } {}

    Stmt const* stmt() const { return dynamic_cast<Stmt const*>(value_.get()); }
    Stmt* stmt() { return dynamic_cast<Stmt*>(value_.get()); }
    Declaration const* decl() const { return dynamic_cast<Declaration const*>(value_.get()); }
    Declaration* decl() { return dynamic_cast<Declaration*>(value_.get()); }

    std::ostream& stream(std::ostream& os) const override;
    void check(Sema&);

private:
    Ptr<Node> value_;
};

class Items : public Node
{
public:
    explicit Items(Loc loc, std::vector<Ptr<Item>>&& itms) : Node(loc), items_{ std::move(itms) } {}

    std::vector<Ptr<Item>> const& items() const { return items_; }
    std::ostream& stream(std::ostream& os) const override;
    void check(Sema&);

private:
    std::vector<Ptr<Item>> items_;
};

class CompoundStmt : public Stmt
{
public:
    explicit CompoundStmt(Loc loc, Ptr<Items>&& itms) : Stmt(loc), items_(std::move(itms)) {}

    std::ostream& stream(std::ostream& os) const override;
    void check(Sema& sema) override;

    Items const& items() const { return *items_; }

private:
    Ptr<Items> items_;
};

class FunctionDecl : public Declaration
{
    friend Sema;

public:
    FunctionDecl(Loc loc, Ptr<TypeDecl>&& type, Ptr<Iden>&& iden, std::vector<Ptr<ObjDecl>>&& args,
                 Ptr<CompoundStmt>&& body) :
        Declaration(loc),
        return_{ std::move(type) },
        iden_{ std::move(iden) },
        args_{ std::move(args) },
        body_{ std::move(body) }
    {
        return_->set_default_storage(Storage::Extern);
    }

    void add(Sema&) const override;
    std::ostream& stream(std::ostream& os) const override;

    TypeDecl const& type() const { return *return_; }
    Iden const& iden() const { return *iden_; }
    CompoundStmt const& body() const { return *body_; }

private:
    Ptr<TypeDecl> return_;
    Ptr<Iden> iden_;
    std::vector<Ptr<ObjDecl>> args_;
    Ptr<CompoundStmt> body_; // TODO could be made optional to mean incomplete type definition
};

class ReturnStmt : public Stmt
{
public:
    ReturnStmt(Loc loc, Ptr<Expr>&& value) : Stmt(loc), expr_{ std::move(value) } {}

    void check(Sema&) override;
    std::ostream& stream(std::ostream& os) const override;

    Expr const* expr() const { return expr_.get(); }

private:
    Ptr<Expr> expr_;
};

class NullStmt : public Stmt
{
public:
    explicit NullStmt(Loc loc) : Stmt(loc) {}

    void check(Sema&) override {};
    std::ostream& stream(std::ostream& os) const override { return os << ";\n"; }
};

class TranslationUnit : public Node
{
public:
    TranslationUnit(std::string_view filename, Ptr<Items>&& items) : Node(Loc{ filename }), items_{ std::move(items) }
    {
    }

    std::ostream& stream(std::ostream&) const override;
    Ptr<Items> const& items() const { return items_; }
    void check(Sema&);

private:
    Ptr<Items> items_;
};

} // namespace compiler::ast
