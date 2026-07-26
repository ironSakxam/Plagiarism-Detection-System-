#include "UI.hpp"
#include "FileDialog.hpp"
#include "raylib.h"
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>

namespace pd {

const Color COLOR_SIDEBAR_BG = Color{ 8, 8, 10, 255 };
const Color COLOR_EXPLORER_BG = Color{ 18, 18, 22, 255 };
const Color COLOR_INSPECTOR_BG = Color{ 8, 8, 10, 255 };
const Color COLOR_ACCENT_INDIGO = Color{ 0, 162, 255, 255 };
const Color COLOR_ACCENT_INDIGO_HOVER = Color{ 80, 200, 255, 255 };
const Color COLOR_CARD_NORMAL = Color{ 25, 25, 30, 255 };
const Color COLOR_CARD_HOVER = Color{ 38, 38, 46, 255 };
const Color COLOR_TEXT_MUTED = Color{ 155, 165, 180, 255 };
const Color COLOR_TEXT_BRIGHT = Color{ 255, 255, 255, 255 };

const Color COLOR_RISK_HIGH = Color{ 255, 75, 75, 255 };
const Color COLOR_RISK_MOD = Color{ 255, 179, 0, 255 };
const Color COLOR_RISK_LOW = Color{ 0, 224, 150, 255 };

static void drawFileIcon(float x, float y, Color col) {
    DrawRectangleLinesEx(Rectangle{ x, y, 14, 18 }, 1.5f, col);
    DrawLineEx(Vector2{ x + 9, y }, Vector2{ x + 9, y + 5 }, 1.5f, col);
    DrawLineEx(Vector2{ x + 9, y + 5 }, Vector2{ x + 14, y + 5 }, 1.5f, col);
    DrawLine(x + 3, y + 8, x + 11, y + 8, col);
    DrawLine(x + 3, y + 11, x + 11, y + 11, col);
    DrawLine(x + 3, y + 14, x + 8, y + 14, col);
}

static void drawFilterIcon(int type, float x, float y, Color col) {
    if (type == 0) {
        DrawRectangleLinesEx(Rectangle{ x, y + 3, 16, 12 }, 1.5f, col);
        DrawRectangleRounded(Rectangle{ x + 1, y, 6, 4 }, 0.5f, 4, col);
    } else if (type == 1) {
        DrawCircle(x + 8, y + 8, 7, COLOR_RISK_HIGH);
        DrawText("!", x + 6, y + 1, 13, WHITE);
    } else if (type == 2) {
        DrawCircle(x + 8, y + 8, 7, COLOR_RISK_MOD);
        DrawText("?", x + 5, y + 1, 13, WHITE);
    } else {
        DrawCircle(x + 8, y + 8, 7, COLOR_RISK_LOW);
        DrawLineEx(Vector2{ x + 5, y + 8 }, Vector2{ x + 7, y + 10 }, 1.5f, WHITE);
        DrawLineEx(Vector2{ x + 7, y + 10 }, Vector2{ x + 11, y + 6 }, 1.5f, WHITE);
    }
}

UI::UI() {
    screenWidth = 0;
    screenHeight = 0;
    scrollOffset = 0.0f;
    windowInitialized = false;
    searchQuery = "";
    activeFilter = 0;
}

bool UI::initialize(int width, int height, const std::string& title) {
    screenWidth = width;
    screenHeight = height;
    InitWindow(screenWidth, screenHeight, title.c_str());
    SetTargetFPS(60);
    windowInitialized = true;
    std::cout << "UI: Window initialized successfully (" << width << "x" << height << ")" << std::endl;
    return true;
}

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

    static int lastFilter = 0;
    if (activeFilter != lastFilter) {
        scrollOffset = 0.0f;
        lastFilter = activeFilter;
    }

    // Sidebar metric calculations
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

    // Filter and search populating
    struct FilteredItem {
        int originalIndex;
        const ComparisonItem* item;
    };
    
    std::vector<FilteredItem> filteredItems;
    for (int i = 0; i < (int)items.size(); i++) {
        if (!searchQuery.empty()) {
            std::string nameLower = items[i].path.filename().string();
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string queryLower = searchQuery;
            std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
            if (nameLower.find(queryLower) == std::string::npos) {
                continue;
            }
        }
        
        if (activeFilter == 1 && items[i].similarity < 0.6) continue;
        if (activeFilter == 2 && (items[i].similarity < 0.3 || items[i].similarity >= 0.6)) continue;
        if (activeFilter == 3 && items[i].similarity >= 0.3) continue;

        filteredItems.push_back({ i, &items[i] });
    }

