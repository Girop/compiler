#include "file.hpp"
#include <fstream>
#include <iostream>

namespace compiler
{

File File::read(std::filesystem::path const& path)
{
    std::stringstream contents;
    std::ifstream file{ path };
    if (!file.is_open())
    {
        std::cerr << "compiler error: file not found\n";
        exit(1);
    }

    contents << file.rdbuf();
    return { path.filename().string(), contents.str() };
}

} // namespace compiler
