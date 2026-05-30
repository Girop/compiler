#pragma once
#include <filesystem>

namespace compiler 
{
    
struct File 
{
    static File read(std::filesystem::path const& path);

    std::string name;
    std::string content;
};

}