    // Search bar focus & keyboard input
    static bool searchFocused = false;
    Vector2 mousePos = GetMousePosition();
    Rectangle searchBarRec = { 295, 22, 630, 46 };

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

    // Explorer scroll limits & controls
    float itemHeight = 72.0f;
    float totalHeight = filteredItems.size() * itemHeight;
    float visibleHeight = 870.0f;
    float maxScroll = std::max(0.0f, totalHeight - visibleHeight);

    float wheel = GetMouseWheelMove();
    scrollOffset -= wheel * 40.0f;
    if (scrollOffset < 0.0f) scrollOffset = 0.0f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    static bool draggingScrollbar = false;
    Rectangle trackRec = { 957, 118, 7, visibleHeight };
    
    float handleHeight = (visibleHeight / std::max(1.0f, totalHeight)) * visibleHeight;
    if (handleHeight < 40.0f) handleHeight = 40.0f;
    if (handleHeight > visibleHeight) handleHeight = visibleHeight;

    float handleY = 118.0f + (maxScroll > 0.0f ? (scrollOffset / maxScroll) * (visibleHeight - handleHeight) : 0.0f);
    Rectangle handleRec = { 957, handleY, 7, handleHeight };

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
        float relativeY = mousePos.y - 118.0f - handleHeight / 2.0f;
        float ratio = relativeY / (visibleHeight - handleHeight);
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        scrollOffset = ratio * maxScroll;
    }

    BeginDrawing();
    ClearBackground(COLOR_SIDEBAR_BG);

    auto drawButton = [&](Rectangle rect, const char* text, Color baseColor, Color hoverColor, Color textColor, float roundness = 0.15f) -> bool {
        bool hovered = CheckCollisionPointRec(mousePos, rect);
        Color col = hovered ? hoverColor : baseColor;
        if (hovered) anyHovered = true;
        
        DrawRectangleRounded(rect, roundness, 4, col);
        int fontSize = 17;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, rect.x + (rect.width - textWidth) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, textColor);
        
        return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    };

    auto drawSidebarTab = [&](int filterIndex, const char* label, int count, int typeY) -> bool {
        Rectangle tabRec = { 16, static_cast<float>(typeY), 248, 44 };
        bool hovered = CheckCollisionPointRec(mousePos, tabRec);
        bool isSelected = (activeFilter == filterIndex);
        
        Color bgCol = isSelected ? COLOR_ACCENT_INDIGO : (hovered ? Color{ 35, 35, 45, 120 } : BLANK);
        Color textCol = (isSelected || hovered) ? COLOR_TEXT_BRIGHT : COLOR_TEXT_MUTED;
        
        if (hovered) anyHovered = true;

        DrawRectangleRounded(tabRec, 0.2f, 4, bgCol);
        drawFilterIcon(filterIndex, tabRec.x + 14, tabRec.y + 13, textCol);
        DrawText(label, tabRec.x + 42, tabRec.y + 13, 16, textCol);
        
        char numStr[12];
        sprintf(numStr, "%d", count);
        int badgeW = MeasureText(numStr, 14) + 14;
        Rectangle badgeRec = { tabRec.x + tabRec.width - badgeW - 10, tabRec.y + 12, static_cast<float>(badgeW), 20 };
        Color badgeBg = isSelected ? Color{ 255, 255, 255, 40 } : Color{ 35, 35, 45, 255 };
        DrawRectangleRounded(badgeRec, 0.5f, 4, badgeBg);
        DrawText(numStr, badgeRec.x + 7, badgeRec.y + 3, 14, textCol);

        return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    };

    // ==========================================
    // COLUMN 1: SIDEBAR
    // ==========================================
    DrawRectangle(0, 0, 280, screenHeight, COLOR_SIDEBAR_BG);
    DrawLine(280, 0, 280, screenHeight, Color{ 35, 35, 42, 255 });

    DrawCircle(38, 42, 16, COLOR_ACCENT_INDIGO);
    DrawLineEx(Vector2{ 32, 42 }, Vector2{ 38, 48 }, 2.5f, WHITE);
    DrawLineEx(Vector2{ 38, 48 }, Vector2{ 46, 36 }, 2.5f, WHITE);
    DrawText("Plagiarism", 66, 22, 22, COLOR_TEXT_BRIGHT);
    DrawText("FILE MANAGER", 66, 48, 13, COLOR_TEXT_MUTED);

    DrawText("CATEGORIES", 20, 102, 14, COLOR_TEXT_MUTED);
    if (drawSidebarTab(0, "All Files",   totalCount, 124)) activeFilter = 0;
    if (drawSidebarTab(1, "High Risk",   highCount,  174)) activeFilter = 1;
    if (drawSidebarTab(2, "Mod Risk",    modCount,   224)) activeFilter = 2;
    if (drawSidebarTab(3, "Clean Files", cleanCount, 274)) activeFilter = 3;

    DrawText("ACTIONS", 20, 340, 14, COLOR_TEXT_MUTED);
    if (drawButton(Rectangle{ 16, 362, 248, 46 }, "Audit External File", COLOR_ACCENT_INDIGO, COLOR_ACCENT_INDIGO_HOVER, WHITE)) {
        uploadedFilePath = openFileDialog();
    }
    if (drawButton(Rectangle{ 16, 418, 248, 46 }, "Import File to DB", Color{ 0, 190, 120, 200 }, COLOR_RISK_LOW, WHITE)) {
        outImportRequested = true;
    }
    if (hasUploaded && drawButton(Rectangle{ 16, 474, 248, 46 }, "Reset Dashboard", Color{ 200, 40, 40, 200 }, COLOR_RISK_HIGH, WHITE)) {
        outResetRequested = true;
    }

    Rectangle statsRec = { 16, 730, 248, 200 };
    DrawRectangleRounded(statsRec, 0.1f, 4, Color{ 25, 25, 32, 120 });
    DrawRectangleRoundedLinesEx(statsRec, 0.1f, 4, 1.0f, Color{ 45, 45, 55, 255 });
    
    DrawText("DATABASE STATISTICS", statsRec.x + 14, statsRec.y + 16, 13, COLOR_TEXT_MUTED);
    
    DrawText("Total Files:", statsRec.x + 14, statsRec.y + 46, 16, COLOR_TEXT_MUTED);
    char totalStr[10]; sprintf(totalStr, "%d", totalCount);
    DrawText(totalStr, statsRec.x + 170, statsRec.y + 46, 16, COLOR_TEXT_BRIGHT);

    DrawText("Avg. Similarity:", statsRec.x + 14, statsRec.y + 80, 16, COLOR_TEXT_MUTED);
    char avgStr[20]; sprintf(avgStr, "%.1f%%", avgSim * 100.0);
    DrawText(avgStr, statsRec.x + 170, statsRec.y + 80, 16, avgSim >= 0.6 ? COLOR_RISK_HIGH : (avgSim >= 0.3 ? COLOR_RISK_MOD : COLOR_RISK_LOW));

    DrawText("System Health:", statsRec.x + 14, statsRec.y + 114, 16, COLOR_TEXT_MUTED);
    const char* healthStr = highCount > 0 ? "Warning" : "Safe";
    Color healthCol = highCount > 0 ? COLOR_RISK_HIGH : COLOR_RISK_LOW;
    DrawText(healthStr, statsRec.x + 170, statsRec.y + 114, 16, healthCol);

    DrawText("ESC to Exit", 16, screenHeight - 28, 13, COLOR_TEXT_MUTED);

    // ==========================================
    // COLUMN 2: FILE EXPLORER
    // ==========================================
    DrawRectangle(280, 0, 680, screenHeight, COLOR_EXPLORER_BG);
    DrawLine(960, 0, 960, screenHeight, Color{ 35, 35, 42, 255 });

    Color searchBorderCol = searchFocused ? COLOR_ACCENT_INDIGO : Color{ 45, 45, 55, 255 };
    DrawRectangleRounded(searchBarRec, 0.15f, 4, COLOR_SIDEBAR_BG);
    DrawRectangleRoundedLinesEx(searchBarRec, 0.15f, 4, 1.5f, searchBorderCol);
    if (CheckCollisionPointRec(mousePos, searchBarRec)) anyHovered = true;

    if (searchQuery.empty()) {
        DrawText("Search files by name...", searchBarRec.x + 14, searchBarRec.y + 13, 18, COLOR_TEXT_MUTED);
    } else {
        DrawText(searchQuery.c_str(), searchBarRec.x + 14, searchBarRec.y + 13, 18, COLOR_TEXT_BRIGHT);
        if (searchFocused && (static_cast<int>(GetTime() * 2.0) % 2 == 0)) {
            int qWidth = MeasureText(searchQuery.c_str(), 18);
            DrawRectangle(searchBarRec.x + 16 + qWidth, searchBarRec.y + 11, 2, 22, COLOR_ACCENT_INDIGO);
        }
    }

    DrawText("File Name", 300, 88, 14, COLOR_TEXT_MUTED);
    DrawText("Similarity", 890, 88, 14, COLOR_TEXT_MUTED);
    DrawLine(295, 108, 955, 108, Color{ 45, 45, 55, 180 });

    if (maxScroll > 0.0f) {
        DrawRectangleRounded(trackRec, 1.0f, 4, Color{ 8, 8, 10, 80 });
        DrawRectangleRounded(handleRec, 1.0f, 4, draggingScrollbar ? COLOR_ACCENT_INDIGO : Color{ 55, 55, 65, 255 });
        if (CheckCollisionPointRec(mousePos, handleRec)) anyHovered = true;
    }

    BeginScissorMode(285, 118, 668, visibleHeight);
    float startCardY = 118.0f - scrollOffset;
    
    if (filteredItems.empty()) {
        DrawText("No files match the search filters.", 310, 180, 16, COLOR_TEXT_MUTED);
    } else {
        for (int idx = 0; idx < (int)filteredItems.size(); idx++) {
            Rectangle cardRec = { 295, startCardY + idx * itemHeight, 650, 64 };
            if (cardRec.y + cardRec.height < 118.0f || cardRec.y > 990.0f) {
                continue;
            }

            bool mouseInScissor = (mousePos.x >= 285 && mousePos.x <= 953 && mousePos.y >= 118 && mousePos.y <= 988);
            bool hovered = mouseInScissor && CheckCollisionPointRec(mousePos, cardRec);
            bool isSelected = (filteredItems[idx].originalIndex == selectedIndex);

            Color cardBg = isSelected ? Color{ 0, 162, 255, 40 } : (hovered ? COLOR_CARD_HOVER : COLOR_CARD_NORMAL);
            Color borderCol = isSelected ? COLOR_ACCENT_INDIGO : (hovered ? Color{ 55, 55, 68, 255 } : Color{ 8, 8, 10, 255 });
            
            if (hovered) anyHovered = true;

            DrawRectangleRounded(cardRec, 0.12f, 4, cardBg);
            DrawRectangleRoundedLinesEx(cardRec, 0.12f, 4, isSelected ? 2.0f : 1.0f, borderCol);

            if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                outNewSelectedIndex = filteredItems[idx].originalIndex;
            }

            Color itemIconCol = isSelected ? COLOR_ACCENT_INDIGO_HOVER : COLOR_TEXT_MUTED;
            drawFileIcon(cardRec.x + 18, cardRec.y + 23, itemIconCol);

            std::string filename = filteredItems[idx].item->path.filename().string();
            std::string dispName = filename.length() > 34 ? filename.substr(0, 31) + "..." : filename;
            DrawText(dispName.c_str(), cardRec.x + 46, cardRec.y + 22, 18, COLOR_TEXT_BRIGHT);

            double simVal = filteredItems[idx].item->similarity;
            Color simCol = simVal >= 0.6 ? COLOR_RISK_HIGH : (simVal >= 0.3 ? COLOR_RISK_MOD : COLOR_RISK_LOW);
            
            char percentStr[24];
            sprintf(percentStr, "%.1f%%", simVal * 100.0);
            int percentWidth = MeasureText(percentStr, 18);
            DrawText(percentStr, cardRec.x + 632 - percentWidth, cardRec.y + 22, 18, simCol);
            
            Rectangle bar = { cardRec.x + 420, cardRec.y + 30, 100, 6 };
            DrawRectangleRounded(bar, 0.5f, 4, Color{ 8, 8, 10, 255 });
            DrawRectangleRounded(Rectangle{ bar.x, bar.y, static_cast<float>(bar.width * simVal), bar.height }, 0.5f, 4, simCol);
        }
    }
    EndScissorMode();

    // ==========================================
    // COLUMN 3: FILE INSPECTOR
    // ==========================================
    DrawRectangle(960, 0, 640, screenHeight, COLOR_INSPECTOR_BG);

    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        const auto& selectedItem = items[selectedIndex];
        std::string filename = selectedItem.path.filename().string();

        DrawText("FILE INSPECTOR", 990, 22, 15, COLOR_TEXT_MUTED);
        
        std::string dispName = filename.length() > 30 ? filename.substr(0, 27) + "..." : filename;
        DrawText(dispName.c_str(), 990, 46, 26, COLOR_TEXT_BRIGHT);
        
        std::string absolutePathStr = selectedItem.path.string();
        std::string dispPath = absolutePathStr.length() > 68 ? "..." + absolutePathStr.substr(absolutePathStr.length() - 65) : absolutePathStr;
        DrawText(dispPath.c_str(), 990, 80, 13, COLOR_TEXT_MUTED);

        DrawLine(990, 110, 1580, 110, Color{ 45, 45, 55, 120 });
        DrawText("AUDIT REPORT", 990, 126, 15, COLOR_TEXT_MUTED);
        
        std::string comparisonTargetName = selectedItem.path.filename().string();
        std::string dispComparisonTargetName = comparisonTargetName.length() > 30 ? comparisonTargetName.substr(0, 27) + "..." : comparisonTargetName;
        
        DrawText("Compared Against:", 990, 158, 16, COLOR_TEXT_MUTED);
        DrawText(dispComparisonTargetName.c_str(), 990, 182, 22, COLOR_TEXT_BRIGHT);

        float similarityVal = selectedItem.similarity;
        Color riskCol = similarityVal >= 0.6 ? COLOR_RISK_HIGH : (similarityVal >= 0.3 ? COLOR_RISK_MOD : COLOR_RISK_LOW);
        
        DrawText("Similarity Index:", 990, 228, 16, COLOR_TEXT_MUTED);
        char matchPercentStr[32];
        sprintf(matchPercentStr, "%.1f%%", similarityVal * 100.0);
        int percentW = MeasureText(matchPercentStr, 24);
        DrawText(matchPercentStr, 1565 - percentW, 225, 24, riskCol);

        Rectangle progressBarRec = { 990, 260, 582, 28 };
        DrawRectangleRounded(progressBarRec, 0.3f, 4, Color{ 25, 25, 32, 255 });
        if (similarityVal > 0.0f) {
            float fillWidth = progressBarRec.width * similarityVal;
            if (fillWidth < 18.0f) fillWidth = 18.0f;
            DrawRectangleRounded(Rectangle{ progressBarRec.x, progressBarRec.y, fillWidth, progressBarRec.height }, 0.3f, 4, riskCol);
        }

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

        Rectangle badgeRec = { 990, 308, 582, 46 };
        Color badgeBg = { riskCol.r, riskCol.g, riskCol.b, 20 };
        DrawRectangleRounded(badgeRec, 0.2f, 4, badgeBg);
        DrawRectangleRoundedLinesEx(badgeRec, 0.2f, 4, 1.5f, riskCol);
        
        int riskW = MeasureText(riskTitle, 18);
        DrawText(riskTitle, badgeRec.x + (badgeRec.width - riskW) / 2.0f, badgeRec.y + 14, 18, riskCol);

        std::string descStr(riskDesc);
        size_t lineStart = 0;
        int lineY = 375;
        while (lineStart < descStr.size()) {
            size_t maxChars = 52;
            if (lineStart + maxChars >= descStr.size()) {
                std::string line = descStr.substr(lineStart);
                DrawText(line.c_str(), 990, lineY, 16, COLOR_TEXT_MUTED);
                break;
            } else {
                size_t space = descStr.find_last_of(' ', lineStart + maxChars);
                if (space == std::string::npos || space <= lineStart) {
                    space = lineStart + maxChars;
                }
                std::string line = descStr.substr(lineStart, space - lineStart);
                DrawText(line.c_str(), 990, lineY, 16, COLOR_TEXT_MUTED);
                lineStart = space + 1;
                lineY += 24;
            }
        }

        Rectangle deleteBtnRec = { 990, 920, 582, 50 };
        if (drawButton(deleteBtnRec, "Delete File from Database", Color{ 153, 27, 27, 255 }, COLOR_RISK_HIGH, WHITE)) {
            outDeleteRequested = true;
        }

    } else {
        DrawText("FILE INSPECTOR", 990, 22, 15, COLOR_TEXT_MUTED);
        DrawText("No File Selected", 990, 50, 26, COLOR_TEXT_BRIGHT);
        DrawText("Choose a document from the File Explorer\nlist on the left to analyze its comparison.", 990, 92, 17, COLOR_TEXT_MUTED);
    }

    if (anyHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    EndDrawing();
    return uploadedFilePath;
}

void UI::shutdown() {
    CloseWindow();
    windowInitialized = false;
    std::cout << "UI: Window shutdown completed successfully." << std::endl;
}

} // namespace pd

