#include<iostream>
#include "raylib.h"
#include <string>
int main() {
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 900;
    const std::string title = "Plagiarism Detection System";

    InitWindow(screenWidth, screenHeight, title.c_str());
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Plagiarism Detection System", 180, 180, 30, DARKGRAY);
        DrawText("Project will be implemented soon.", 110, 240, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
