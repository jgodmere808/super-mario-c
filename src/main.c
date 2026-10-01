
#include "raylib.h"
#include <stdio.h>

#include "config.h"
#include "mario.h"
#include "blocks/block_brick.h"
#include "blocks/block_dirt.h"

int main()
{
    // original resolution * 3
    const int screenWidth = 256 * FACTOR;
    const int screenHeight = 224 * FACTOR;

    InitWindow(screenWidth, screenHeight, "Super Mario!");

    SetTargetFPS(60);

    Mario mario = initMario(SMALL);
    BlockBrick blockBrick = initBlockBrick((Vector2){ 100, 100 });
    BlockDirt blockDirt = initBlockDirt((Vector2){ 148, 100 });

    printf("%i %i\n", blockBrick.width, blockBrick.height);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground((Color){ 92, 148, 252, 255 });

            updateMario(&mario);
            drawMario(&mario);

            updateBlockBrick(&blockBrick);
            drawBlockBrick(&blockBrick);

            updateBlockDirt(&blockDirt);
            drawBlockDirt(&blockDirt);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
