// ==============================================================================
// UI.cpp - Raylib User Interface Module Implementation
// ==============================================================================
// PURPOSE: Implement a highly polished, premium 3-column File Manager layout.
// Columns:
//   - Sidebar (240px): Category filters with dynamic file counts, action buttons, system stats.
//   - File Explorer (500px): Interactive Search bar, scrollable list of files with vector icons, max matches, selection states.
//   - File Inspector (460px): Detailed audit comparison, Jaccard similarity gauge, badge, descriptions, and file deletion.
// ==============================================================================

#include "UI.hpp"           // Include UI header
#include "FileDialog.hpp"   // Include FileDialog helper
#include "raylib.h"         // Include Raylib graphics library
#include <iostream>         // For console output
#include <cstdio>           // For sprintf
#include <algorithm>        // For std::min, std::max
#include <cstring>          // For strlen

namespace pd {

// Initialize standard color palette (Slate & Indigo design system)
const Color COLOR_SIDEBAR_BG = Color{ 15, 23, 42, 255 };      // Slate 900
const Color COLOR_EXPLORER_BG = Color{ 30, 41, 59, 255 };     // Slate 800
const Color COLOR_INSPECTOR_BG = Color{ 15, 23, 42, 255 };    // Slate 900
const Color COLOR_ACCENT_INDIGO = Color{ 79, 70, 229, 255 };   // Indigo 600
const Color COLOR_ACCENT_INDIGO_HOVER = Color{ 99, 102, 241, 255 }; // Indigo 500
const Color COLOR_CARD_NORMAL = Color{ 15, 23, 42, 180 };     // Slate 900 transparent
const Color COLOR_CARD_HOVER = Color{ 51, 65, 85, 255 };      // Slate 700
const Color COLOR_TEXT_MUTED = Color{ 148, 163, 184, 255 };   // Slate 400
const Color COLOR_TEXT_BRIGHT = Color{ 248, 250, 252, 255 };  // Slate 50

// Colors for risk severity
const Color COLOR_RISK_HIGH = Color{ 239, 68, 68, 255 };      // Red 500
const Color COLOR_RISK_MOD = Color{ 245, 158, 11, 255 };      // Amber 500
const Color COLOR_RISK_LOW = Color{ 16, 185, 129, 255 };      // Emerald 500

// Helper to draw a modern vector document icon
static void drawFileIcon(float x, float y, Color col) {
    DrawRectangleLinesEx(Rectangle{ x, y, 14, 18 }, 1.5f, col);
    DrawLineEx(Vector2{ x + 9, y }, Vector2{ x + 9, y + 5 }, 1.5f, col);
    DrawLineEx(Vector2{ x + 9, y + 5 }, Vector2{ x + 14, y + 5 }, 1.5f, col);
    DrawLine(x + 3, y + 8, x + 11, y + 8, col);
    DrawLine(x + 3, y + 11, x + 11, y + 11, col);
    DrawLine(x + 3, y + 14, x + 8, y + 14, col);
}

// Helper to draw custom icons for sidebar categories
static void drawFilterIcon(int type, float x, float y, Color col) {
    if (type == 0) { // All Files Folder
        DrawRectangleLinesEx(Rectangle{ x, y + 3, 16, 12 }, 1.5f, col);
        DrawRectangleRounded(Rectangle{ x + 1, y, 6, 4 }, 0.5f, 4, col);
    } else if (type == 1) { // High Risk (Red Shield/Alert)
        DrawCircle(x + 8, y + 8, 7, COLOR_RISK_HIGH);
        DrawText("!", x + 6, y + 1, 13, WHITE);
    } else if (type == 2) { // Moderate Risk (Amber Shield/Alert)
        DrawCircle(x + 8, y + 8, 7, COLOR_RISK_MOD);
        DrawText("?", x + 5, y + 1, 13, WHITE);
    } else { // Clean Files (Green Checkmark)
        DrawCircle(x + 8, y + 8, 7, COLOR_RISK_LOW);
        DrawLineEx(Vector2{ x + 5, y + 8 }, Vector2{ x + 7, y + 10 }, 1.5f, WHITE);
        DrawLineEx(Vector2{ x + 7, y + 10 }, Vector2{ x + 11, y + 6 }, 1.5f, WHITE);
    }
}

// ==============================================================================
// CONSTRUCTOR: UI::UI
// ==============================================================================
UI::UI() {
    screenWidth = 0;
    screenHeight = 0;
    scrollOffset = 0.0f;
    windowInitialized = false;
    searchQuery = "";
    activeFilter = 0;
}

// ==============================================================================
// FUNCTION: UI::initialize
// ==============================================================================
bool UI::initialize(int width, int height, const std::string& title) {
    screenWidth = width;
    screenHeight = height;
    InitWindow(screenWidth, screenHeight, title.c_str());
    SetTargetFPS(60);
    windowInitialized = true;
    
    std::cout << "UI: Window initialized successfully (" << width << "x" << height << ")" << std::endl;
    return true;
}

// ==============================================================================
// FUNCTION: UI::render
// ==============================================================================
std::string UI::render(bool hasUploaded, 
                       const std::string& activeFile,
                       int activeTokens,
                       const std::vector<ComparisonItem>& items,
                       int selectedIndex,
                       int& outNewSelectedIndex,
                       bool& outResetRequested,
                       bool& outImportRequested,
                       bool& outDeleteRequested) {
    
    std::string uploadedFilePath = "";
    outResetRequested = false;
    outImportRequested = false;
    outDeleteRequested = false;
    bool anyHovered = false;

    // Reset scroll if filters change
    static int lastFilter = 0;
    if (activeFilter != lastFilter) {
        scrollOffset = 0.0f;
        lastFilter = activeFilter;
    }

    // ==========================================
    // SIDEBAR METRIC CALCULATIONS
    // ==========================================
    int totalCount = items.size();
    int highCount = 0;
    int modCount = 0;
    int cleanCount = 0;
    double totalSim = 0.0;
    for (const auto& item : items) {
        totalSim += item.similarity;
        if (item.similarity >= 0.6) highCount++;
        else if (item.similarity >= 0.3) modCount++;
        else cleanCount++;
    }
    double avgSim = totalCount > 0 ? (totalSim / totalCount) : 0.0;

    // ==========================================
    // FILTER AND SEARCH LIST POPULATION
    // ==========================================
    struct FilteredItem {
        int originalIndex;
        const ComparisonItem* item;
    };
    
    std::vector<FilteredItem> filteredItems;
    for (int i = 0; i < (int)items.size(); i++) {
        // Search filter (case-insensitive substring match)
        if (!searchQuery.empty()) {
            std::string nameLower = items[i].path.filename().string();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string queryLower = searchQuery;
            std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
            if (nameLower.find(queryLower) == std::string::npos) {
                continue;
            }
        }
        
        // Category filter
        if (activeFilter == 1 && items[i].similarity < 0.6) continue;
        if (activeFilter == 2 && (items[i].similarity < 0.3 || items[i].similarity >= 0.6)) continue;
        if (activeFilter == 3 && items[i].similarity >= 0.3) continue;

        filteredItems.push_back({ i, &items[i] });
    }

    // ==========================================
    // KEYBOARD INPUT FOR SEARCH BAR
    // ==========================================
    static bool searchFocused = false;
    Vector2 mousePos = GetMousePosition();
    Rectangle searchBarRec = { 260, 20, 460, 40 };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        searchFocused = CheckCollisionPointRec(mousePos, searchBarRec);
    }

