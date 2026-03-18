#pragma once
#include "ast.hpp"
#include "graph.hpp"
#include <format>
#include <unordered_map>

namespace compiler::ast
{

class GraphAdapter
{
public:
    explicit GraphAdapter(ast::TranslationUnit const& tu) : data_{ tu } {}

    std::string name() const
    {
        return std::format("AST_{}", data_.loc().filename());
    }

    // std::vector<graph::Edge> edges() const
    // {
    //     std::vector<graph::Edge> edges;
    //
    //     edges.emplace_back();
    // }
    //
    // std::vector<graph::Vertex> vertices() const
    // {
    //       
    // }

private:
    ast::TranslationUnit const& data_;
    std::unordered_map<Node*, size_t> node_to_idx;
};

} // namespace compiler::ast
