#include "FileManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace pd {

std::vector<std::filesystem::path> FileManager::scanDirectory(const std::filesystem::path& dir) {
    std::vector<std::filesystem::path> files;
    
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        std::cerr << "Error: Invalid directory path '" << dir << "'." << std::endl;
        return files;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            std::string extension = entry.path().extension().string();
            if (extension == ".cpp" || extension == ".txt") {
                files.push_back(entry.path());
            }
        }
    }
    
    return files;
}

std::string FileManager::readFile(const std::filesystem::path& path) {
    std::string content;
    std::ifstream file(path);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << path << "'." << std::endl;
        return content;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();
    
    std::cout << "Successfully read file: " << path << std::endl;
    return content;
}

} // namespace pd
