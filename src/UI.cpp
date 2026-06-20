// ==============================================================================
// UI.cpp - Raylib User Interface Module Implementation
// ==============================================================================
// PURPOSE: Implement all UI rendering and graphics display using Raylib.
// Overhauls the interface to feel warm, human-crafted, and high-contrast:
//   - Clear, friendly natural language labels ("File A", "File B", "Database Matches").
//   - Improved typography sizing and text contrast.
//   - Full dynamic scrollbar featuring mouse wheel and click-drag integration.
//   - Scissor clipping viewport to handle list scroll smoothly.
//   - Interactive hover highlights and customized circular progress rings.
// ==============================================================================

#include "UI.hpp"           // Include UI header
#include "FileDialog.hpp"   // Include FileDialog helper
#include "raylib.h"         // Include Raylib graphics library
#include <iostream>         // For console output
#include <cstdio>           // For sprintf
#include <algorithm>        // For std::min, std::max

namespace pd {

// ==============================================================================
// CONSTRUCTOR: UI::UI()
// ==============================================================================
UI::UI() {
    screenWidth = 0;
    screenHeight = 0;
    scrollOffset = 0.0f;
    windowInitialized = false;
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
                       bool& outAddToDbRequested) {
    
    std::string uploadedFilePath = "";
    outResetRequested = false;
    outAddToDbRequested = false;
    bool anyHovered = false;

    // Reset scroll if upload mode changes
    static bool lastHasUploaded = false;
    if (hasUploaded != lastHasUploaded) {
        scrollOffset = 0.0f;
        lastHasUploaded = hasUploaded;
    }

    // ========== DYNAMIC SCROLLBAR LOGIC ==========
    float totalHeight = items.size() * 82.0f;
    float visibleHeight = 760.0f; // height of scissor box
    float maxScroll = std::max(0.0f, totalHeight - visibleHeight);

    // Scroll wheel input
    float wheel = GetMouseWheelMove();
    scrollOffset -= wheel * 35.0f;
    if (scrollOffset < 0.0f) scrollOffset = 0.0f;
    if (scrollOffset > maxScroll) scrollOffset = maxScroll;

    // Drag-scrollbar drag state
    static bool draggingScrollbar = false;
    Rectangle trackRec = { 862, 155, 8, visibleHeight };
    
    float handleHeight = (visibleHeight / totalHeight) * visibleHeight;
    if (handleHeight < 40.0f) handleHeight = 40.0f;
    if (handleHeight > visibleHeight) handleHeight = visibleHeight;

    float handleY = 155.0f + (maxScroll > 0.0f ? (scrollOffset / maxScroll) * (visibleHeight - handleHeight) : 0.0f);
    Rectangle handleRec = { 862, handleY, 8, handleHeight };

    Vector2 mousePos = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (CheckCollisionPointRec(mousePos, handleRec) || CheckCollisionPointRec(mousePos, trackRec)) {
            draggingScrollbar = true;
        }
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        draggingScrollbar = false;
    }

    if (draggingScrollbar && maxScroll > 0.0f) {
        anyHovered = true;
        float relativeY = mousePos.y - 155.0f - handleHeight / 2.0f;
        float ratio = relativeY / (visibleHeight - handleHeight);
        if (ratio < 0.0f) ratio = 0.0f;
        if (ratio > 1.0f) ratio = 1.0f;
        scrollOffset = ratio * maxScroll;
    }

    // ========== BEGIN DRAWING FRAME ==========
    BeginDrawing();
    
    // Premium Warm Charcoal Dark background (Gray 900)
    ClearBackground(Color{ 17, 24, 39, 255 });

    // ========== HELPER LAMBDA: DRAW BUTTON ==========
    auto drawButton = [&](Rectangle rect, const char* text, Color baseColor, Color hoverColor, Color textColor, float roundness = 0.2f) -> bool {
        Vector2 mPos = GetMousePosition();
        bool hovered = CheckCollisionPointRec(mPos, rect);
        Color col = hovered ? hoverColor : baseColor;
        
        if (hovered) anyHovered = true;
        
        DrawRectangleRounded(rect, roundness, 4, col);
        
        int fontSize = 16;
        int textWidth = MeasureText(text, fontSize);
        DrawText(text, rect.x + (rect.width - textWidth) / 2, rect.y + (rect.height - fontSize) / 2, fontSize, textColor);
        
        return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    };

