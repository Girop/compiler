#include "file.hpp"
#include "lexer/lexer.hpp"

struct Flags
{
    std::string filename;
    bool lex{false};
};

Flags parse(int argc, char** argv)
{
    Flags flags;
    for (int i = 1; i < argc; ++i)
    {
        std::string_view arg {argv[i]};
        if (arg.substr(0, 2) != "--")
        {
           flags.filename = arg; 
           continue;
        }
        if (arg == "--lex")
        {
            flags.lex = true;
            continue;
        }
    }
    return flags;
}

int main(int argc, char* argv[])
{ 
    auto const flags = parse(argc, argv);
    compiler::File file {flags.filename};
    compiler::Lexer lex {file.name, file.content};
    if (flags.lex) 
    {
        for (auto tok = lex.advance(); tok.tag != compiler::tokens::Tag::EoF; tok = lex.advance())
        {
            std::cout << tok.specific_format() << '\n';
        }

    }
}
