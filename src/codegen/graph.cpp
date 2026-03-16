#include "graph.hpp"
#include "cfg.hpp"
#include <algorithm>
#include <format>
#include <fstream>

namespace compiler::codegen
{

using BlockView = std::span<std::unique_ptr<Block> const>;

namespace
{

std::string nodes(BlockView blocks)
{
    std::string res;
    size_t idx{};
    for (auto& block : blocks)
    {
        std::string repr;
        for (auto& inst : block.get()->ins)
        {
            repr += inst->to_string() + "\\l";
        }

        std::string block_repr{ std::format("{} [label=\"{}\"];", std::to_string(idx), repr) };
        res += block_repr;
        res += "\n";
        ++idx;
    }
    return res;
}

std::string verticies(BlockView blocks)
{

    std::string res;
    size_t start_idx{};
    for (auto& b : blocks)
    {
        for (auto* succ : b->successors)
        {
            auto it = std::find_if(blocks.begin(), blocks.end(), [&succ](auto& block) { return succ == block.get(); });
            assert(it != blocks.end());
            auto end_idx = std::distance(blocks.begin(), it);
            res += std::format("\t{} -> {};\n", start_idx, end_idx);
        }
        ++start_idx;
    }

    return res;
}

} // namespace

void GraphWriter::dump(CFG const& cfg)
{
    auto filename = std::format("cfg.{}.dot", cfg.name());
    std::ofstream file(filename, std::ios::out);

    constexpr std::string_view graph_opts = "graph [fontname=\"Helvetica\", rankdir=TB]";
    constexpr std::string_view node_opts = "node [shape=record, fontsize=10, style=filled, fillcolor=lightyellow]";
    auto graph_name = std::format("digraph CFG_{}", cfg.name());

    file << graph_name << "{\n\t" << graph_opts << "\n\t" << node_opts << "\n\n"
         << nodes(cfg.blocks_) << '\n'
         << verticies(cfg.blocks_) << '\n'
         << "}\n";
};

} // namespace compiler::codegen
