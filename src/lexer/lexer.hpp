#pragma once
#include "DFMA.hpp"
#include "loc.hpp"
#include "token.hpp"
#include <optional>

namespace compiler
{

class Lexer
{
    struct BufferedPeek
    {
        Loc loc;
        size_t index;
        tokens::Token token;
    };

public:
    explicit Lexer(std::string_view filename, std::string_view fc) :
        loc_{ filename, 0, 0 },
        file_content_{ fc }
    {
    }

    tokens::Token peek() const;
    tokens::Token advance();

    Loc const& location() const { return loc_; }

private:
    BufferedPeek peek_with_offset() const;
    tokens::Token create(size_t starting, size_t current, DFMAState state) const;
    bool is_eof() const { return index_ >= file_content_.size(); }

    static constexpr DFMATable table{};

    mutable std::optional<BufferedPeek> buffered_;
    Loc loc_;
    size_t index_{ 0 };
    std::string_view file_content_;
};

} // namespace compiler