    // ========== DRAW HEADER BAR (Indigo / Dark Slate Accent) ==========
    DrawRectangle(0, 0, screenWidth, 80, Color{ 10, 15, 30, 255 });
    DrawRectangle(0, 78, screenWidth, 2, Color{ 67, 56, 202, 255 }); // Indigo 700 Accent line
    
    // Title & status counter
    DrawText("Code Plagiarism Guard", 30, 25, 24, WHITE);
    
    char countStr[100];
    sprintf(countStr, "Database: %d source files active", (int)items.size());
    DrawText(countStr, screenWidth - 280, 29, 15, Color{ 226, 232, 240, 255 }); // High contrast text

    // ========== LEFT COLUMN: COMPARISON PANEL (WIDTH: 335) ==========
    Rectangle leftPanelRec = { 25, 100, 335, 840 };
    DrawRectangleRounded(leftPanelRec, 0.03f, 4, Color{ 31, 41, 55, 255 }); // Gray 800
    
    DrawText("Selected Comparison", 45, 120, 18, Color{ 241, 245, 249, 255 }); // Slate 100

    // Button: Upload Code File
    Rectangle uploadBtnRec = { 45, 155, 295, 48 };
    if (drawButton(uploadBtnRec, "Upload File for Audit", Color{ 79, 70, 229, 255 }, Color{ 67, 56, 202, 255 }, WHITE, 0.15f)) {
        uploadedFilePath = openFileDialog();
    }

    // Button: Add file to Database (always visible, green accent)
    Rectangle addToDbBtnRec = { 45, 215, 295, 40 };
    if (drawButton(addToDbBtnRec, "+ Add File to Database", Color{ 5, 150, 105, 255 }, Color{ 4, 120, 87, 255 }, WHITE, 0.15f)) {
        outAddToDbRequested = true;
    }

    // Button: Reset (Custom upload state only)
    float activeSectionY = 270;
    if (hasUploaded) {
        Rectangle resetBtnRec = { 45, 265, 295, 36 };
        if (drawButton(resetBtnRec, "Reset Dashboard", Color{ 239, 68, 68, 255 }, Color{ 220, 38, 38, 255 }, WHITE, 0.15f)) {
            outResetRequested = true;
        }
        activeSectionY = 315;
    }

    // Active File Metadata
    std::string activeFileName = std::filesystem::path(activeFile).filename().string();
    DrawText("FILE A (UNDER REVIEW):", 45, activeSectionY, 12, Color{ 203, 213, 225, 255 }); // High contrast Slate 300
    
    std::string dispActiveFile = activeFileName.length() > 24 ? activeFileName.substr(0, 21) + "..." : activeFileName;
    DrawText(dispActiveFile.c_str(), 45, activeSectionY + 18, 18, WHITE);
    
    char tokenStr[100];
    sprintf(tokenStr, "%d structural nodes detected", activeTokens);
    DrawText(tokenStr, 45, activeSectionY + 40, 13, Color{ 203, 213, 225, 180 });

    // Target File Metadata (from selected card in database)
    float targetSectionY = activeSectionY + 75;
    DrawText("FILE B (COMPARED FROM DB):", 45, targetSectionY, 12, Color{ 203, 213, 225, 255 });
    
    double similarityVal = 0.0;
    int targetTokens = 0;
    std::string targetFileName = "N/A";
    
    if (selectedIndex >= 0 && selectedIndex < (int)items.size()) {
        targetFileName = items[selectedIndex].path.filename().string();
        similarityVal = items[selectedIndex].similarity;
        targetTokens = items[selectedIndex].tokenCount;
    }
    
    std::string dispTargetFile = targetFileName.length() > 24 ? targetFileName.substr(0, 21) + "..." : targetFileName;
    DrawText(dispTargetFile.c_str(), 45, targetSectionY + 18, 18, WHITE);
    
    char targetTokenStr[100];
    sprintf(targetTokenStr, "%d structural nodes detected", targetTokens);
    DrawText(targetTokenStr, 45, targetSectionY + 40, 13, Color{ 203, 213, 225, 180 });

