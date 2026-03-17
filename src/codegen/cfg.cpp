#include "cfg.hpp"
#include "util/ice.hpp"
#include <cassert>
#include <optional>
#include "cfgGraph.hpp"
#include <fstream>

namespace compiler::codegen
{

class NameCounter
{
public:
    size_t get() { return ++counter_; }

private:
    size_t counter_{ 0 };
};

class SSAGenerator
{
public:
    explicit SSAGenerator(ast::FunctionDecl const& func) : func_{ func } {}

    CFG construct()
    {
        auto starting = cfg.insert();
        on_items(starting, func_.body().items());
        return std::move(cfg);
    }

private:
    template <typename T, typename... Args> Inst* emit(Block* block, Args&&... args)
    {
        assert(!block->filled);
        auto& inst = block->ins.emplace_back(std::make_unique<T>(names_.get(), std::forward<Args>(args)...));
        return inst.get();
    }

    Block* on_items(Block* block, ast::Items const& items)
    {
        for (auto& item : items.items())
        {
            block = on_item(block, *item);
        }
        return block;
    }

    Block* on_item(Block* block, ast::Item const& item)
    {
        if (auto decl = item.decl())
        {
            on_decl(block, dynamic_cast<ast::ObjDecl const&>(*decl));
        }
        else
        {
            block = on_stmt(block, *item.stmt());
        }
        return block;
    }

    Block* on_if_branch(Block* parent, ast::Stmt const* stmt)
    {
        auto suc = cfg.insert();
        cfg.add_successor(parent, suc);
        Block* child = stmt ? on_stmt(suc, *stmt) : suc;
        seal(child);
        child->filled = true;
        return child;
    }

    Block* on_if(Block* block, ast::IfStmt const& ifstmt)
    {
        auto cond = on_expr(block, ifstmt.cond());
        seal(block);
        emit<JumpIf>(block, cond);

        Block* lhs = on_if_branch(block, &ifstmt.cons());
        Block* rhs = on_if_branch(block, ifstmt.alt());

        auto exit = cfg.insert();
        cfg.add_successor(lhs, exit);
        cfg.add_successor(rhs, exit);

        return exit;
    }

    // Returns the last created successor by this procedure
    Block* on_stmt(Block* block, ast::Stmt const& stmt)
    {
        if (auto expr = dynamic_cast<ast::ExprStmt const*>(&stmt))
        {
            on_expr(block, expr->expr());
            return block;
        }
        else if (auto compound = dynamic_cast<ast::CompoundStmt const*>(&stmt))
        {
            return on_items(block, compound->items());
        }
        else if (auto ifstmt = dynamic_cast<ast::IfStmt const*>(&stmt))
        {
            return on_if(block, *ifstmt);
        }
        else if (auto retstmt = dynamic_cast<ast::ReturnStmt const*>(&stmt))
        {
            Inst* retval{ nullptr };
            if (retstmt->expr())
            {
                retval = on_expr(block, *retstmt->expr());
            }
            emit<Ret>(block, retval);
            seal(block);
            block->filled = true;
            return block;
        }
        else if (dynamic_cast<ast::NullStmt const*>(&stmt))
        {
            return block;
        }
        REPORT_ICE("Unhandled statement");
    }

    Inst* on_expr(Block* block, ast::Expr const& expr)
    {
        // Only accesses, the write to should be handled in BinExpr
        if (auto iden = dynamic_cast<ast::Iden const*>(&expr))
        {
            return read_variable(iden->referenced(), block);
        }

        if (auto constant = dynamic_cast<ast::IntLiteral const*>(&expr))
        {
            return emit<ConstInst>(block, constant->value());
        }

        if (auto bin = dynamic_cast<ast::BinExpr const*>(&expr))
        {
            if (math_op(bin->op()).has_value())
            {
                return on_math(block, *bin);
            }

            assert(bin->op() == tokens::Punctuator::Equal);
            auto iden = dynamic_cast<ast::Iden const*>(&bin->lhs());
            assert(iden);
            return on_assign(block, iden, bin->lhs());
        }

        if (auto un = dynamic_cast<ast::UnaryExpr const*>(&expr))
        {
            assert(un->op() == tokens::Punctuator::Exclaim);
            auto arg = on_expr(block, un->expr());
            return emit<Unary>(block, Opcode::LogicalNegate, arg);
        }

        REPORT_ICE("Unhandled expression evaluation");
    }

