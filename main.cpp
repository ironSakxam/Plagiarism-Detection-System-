#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Plagiarism Detection System");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Plagiarism Detection System", 180, 180, 30, DARKGRAY);
        DrawText("Raylib is already configured in the parent repo.", 110, 240, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
