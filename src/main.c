
#include "raylib.h"
#include <stdio.h>

#include "config.h"
#include "mario.h"
#include "blocks/block_basic.h"

int main()
{
    // original resolution * 3
    const int screenWidth = 256 * FACTOR;
    const int screenHeight = 224 * FACTOR;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    Mario mario = initMario(SMALL);
    BlockBasic blockBasic = initBlockBasic((Vector2){ 100, 100 });

    printf("%i %i\n", blockBasic.width, blockBasic.height);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            updateMario(&mario);
            drawMario(&mario);

            updateBlockBasic(&blockBasic);
            drawBlockBasic(&blockBasic);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}