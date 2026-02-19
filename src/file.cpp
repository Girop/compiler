#include "file.hpp"
#include <fstream>
#include <iostream>

namespace compiler
{
namespace
{

std::string load_file(std::filesystem::path const& path)
{
    std::stringstream contents;
    std::ifstream file{ path };
    if(!file.is_open())
        {
            std::cerr << "compiler error: file not found\n";
            exit(1);
        }
    contents << file.rdbuf();
    return contents.str();
}

} // namespace

File::File(std::filesystem::path const& path) :
    name{ path.filename() },
    content{ load_file(path) }
{
}

} // namespace compiler
