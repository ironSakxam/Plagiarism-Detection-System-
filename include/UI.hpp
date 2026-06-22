#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace pd {

// Struct representing a single comparison item in the database view
struct ComparisonItem {
    std::filesystem::path path;
    double similarity;
    int tokenCount;
};

// ==============================================================================
// UI CLASS - Raylib User Interface Module
// ==============================================================================
// PURPOSE: Handle all graphics rendering and user interface using Raylib.
// Manages the dashboard, drawing interactive elements (buttons, cards, gauges),
// and capturing click events.
// ==============================================================================
class UI {
private:
    int screenWidth;   // Width of the application window
    int screenHeight;  // Height of the application window
    float scrollOffset;        // Scroll offset for the database list
    bool windowInitialized;    // Flag to track if window is ready
    std::string searchQuery;   // Search query string input
    int activeFilter;          // Selected sidebar filter (0 = All, 1 = High, 2 = Mod, 3 = Clean)

public:
    // Constructor - Initialize the UI object
    UI();
    
    // Initialize the Raylib window
    bool initialize(int width, int height, const std::string& title);
    
    // Render/draw the current frame
    // Returns selected file path from file dialogue if upload was triggered, or empty string.
    // Outputs selected index changes, reset clicks, import clicks, and delete clicks.
    std::string render(bool hasUploaded, 
                       const std::string& activeFile,
                       int activeTokens,
                       const std::vector<ComparisonItem>& items,
                       int selectedIndex,
                       int& outNewSelectedIndex,
                       bool& outResetRequested,
                       bool& outImportRequested,
                       bool& outDeleteRequested);
    
    // Cleanup and close the window
    void shutdown();
};

} // namespace pd

