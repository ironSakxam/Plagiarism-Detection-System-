#pragma once
#include <string>
#include <vector>
#include <filesystem>

namespace pd {

struct ComparisonItem {
    std::filesystem::path path;
    double similarity;
    int tokenCount;
};

class UI {
private:
    int screenWidth;
    int screenHeight;
    float scrollOffset;
    bool windowInitialized;
    std::string searchQuery;
    int activeFilter;

public:
    UI();
    bool initialize(int width, int height, const std::string& title);
    
    std::string render(bool hasUploaded, 
                       const std::string& activeFile,
                       int activeTokens,
                       const std::vector<ComparisonItem>& items,
                       int selectedIndex,
                       int& outNewSelectedIndex,
                       bool& outResetRequested,
                       bool& outImportRequested,
                       bool& outDeleteRequested);
                       
    void shutdown();
};

} // namespace pd

