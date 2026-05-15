#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace pd {

struct Document {
    std::filesystem::path path;
    std::string rawText;
    std::vector<std::string> tokens;
};

} // namespace pd
