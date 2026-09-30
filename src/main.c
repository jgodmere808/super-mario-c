
#include "raylib.h"

#include "config.h"
#include "mario.h"

int main()
{
    // original resolution * 3
    const int screenWidth = 256 * FACTOR;
    const int screenHeight = 224 * FACTOR;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    Mario mario = initMario(SMALL);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            updateMario(&mario);
            drawMario(&mario);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}