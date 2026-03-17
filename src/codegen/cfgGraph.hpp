#pragma once
#include "cfg.hpp"
#include "cfgGraph.fwd.hpp"
#include "graph.hpp"
#include <algorithm>

namespace compiler::graph
{

class CfgGraphAdapter
{
public:
    explicit CfgGraphAdapter(codegen::CFG const& cfg) : data_{ cfg } {}

    std::string name() const { return std::format("CFG_{}", data_.name()); }

    std::vector<Edge> edges() const
    {
        std::vector<Edge> edges;

        auto get_idx = [&](auto* bb) -> size_t
        {
            auto it = std::find_if(data_.blocks_.begin(), data_.blocks_.end(), [&](auto& b) { return bb == b.get(); });
            assert(it != data_.blocks_.end());

            return std::distance(data_.blocks_.begin(), it);
        };

        for (auto& bb : data_.blocks_)
        {
            size_t from = get_idx(bb.get());
            for (auto& succ : bb->successors)
            {
                size_t to = get_idx(succ);
                edges.emplace_back(from, to);
            }
        }

        return edges;
    }

    std::vector<Vertex> vertices() const
    {
        std::vector<Vertex> vertices;

        size_t idx{0};
        for (auto& bb : data_.blocks_)
        {
            std::string label;
            for (auto& ins : bb->ins)
            {
                label += ins->to_string();
                label += "\\l";
            }

            vertices.emplace_back(idx, std::move(label));
            idx++;
        }

        return vertices;
    }

private:
    codegen::CFG const& data_;
};

} // namespace compiler::graph
