
#include "raylib.h"

#include "config.h"
#include "game.h"
#include "texture_map.h"

int main()
{
    // original resolution * 3
    const int screenWidth = 256 * FACTOR;
    const int screenHeight = 224 * FACTOR;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    initTextureMap();
    if (!initGame()) {
        return 1;
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground((Color){ 92, 148, 252, 255 });

            updateGame();
            drawGame();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