    Inst* on_math(Block* block, ast::BinExpr const& bin)
    {
        auto lhs = on_expr(block, bin.lhs());
        auto rhs = on_expr(block, bin.rhs());
        return emit<MathInst>(block, math_op(bin.op()).value(), lhs, rhs);
    }

    Inst* on_assign(Block* block, ast::Iden const* iden, ast::Expr const& rhs)
    {
        auto rhs_inst = on_expr(block, rhs);
        write_var(iden->referenced(), block, rhs_inst);
        return emit<Set>(block, read_variable(iden->referenced(), block));
    }

    std::optional<Opcode> math_op(tokens::Punctuator punct) const
    {
        switch (punct)
        {
        case tokens::Punctuator::Plus: return Opcode::Add;
        case tokens::Punctuator::Minus: return Opcode::Sub;
        case tokens::Punctuator::Slash: return Opcode::Div;
        case tokens::Punctuator::Star: return Opcode::Mul;
        case tokens::Punctuator::EqualEqual: return Opcode::Cmp;
        default: return std::nullopt;
        }
    }

    void on_decl(Block* block, ast::ObjDecl const& decl)
    {
        if (decl.initalizer() == nullptr) return;
        Inst* init = on_expr(block, *decl.initalizer());
        write_var(decl.iden().referenced(), block, init);
    }

    Inst* read_variable(ast::ObjDecl const* var, Block* block)
    {
        auto& values = current_defs.at(var);
        if (auto it = values.find(block); it != values.end())
        {
            return it->second;
        }

        return read_var_recursive(var, block);
    }

    Inst* read_var_recursive(ast::ObjDecl const* var, Block* block)
    {
        Inst* value;
        if (!block->sealed)
        {
            auto shadow_var = names_.get();
            auto phi = emit<Phi>(block, shadow_var);
            incomplete_phis[block][var] = phi->as<Phi>();
            value = phi;
        }
        else if (cfg.predecessors(block).size() == 1)
        {
            value = read_variable(var, cfg.predecessors(block)[0]);
        }
        else
        {
            auto shadow_var = names_.get();
            value = emit<Phi>(block, shadow_var);
            write_var(var, block, value);
            value = add_phi_operands(block, var, value->as<Phi>());
        }
        write_var(var, block, value);
        return value;
    }

    void write_var(ast::ObjDecl const* var, Block* block, Inst* value) { current_defs[var][block] = value; }

    Inst* add_phi_operands(Block* block, ast::ObjDecl const* var, Phi* phi)
    {
        for (Block* pred : cfg.predecessors(block))
        {
            phi->append_operand(read_variable(var, pred));
        }
        // TODO return remove_trivial_phi(phi);
        return phi;
    }

    Inst* remove_trivial_phi(Phi* phi)
    {
        Inst* same{ nullptr };
        for (auto* op : phi->args())
        {
            if (same == op || op == phi) continue;
            if (same != nullptr) return phi;
            same = op;
        }
        // TODO rest of the algo
        return phi;
    }

    void seal(Block* block)
    {
        assert(!block->sealed);
        for (auto [var, phi] : incomplete_phis[block])
        {
            add_phi_operands(block, var, phi);
        }
        block->sealed = true;
    }

    ast::FunctionDecl const& func_;
    CFG cfg{ func_.iden().name() };
    NameCounter names_;

    std::unordered_map<Block*, std::unordered_map<ast::ObjDecl const*, Phi*>> incomplete_phis;
    std::unordered_map<ast::ObjDecl const*, std::unordered_map<Block*, Inst*>> current_defs;
};

CFG CFG::construct(ast::FunctionDecl const& func)
{
    SSAGenerator gen(func);
    return gen.construct();
}

CFG::~CFG()
{
    // assert(std::all_of(blocks_.begin(), blocks_.end(), [](auto& b) { return b->sealed; }));
    // assert(std::all_of(blocks_.begin(), blocks_.end(), [](auto& b) { return b->filled; }));
}

void CFG::dump() const 
{
    auto filename = std::format(".cfg.{}.dot", name_);
    std::ofstream file(filename, std::ios::out);
    graph::CfgGraphAdapter a {*this};
    return graph::GraphWriter<graph::CfgGraphAdapter>::dump(file, a);
    
}

} // namespace compiler::codegen
