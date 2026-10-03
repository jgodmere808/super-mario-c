
#include "raylib.h"

#include "config.h"
#include "game.h"
#include "texture_map.h"

int main()
{
    // original resolution * 3
    const int screenWidth  = SCREEN_WIDTH;
    const int screenHeight = SCREEN_HEIGHT;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    InitAudioDevice();

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

    endGame();

    CloseAudioDevice();

    CloseWindow();

    return 0;
}
