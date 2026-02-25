#include "driver.hpp"

compiler::Flags parse(int argc, char** argv)
{
    compiler::Flags flags;
    for (int i = 1; i < argc; ++i)
    {
        std::string_view arg{ argv[i] };
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
        if (arg == "--parse")
        {
            flags.parse = true;
            continue;
        }
    }
    return flags;
}

int main(int argc, char* argv[])
{
    auto const flags = parse(argc, argv);

    if (flags.filename.empty())
    {
        std::cerr << "No file provided\n";
        exit(1);
    }

    compiler::Driver driver{ flags };
    driver.compile();
    return 0;
}
