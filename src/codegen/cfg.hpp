#pragma once
#include "ast/ast.hpp"
#include "cfgGraph.fwd.hpp"
#include "inst.hpp"

namespace compiler::codegen
{

class Block
{
public:
    template <typename... Args> Inst* insert(Args&&... args)
    {
        auto& instr_ref = ins_.emplace_back(std::forward<Args>(args)...);
        return instr_ref.get();
    }

    void seal()
    {
        assert(!sealed_);
        sealed_ = true;
    }

    void fill()
    {
        assert(!filled_);
        filled_ = true;
    }

    void add_successor(Block* suc)
    {
        successors_.emplace_back(suc);
        assert(!suc->sealed_);
        suc->predecessors_.emplace_back(this);
    }

    std::span<Block*> successors() { return successors_; }
    std::span<Block*> predecessors() { return predecessors_; }
    std::span<std::unique_ptr<Inst>> ins() { return ins_; }

    bool is_sealed() const { return sealed_; }
    bool is_filled() const { return filled_; }

    void split_edge(Block* parent, Block* block)
    {
        std::erase(predecessors_, parent);
        parent->add_successor(block);
        add_successor(block);
    }

private:
    bool filled_{ false };
    bool sealed_{ false };

    std::vector<Block*> successors_;
    std::vector<Block*> predecessors_;

    std::vector<std::unique_ptr<Inst>> ins_;
};

class CFG
{
    friend cfg::GraphAdapter;
    friend class SSAGenerator;

public:
    static CFG construct(ast::FunctionDecl const& func);
    explicit CFG(std::string_view name) : name_{ name } {}

    ~CFG();

    CFG(CFG const&) = delete;
    CFG& operator=(CFG const&) = delete;
    CFG(CFG&&) = default;
    CFG& operator=(CFG&&) = default;

    std::string_view name() const { return name_; }
    std::vector<std::unique_ptr<Block>> const& blocks() const { return blocks_; }

    std::vector<Inst*> lower();

    // TODO this is in fact the 2nd type of IR, already lowered one 
    // a) it should be more distinct aka. maybe a separete class wrapper on lowering operations? 
    // b) This 2nd IR should already have notion of the memory storage 
    // c) refactor of CFG construction will be needed as hell
    void add_labels();
    void phi_resolution();

    void dumpCFG() const;

private:
    Block* insert() 
    {
        return blocks_.emplace_back(std::make_unique<Block>()).get();
    }
    std::vector<Inst*> users(Inst* inst) const;

    std::string_view name_;
    std::vector<std::unique_ptr<Block>> blocks_;
};

} // namespace compiler::codegen
