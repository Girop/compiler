#pragma once
#include "DFMA.hpp"
#include "loc.hpp"
#include "token.hpp"
#include <optional>

namespace compiler
{

class Lexer
{
    struct FilePos 
    {
        Loc loc;
        size_t index;

        void advance(char c)
        {
            loc.advance(c);
            ++index;
        }
    };

    struct BufferedPeek
    {
        FilePos pos;
        tokens::Token token;
    };

public:
    explicit Lexer(std::string_view filename, std::string_view fc) :
        position_{ Loc{filename} , 0},
        file_content_{ fc }
    {
    }

    tokens::Token peek() const;
    tokens::Token advance();

private:
    BufferedPeek peek_with_offset() const;
    tokens::Token create(FilePos const& starting_pos, size_t current, DFMAState state) const;
    bool is_eof(FilePos const& pos) const
    {
        return pos.index >= file_content_.size();
    }

    FilePos skip_whitespace() const;

    char peek_char(FilePos const& fp) const
    {
        return file_content_.at(fp.index);
    }

    static constexpr DFMATable table{};

    mutable std::optional<BufferedPeek> buffered_;
    FilePos position_ ;
    std::string_view file_content_;
};

} // namespace compiler
