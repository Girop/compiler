#pragma once
#include <filesystem>

namespace compiler 
{
    
struct File 
{
    explicit File(std::filesystem::path const& path);

    std::string name;
    std::string content;
};

}
