
#include "raylib.h"
#include <stdio.h>

#include "config.h"
#include "mario.h"
#include "blocks/block_brick.h"

int main()
{
    // original resolution * 3
    const int screenWidth = 256 * FACTOR;
    const int screenHeight = 224 * FACTOR;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    Mario mario = initMario(SMALL);
    BlockBrick blockBrick = initBlockBrick((Vector2){ 100, 100 });

    printf("%i %i\n", blockBrick.width, blockBrick.height);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);

            updateMario(&mario);
            drawMario(&mario);

            updateBlockBrick(&blockBrick);
            drawBlockBrick(&blockBrick);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
