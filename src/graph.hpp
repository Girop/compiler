#pragma once
#include <format>

namespace compiler::graph
{

struct Edge
{
    size_t from;
    size_t to;
};

struct Vertex
{
    size_t idx;
    std::string label;
};

template <typename T>
concept GraphAdapter = requires(const T& t) {
    { t.name() } -> std::convertible_to<std::string>;
    { t.vertices() } -> std::ranges::range;
    { t.edges() } -> std::ranges::range;
};

template <GraphAdapter Adapter> class GraphWriter
{
public:
    static void dump(std::ostream& os, Adapter const& graph)
    {
        os << std::format("digraph {}", graph.name()) << " {" << endline << graph_opts << endline << node_opts
           << endline << verticies(graph.vertices()) << endline << edges(graph.edges()) << "\n}\n";
    }

private:
    static constexpr std::string_view endline = "\n\t";
    static constexpr std::string_view graph_opts = "graph [fontname=\"Helvetica\", rankdir=TB]";
    static constexpr std::string_view node_opts
        = "node [shape=record, fontsize=10, style=filled, fillcolor=lightyellow]";

    template <typename Range>
    static std::string verticies(Range const& verticies)
    {
        std::string res;

        for (auto& [idx, label] : verticies)
        {
            res += std::format("{} [label=\"{}\"]", idx, label);
            res += "\n";
        }

        return res;
    }

    template <typename Range>
    static std::string edges(Range const& edges)
    {
        std::string res;
        for (auto& [from, to] : edges)
        {
            res += std::format("{} -> {}", from, to);
        }
        return res;
    }
};

} // namespace compiler::graph
