#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace pd {

class FileManager {
public:
    std::vector<std::filesystem::path> scanDirectory(const std::filesystem::path& dir);
    std::string readFile(const std::filesystem::path& path);
};

} // namespace pd