    // Threshold Color Coding (Human readable text)
    Color simColor = Color{ 16, 185, 129, 255 }; // Green (< 30%)
    const char* simStatus = "Unique Code Structure";
    const char* simDesc = "No copy-pasting detected. The logical blueprints of these files share minimal structural overlap. The designs are independent.";
    
    if (similarityVal >= 0.8) {
        simColor = Color{ 239, 68, 68, 255 }; // Red
        simStatus = "Definite Copying";
        simDesc = "These files look almost identical. The underlying structures match exactly, suggesting the code was copied and variable names or comments were simply changed.";
    } else if (similarityVal >= 0.6) {
        simColor = Color{ 249, 115, 22, 255 }; // Orange
        simStatus = "High Similarity";
        simDesc = "Significant logic paths are structured identically. It is highly likely that sections of this code were copy-pasted directly.";
    } else if (similarityVal >= 0.3) {
        simColor = Color{ 245, 158, 11, 255 }; // Yellow
        simStatus = "Moderate Similarity";
        simDesc = "Some algorithms share similar constructs. This could represent standard boilerplate templates or helper libraries, but review is advised.";
    }

    // Similarity Circle Gauge with dark inner shadow backing
    Vector2 gaugeCenter = { 25 + 335 / 2.0f, targetSectionY + 165.0f };
    DrawCircleSector(gaugeCenter, 72.0f, 0, 360, 40, Color{ 17, 24, 39, 255 }); // Slate 900 backing
    DrawRing(gaugeCenter, 58.0f, 72.0f, 0.0f, 360.0f, 40, Color{ 31, 41, 55, 255 }); // background ring
    
    float sweepAngle = static_cast<float>(similarityVal * 360.0f);
    DrawRing(gaugeCenter, 58.0f, 72.0f, -90.0f, -90.0f + sweepAngle, 40, simColor); // sweep clockwise from top

    // Percentage text centered
    char scoreStr[50];
    sprintf(scoreStr, "%.1f%%", similarityVal * 100.0);
    int scoreTextWidth = MeasureText(scoreStr, 26);
    DrawText(scoreStr, gaugeCenter.x - scoreTextWidth / 2.0f, gaugeCenter.y - 12.0f, 26, WHITE);

    // Status Badge
    Rectangle badgeRec = { 45, targetSectionY + 260, 295, 36 };
    Color badgeBg = { simColor.r, simColor.g, simColor.b, 30 };
    DrawRectangleRounded(badgeRec, 0.2f, 4, badgeBg);
    DrawRectangleRoundedLinesEx(badgeRec, 0.2f, 4, 1.5f, simColor);
    
    int statusTextWidth = MeasureText(simStatus, 14);
    DrawText(simStatus, badgeRec.x + (badgeRec.width - statusTextWidth) / 2.0f, badgeRec.y + 11.0f, 14, simColor);

    // Description text (wrapped to panel margins)
    DrawRectangle(45, targetSectionY + 315, 295, 1, Color{ 55, 65, 81, 255 }); // separator line
    
    std::string descStr(simDesc);
    size_t lineStart = 0;
    int lineY = targetSectionY + 332;
    while (lineStart < descStr.size()) {
        size_t chars = 35;
        if (lineStart + chars >= descStr.size()) {
            std::string line = descStr.substr(lineStart);
            DrawText(line.c_str(), 45, lineY, 13, Color{ 226, 232, 240, 255 }); // High visibility text
            break;
        } else {
            size_t space = descStr.find_last_of(' ', lineStart + chars);
            if (space == std::string::npos || space <= lineStart) {
                space = lineStart + chars;
            }
            std::string line = descStr.substr(lineStart, space - lineStart);
            DrawText(line.c_str(), 45, lineY, 13, Color{ 226, 232, 240, 255 });
            lineStart = space + 1;
            lineY += 18;
        }
    }

    // ========== RIGHT COLUMN: MATCH DATABASE RESULTS (WIDTH: 490) ==========
    Rectangle rightPanelRec = { 385, 100, 490, 840 };
    DrawRectangleRounded(rightPanelRec, 0.03f, 4, Color{ 31, 41, 55, 255 }); // Gray 800
    
    DrawText("Database Match Results (Ranked)", 405, 120, 18, Color{ 241, 245, 249, 255 });

