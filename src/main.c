
#include "raylib.h"

int main()
{
    // original resolution * 3
    const int screenWidth = 256 * 3;
    const int screenHeight = 224 * 3;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}