    if (searchFocused) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (searchQuery.length() < 30)) {
                searchQuery += static_cast<char>(key);
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !searchQuery.empty()) {
            searchQuery.pop_back();
        }
    }

    // ==========================================
    // EXPLORER VIEW SCROLLBAR LOGIC
    // ==========================================
    float itemHeight = 64.0f; // Row height + spacing
    float totalHeight = filteredItems.size() * itemHeight;
    float visibleHeight = 670.0f; // Scrollable area height
    float maxScroll = std::max(0.0f, totalHeight - visibleHeight);

    // Scroll wheel input
    float wheel = GetMouseWheelMove();
    scrollOffset -= wheel * 35.0f;
    if (scrollOffset < 0.0f) scrollOffset = 0.0f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    // Scrollbar drag logic
    static bool draggingScrollbar = false;
    Rectangle trackRec = { 720, 110, 6, visibleHeight };
    
    float handleHeight = (visibleHeight / std::max(1.0f, totalHeight)) * visibleHeight;
    if (handleHeight < 40.0f) handleHeight = 40.0f;
    if (handleHeight > visibleHeight) handleHeight = visibleHeight;

    float handleY = 110.0f + (maxScroll > 0.0f ? (scrollOffset / maxScroll) * (visibleHeight - handleHeight) : 0.0f);
    Rectangle handleRec = { 720, handleY, 6, handleHeight };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (maxScroll > 0.0f && (CheckCollisionPointRec(mousePos, handleRec) || CheckCollisionPointRec(mousePos, trackRec))) {
            draggingScrollbar = true;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        draggingScrollbar = false;
    }

    if (draggingScrollbar && maxScroll > 0.0f) {
        anyHovered = true;
        float relativeY = mousePos.y - 110.0f - handleHeight / 2.0f;
        float ratio = relativeY / (visibleHeight - handleHeight);
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        scrollOffset = ratio * maxScroll;
    }

    // ==========================================
    // DRAWING THE CORE WINDOW FRAME
    // ==========================================
    BeginDrawing();
    
    // Clear screen
    ClearBackground(COLOR_SIDEBAR_BG);

    // HELPER: RENDER MODERN BUTTONS
    auto drawButton = [&](Rectangle rect, const char* text, Color baseColor, Color hoverColor, Color textColor, float roundness = 0.15f) -> bool {
        bool hovered = CheckCollisionPointRec(mousePos, rect);
        Color col = hovered ? hoverColor : baseColor;
        if (hovered) anyHovered = true;
        
        DrawRectangleRounded(rect, roundness, 4, col);
        int fontSize = 14;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, rect.x + (rect.width - textWidth) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, textColor);
        
        return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    };

    // HELPER: RENDER ICON TABS IN SIDEBAR
    auto drawSidebarTab = [&](int filterIndex, const char* label, int count, int typeY) -> bool {
        Rectangle tabRec = { 15, static_cast<float>(typeY), 210, 38 };
        bool hovered = CheckCollisionPointRec(mousePos, tabRec);
        bool isSelected = (activeFilter == filterIndex);
        
        Color bgCol = isSelected ? COLOR_ACCENT_INDIGO : (hovered ? Color{ 30, 41, 59, 120 } : BLANK);
        Color textCol = (isSelected || hovered) ? COLOR_TEXT_BRIGHT : COLOR_TEXT_MUTED;
        
        if (hovered) anyHovered = true;

        DrawRectangleRounded(tabRec, 0.2f, 4, bgCol);
        drawFilterIcon(filterIndex, tabRec.x + 12, tabRec.y + 11, textCol);
        DrawText(label, tabRec.x + 38, tabRec.y + 12, 13, textCol);
        
        // Render file count indicator badge
        char numStr[12];
        sprintf(numStr, "%d", count);
        int badgeW = MeasureText(numStr, 11) + 12;
        Rectangle badgeRec = { tabRec.x + tabRec.width - badgeW - 10, tabRec.y + 11, static_cast<float>(badgeW), 16 };
        Color badgeBg = isSelected ? Color{ 255, 255, 255, 40 } : Color{ 30, 41, 59, 255 };
        DrawRectangleRounded(badgeRec, 0.5f, 4, badgeBg);
        DrawText(numStr, badgeRec.x + 6, badgeRec.y + 3, 11, textCol);

        return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    };

    // ==============================================================================
    // COLUMN 1: SIDEBAR (WIDTH: 240px, Slate 900 Background)
    // ==============================================================================
    DrawRectangle(0, 0, 240, screenHeight, COLOR_SIDEBAR_BG);
    DrawLine(240, 0, 240, screenHeight, Color{ 30, 41, 59, 255 }); // Explorer boundary

    // Title / Brand
    DrawCircle(35, 38, 14, COLOR_ACCENT_INDIGO);
    DrawLineEx(Vector2{ 30, 38 }, Vector2{ 35, 43 }, 2.0f, WHITE);
    DrawLineEx(Vector2{ 35, 43 }, Vector2{ 42, 33 }, 2.0f, WHITE);
    DrawText("Plagiarism", 60, 22, 17, COLOR_TEXT_BRIGHT);
    DrawText("FILE MANAGER", 60, 42, 10, COLOR_TEXT_MUTED);

    // Categories Title
    DrawText("CATEGORIES", 20, 95, 11, COLOR_TEXT_MUTED);
    if (drawSidebarTab(0, "All Files", totalCount, 115)) activeFilter = 0;
    if (drawSidebarTab(1, "High Risk", highCount, 158)) activeFilter = 1;
    if (drawSidebarTab(2, "Mod Risk", modCount, 201)) activeFilter = 2;
    if (drawSidebarTab(3, "Clean Files", cleanCount, 244)) activeFilter = 3;

    // Actions Title
    DrawText("ACTIONS", 20, 305, 11, COLOR_TEXT_MUTED);
    
    // Audit New File button
    if (drawButton(Rectangle{ 15, 325, 210, 38 }, "📤 Audit External File", COLOR_ACCENT_INDIGO, COLOR_ACCENT_INDIGO_HOVER, WHITE)) {
        uploadedFilePath = openFileDialog();
    }

    // Import File to DB button
    if (drawButton(Rectangle{ 15, 373, 210, 38 }, "➕ Import File to DB", Color{ 16, 185, 129, 200 }, COLOR_RISK_LOW, WHITE)) {
        outImportRequested = true;
    }

    // Reset Dashboard (conditional, only if has uploaded)
    if (hasUploaded) {
        if (drawButton(Rectangle{ 15, 421, 210, 38 }, "🔄 Reset Dashboard", Color{ 239, 68, 68, 200 }, COLOR_RISK_HIGH, WHITE)) {
            outResetRequested = true;
        }
    }

    // Statistics Box at Bottom
    Rectangle statsRec = { 15, 600, 210, 150 };
    DrawRectangleRounded(statsRec, 0.1f, 4, Color{ 30, 41, 59, 120 });
    DrawRectangleRoundedLinesEx(statsRec, 0.1f, 4, 1.0f, Color{ 51, 65, 85, 255 });
    
    DrawText("DATABASE STATISTICS", statsRec.x + 15, statsRec.y + 15, 11, COLOR_TEXT_MUTED);
    
    DrawText("Total Files:", statsRec.x + 15, statsRec.y + 40, 13, COLOR_TEXT_MUTED);
    char totalStr[10]; sprintf(totalStr, "%d", totalCount);
    DrawText(totalStr, statsRec.x + 140, statsRec.y + 40, 13, COLOR_TEXT_BRIGHT);

    DrawText("Avg. Similarity:", statsRec.x + 15, statsRec.y + 65, 13, COLOR_TEXT_MUTED);
    char avgStr[20]; sprintf(avgStr, "%.1f%%", avgSim * 100.0);
    DrawText(avgStr, statsRec.x + 140, statsRec.y + 65, 13, avgSim >= 0.6 ? COLOR_RISK_HIGH : (avgSim >= 0.3 ? COLOR_RISK_MOD : COLOR_RISK_LOW));

    DrawText("System Health:", statsRec.x + 15, statsRec.y + 90, 13, COLOR_TEXT_MUTED);
    const char* healthStr = highCount > 0 ? "Warning" : "Safe";
    Color healthCol = highCount > 0 ? COLOR_RISK_HIGH : COLOR_RISK_LOW;
    DrawText(healthStr, statsRec.x + 140, statsRec.y + 90, 13, healthCol);

    DrawText("ESC to Exit application", 15, screenHeight - 25, 10, COLOR_TEXT_MUTED);

    // ==============================================================================
    // COLUMN 2: FILE EXPLORER (WIDTH: 500px, Slate 800 Background)
    // ==============================================================================
    DrawRectangle(240, 0, 500, screenHeight, COLOR_EXPLORER_BG);
    DrawLine(740, 0, 740, screenHeight, Color{ 51, 65, 85, 255 }); // Inspector boundary

    // Search Bar Box
    Color searchBorderCol = searchFocused ? COLOR_ACCENT_INDIGO : Color{ 51, 65, 85, 255 };
    DrawRectangleRounded(searchBarRec, 0.15f, 4, COLOR_SIDEBAR_BG);
    DrawRectangleRoundedLinesEx(searchBarRec, 0.15f, 4, 1.5f, searchBorderCol);
    if (CheckCollisionPointRec(mousePos, searchBarRec)) anyHovered = true;

    // Search placeholder / text rendering
    if (searchQuery.empty()) {
        DrawText("🔍 Search files by name...", searchBarRec.x + 12, searchBarRec.y + 13, 14, COLOR_TEXT_MUTED);
    } else {
        DrawText(searchQuery.c_str(), searchBarRec.x + 12, searchBarRec.y + 13, 14, COLOR_TEXT_BRIGHT);
        // Blinking cursor if focused
        if (searchFocused && (static_cast<int>(GetTime() * 2.0) % 2 == 0)) {
            int qWidth = MeasureText(searchQuery.c_str(), 14);
            DrawRectangle(searchBarRec.x + 14 + qWidth, searchBarRec.y + 12, 2, 16, COLOR_ACCENT_INDIGO);
        }
    }

    // Explorer Column Headers
    DrawText("File Name", 260, 82, 11, COLOR_TEXT_MUTED);
    DrawText("Similarity", 665, 82, 11, COLOR_TEXT_MUTED);
    DrawLine(260, 98, 720, 98, Color{ 51, 65, 85, 180 });

    // Scrollbar display
    if (maxScroll > 0.0f) {
        DrawRectangleRounded(trackRec, 1.0f, 4, Color{ 15, 23, 42, 80 });
        DrawRectangleRounded(handleRec, 1.0f, 4, draggingScrollbar ? COLOR_ACCENT_INDIGO : Color{ 71, 85, 105, 255 });
        if (CheckCollisionPointRec(mousePos, handleRec)) anyHovered = true;
    }

    // Render list of files in the scrollable view
    BeginScissorMode(250, 110, 465, visibleHeight);
    
    float startCardY = 110.0f - scrollOffset;
    
    if (filteredItems.empty()) {
        DrawText("No files match the search filters.", 270, 150, 14, COLOR_TEXT_MUTED);
    } else {
        for (int idx = 0; idx < (int)filteredItems.size(); idx++) {
            Rectangle cardRec = { 260, startCardY + idx * itemHeight, 450, 56 };
            
            // Render Culling
            if (cardRec.y + cardRec.height < 110.0f || cardRec.y > 780.0f) {
                continue;
            }

            bool mouseInScissor = (mousePos.x >= 250 && mousePos.x <= 715 && mousePos.y >= 110 && mousePos.y <= 780);
            bool hovered = mouseInScissor && CheckCollisionPointRec(mousePos, cardRec);
            bool isSelected = (filteredItems[idx].originalIndex == selectedIndex);

            Color cardBg = isSelected ? Color{ 79, 70, 229, 40 } : (hovered ? COLOR_CARD_HOVER : COLOR_CARD_NORMAL);
            Color borderCol = isSelected ? COLOR_ACCENT_INDIGO : (hovered ? Color{ 100, 116, 139, 255 } : Color{ 15, 23, 42, 255 });
            
            if (hovered) anyHovered = true;

            // Draw card layout
            DrawRectangleRounded(cardRec, 0.15f, 4, cardBg);
            DrawRectangleRoundedLinesEx(cardRec, 0.15f, 4, isSelected ? 2.0f : 1.0f, borderCol);

            // Click check
            if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                outNewSelectedIndex = filteredItems[idx].originalIndex;
            }

            // Draw File Icon & text details inside the card
            Color itemIconCol = isSelected ? COLOR_ACCENT_INDIGO_HOVER : COLOR_TEXT_MUTED;
            drawFileIcon(cardRec.x + 16, cardRec.y + 19, itemIconCol);

            std::string filename = filteredItems[idx].item->path.filename().string();
            std::string dispName = filename.length() > 28 ? filename.substr(0, 25) + "..." : filename;
            DrawText(dispName.c_str(), cardRec.x + 42, cardRec.y + 13, 14, COLOR_TEXT_BRIGHT);

            char tokensStr[64];
            sprintf(tokensStr, "%d structural nodes", filteredItems[idx].item->tokenCount);
            DrawText(tokensStr, cardRec.x + 42, cardRec.y + 33, 11, COLOR_TEXT_MUTED);

            // Severity colored badge indicators on card
            double simVal = filteredItems[idx].item->similarity;
            Color simCol = simVal >= 0.6 ? COLOR_RISK_HIGH : (simVal >= 0.3 ? COLOR_RISK_MOD : COLOR_RISK_LOW);
            
            char percentStr[24];
            sprintf(percentStr, "%.1f%%", simVal * 100.0);
            int percentWidth = MeasureText(percentStr, 13);
            DrawText(percentStr, cardRec.x + 434 - percentWidth, cardRec.y + 21, 13, simCol);
            
            // Progress Bar representing match
            Rectangle bar = { cardRec.x + 290, cardRec.y + 26, 70, 5 };
            DrawRectangleRounded(bar, 0.5f, 4, Color{ 15, 23, 42, 255 });
            DrawRectangleRounded(Rectangle{ bar.x, bar.y, static_cast<float>(bar.width * simVal), bar.height }, 0.5f, 4, simCol);
        }
    }
    
    EndScissorMode();

    // ==============================================================================
    // COLUMN 3: FILE INSPECTOR (WIDTH: 460px, Slate 900 Background)
    // ==============================================================================
    DrawRectangle(740, 0, 460, screenHeight, COLOR_INSPECTOR_BG);

    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        const auto& selectedItem = items[selectedIndex];
        std::string filename = selectedItem.path.filename().string();

        DrawText("FILE INSPECTOR", 765, 20, 11, COLOR_TEXT_MUTED);
        
        std::string dispName = filename.length() > 24 ? filename.substr(0, 21) + "..." : filename;
        DrawText(dispName.c_str(), 765, 42, 20, COLOR_TEXT_BRIGHT);
        
        std::string absolutePathStr = selectedItem.path.string();
        std::string dispPath = absolutePathStr.length() > 56 ? "..." + absolutePathStr.substr(absolutePathStr.length() - 53) : absolutePathStr;
        DrawText(dispPath.c_str(), 765, 70, 10, COLOR_TEXT_MUTED);

        // General file stats card
        Rectangle infoBox = { 765, 95, 410, 58 };
        DrawRectangleRounded(infoBox, 0.15f, 4, Color{ 30, 41, 59, 100 });
        DrawRectangleRoundedLinesEx(infoBox, 0.15f, 4, 1.0f, Color{ 51, 65, 85, 120 });
        
        DrawText("FILE PROFILE", infoBox.x + 12, infoBox.y + 10, 10, COLOR_TEXT_MUTED);
        char nodesText[80];
        sprintf(nodesText, "Tokens: %d structural nodes  |  Format: C++ Source File", selectedItem.tokenCount);
        DrawText(nodesText, infoBox.x + 12, infoBox.y + 28, 12, COLOR_TEXT_BRIGHT);

        DrawLine(765, 172, 1175, 172, Color{ 51, 65, 85, 120 });

        // Plagiarism report section
        DrawText("AUDIT REPORT", 765, 190, 11, COLOR_TEXT_MUTED);
        
        // Active file description
        std::string activeName = std::filesystem::path(activeFile).filename().string();
        std::string dispActiveName = activeName.length() > 22 ? activeName.substr(0, 19) + "..." : activeName;
        
        DrawText("Compared With Source Code:", 765, 215, 13, COLOR_TEXT_MUTED);
        DrawText(dispActiveName.c_str(), 765, 235, 15, COLOR_TEXT_BRIGHT);

        // Circular Gauge
        Vector2 gaugeCenter = { 970.0f, 375.0f };
        float similarityVal = selectedItem.similarity;
        Color riskCol = similarityVal >= 0.6 ? COLOR_RISK_HIGH : (similarityVal >= 0.3 ? COLOR_RISK_MOD : COLOR_RISK_LOW);
        
        DrawCircleSector(gaugeCenter, 72.0f, 0, 360, 40, COLOR_SIDEBAR_BG);
        DrawRing(gaugeCenter, 58.0f, 72.0f, 0.0f, 360.0f, 40, Color{ 30, 41, 59, 255 }); // Background ring
        DrawRing(gaugeCenter, 58.0f, 72.0f, -90.0f, -90.0f + static_cast<float>(similarityVal * 360.0f), 40, riskCol); // Animated match sweep

        // Percentage text inside ring
        char matchPercentStr[32];
        sprintf(matchPercentStr, "%.1f%%", similarityVal * 100.0);
        int percentW = MeasureText(matchPercentStr, 22);
        DrawText(matchPercentStr, gaugeCenter.x - percentW / 2.0f, gaugeCenter.y - 11.0f, 22, COLOR_TEXT_BRIGHT);

        // Risk Category Badge
        const char* riskTitle = "Unique Blueprint";
        const char* riskDesc = "No copy-pasting detected. The logical blueprints of these files share minimal structural overlap. The designs are independent.";
        
        if (similarityVal >= 0.8) {
            riskTitle = "Definite Plagiarism";
            riskDesc = "These files look almost identical. The underlying structures match exactly, suggesting the code was copied and variable names or comments were simply changed.";
        } else if (similarityVal >= 0.6) {
            riskTitle = "High Similarity";
            riskDesc = "Significant logic paths are structured identically. It is highly likely that sections of this code were copy-pasted directly.";
        } else if (similarityVal >= 0.3) {
            riskTitle = "Moderate Similarity";
            riskDesc = "Some algorithms share similar constructs. This could represent standard boilerplate templates or helper libraries, but review is advised.";
        }

        Rectangle badgeRec = { 765, 475, 410, 36 };
        Color badgeBg = { riskCol.r, riskCol.g, riskCol.b, 20 };
        DrawRectangleRounded(badgeRec, 0.2f, 4, badgeBg);
        DrawRectangleRoundedLinesEx(badgeRec, 0.2f, 4, 1.2f, riskCol);
        
        int riskW = MeasureText(riskTitle, 13);
        DrawText(riskTitle, badgeRec.x + (badgeRec.width - riskW) / 2.0f, badgeRec.y + 11, 13, riskCol);

        // Description Paragraph textwrap
        std::string descStr(riskDesc);
        size_t lineStart = 0;
        int lineY = 530;
        while (lineStart < descStr.size()) {
            size_t maxChars = 48;
            if (lineStart + maxChars >= descStr.size()) {
                std::string line = descStr.substr(lineStart);
                DrawText(line.c_str(), 765, lineY, 12, COLOR_TEXT_MUTED);
                break;
            } else {
                size_t space = descStr.find_last_of(' ', lineStart + maxChars);
                if (space == std::string::npos || space <= lineStart) {
                    space = lineStart + maxChars;
                }
                std::string line = descStr.substr(lineStart, space - lineStart);
                DrawText(line.c_str(), 765, lineY, 12, COLOR_TEXT_MUTED);
                lineStart = space + 1;
                lineY += 16;
            }
        }

        // Delete File Button (Bottom Action)
        Rectangle deleteBtnRec = { 765, 710, 410, 44 };
        if (drawButton(deleteBtnRec, "🗑️ Delete File from Database", Color{ 153, 27, 27, 255 }, COLOR_RISK_HIGH, WHITE)) {
            outDeleteRequested = true;
        }

    } else {
        // Fallback state if no file is selected
        DrawText("FILE INSPECTOR", 765, 20, 11, COLOR_TEXT_MUTED);
        DrawText("No File Selected", 765, 45, 18, COLOR_TEXT_BRIGHT);
        DrawText("Please choose a document from the File Explorer\nlist on the left to analyze its comparison report.", 765, 80, 13, COLOR_TEXT_MUTED);
    }

    // Set interactive pointing cursor when hovering clickable items
    if (anyHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    EndDrawing();

    return uploadedFilePath;
}

// ==============================================================================
// FUNCTION: UI::shutdown
// ==============================================================================
void UI::shutdown() {
    CloseWindow();
    windowInitialized = false;
    std::cout << "UI: Window shutdown completed successfully." << std::endl;
}

} // namespace pd