    // Draw scrollbar track & handle
    if (maxScroll > 0.0f) {
        DrawRectangleRounded(trackRec, 1.0f, 4, Color{ 17, 24, 39, 120 });
        DrawRectangleRounded(handleRec, 1.0f, 4, draggingScrollbar ? Color{ 99, 102, 241, 255 } : Color{ 75, 85, 99, 255 });
        if (CheckCollisionPointRec(mousePos, handleRec)) anyHovered = true;
    }

    // Draw cards within clipped viewport (scissor box)
    BeginScissorMode(395, 155, 458, visibleHeight);
    
    float startCardY = 155.0f - scrollOffset;
    for (int i = 0; i < (int)items.size(); i++) {
        Rectangle cardRec = { 405, startCardY + i * 82.0f, 440, 74 };
        
        // Culling: skip drawing cards off-screen
        if (cardRec.y + cardRec.height < 155.0f || cardRec.y > 915.0f) {
            continue;
        }
        
        bool mouseInScissor = (mousePos.x >= 395 && mousePos.x <= 853 && mousePos.y >= 155 && mousePos.y <= 915);
        bool hovered = mouseInScissor && CheckCollisionPointRec(mousePos, cardRec);
        bool isSelected = (i == selectedIndex);

        Color cardColor = Color{ 17, 24, 39, 255 }; // Slate 900
        if (hovered) {
            cardColor = Color{ 55, 65, 81, 255 }; // Gray 700 (bright hover)
            anyHovered = true;
        }

        // Draw Card Background
        DrawRectangleRounded(cardRec, 0.15f, 4, cardColor);
        
        // Selection/Hover outline
        if (isSelected) {
            DrawRectangleRoundedLinesEx(cardRec, 0.15f, 4, 2.5f, Color{ 99, 102, 241, 255 }); // Indigo 500
        } else if (hovered) {
            DrawRectangleRoundedLinesEx(cardRec, 0.15f, 4, 1.5f, Color{ 156, 163, 175, 150 }); // Gray 400
        }

        // Click handler
        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            outNewSelectedIndex = i;
        }

        // Database Filename & token counts
        std::string cardFile = items[i].path.filename().string();
        std::string dispCardFile = cardFile.length() > 22 ? cardFile.substr(0, 19) + "..." : cardFile;
        DrawText(dispCardFile.c_str(), cardRec.x + 18, cardRec.y + 15, 17, WHITE);
        
        char cardTokens[80];
        sprintf(cardTokens, "%d structural nodes", items[i].tokenCount);
        DrawText(cardTokens, cardRec.x + 18, cardRec.y + 42, 13, Color{ 209, 213, 219, 255 }); // Gray 300 (high contrast)

        // Threshold Color Code for Card Indicators
        Color cardThresholdCol = Color{ 16, 185, 129, 255 };
        if (items[i].similarity >= 0.8) cardThresholdCol = Color{ 239, 68, 68, 255 };
        else if (items[i].similarity >= 0.6) cardThresholdCol = Color{ 249, 115, 22, 255 };
        else if (items[i].similarity >= 0.3) cardThresholdCol = Color{ 245, 158, 11, 255 };

        // Similarity score indicator
        char cardPercent[40];
        sprintf(cardPercent, "%.1f%%", items[i].similarity * 100.0);
        int percentWidth = MeasureText(cardPercent, 18);
        DrawText(cardPercent, cardRec.x + 422 - percentWidth, cardRec.y + 15, 18, cardThresholdCol);

        // Progress bar inside the card
        Rectangle barRec = { cardRec.x + 235, cardRec.y + 44, 187, 8 };
        DrawRectangleRounded(barRec, 0.5f, 4, Color{ 17, 24, 39, 255 }); // track background
        
        int fillWidth = static_cast<int>(barRec.width * items[i].similarity);
        Rectangle fillBarRec = { barRec.x, barRec.y, static_cast<float>(fillWidth), barRec.height };
        DrawRectangleRounded(fillBarRec, 0.5f, 4, cardThresholdCol); // filled bar
    }
    
    EndScissorMode();

    // ========== FOOTER SECTION ==========
    DrawText("Press ESC to exit Plagiarism Guard", 25, screenHeight - 35, 14, Color{ 156, 163, 175, 255 });

    // Set active mouse hand cursor on hovers
    if (anyHovered) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // ========== END DRAWING FRAME ==========
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
