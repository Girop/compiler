#pragma once
#include "ast/ast.hpp"
#include "inst.hpp"
#include <unordered_map>
#include "graph.fwd.hpp"

namespace compiler::codegen
{

struct Block
{
    std::vector<std::unique_ptr<Inst>> ins;
    std::vector<Block*> successors;
    bool filled{ false };
    bool sealed{ false };

};

class CFG
{
    friend GraphWriter; 

public:
    static CFG construct(ast::FunctionDecl const& func);

    explicit CFG(std::string_view name) : name_{ name } {}

    CFG(CFG const&) = delete;
    CFG& operator=(CFG const&) = delete;
    CFG(CFG&&) = default;
    CFG& operator=(CFG&&) = default;

    Block* insert() { return blocks_.emplace_back(std::make_unique<Block>()).get(); }

    void add_successor(Block* parent, Block* suc)
    {
        parent->successors.emplace_back(suc);
        assert(!suc->sealed);
        predecessors_[suc].emplace_back(parent);
    }

    std::string_view name() const { return name_; }

    std::vector<Block*> predecessors(Block* block) { return predecessors_[block]; }

    void dump() const;
private:
    std::string_view name_;
    std::vector<std::unique_ptr<Block>> blocks_;
    std::unordered_map<Block*, std::vector<Block*>> predecessors_;
};

} // namespace compiler::ssa
