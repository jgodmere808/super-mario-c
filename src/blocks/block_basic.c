
#include "block_basic.h"

BlockBasic initBlockBasic(Vector2 pos)
{
    BlockBasic blockBasic = {
        .frameTimeCounter = 0,
        .pos = pos,
        .vel = { 0, 0 },
        .width = 16 * FACTOR,
        .height = 16 * FACTOR,
        .frameRect = { 0, 0, 16, 16 }
        // texture = LoadTexture("resources/block-basic.png"),
    };

    return blockBasic;
}

void updateBlockBasic(BlockBasic *blockBasic)
{
    return;
}

void drawBlockBasic(BlockBasic *blockBasic)
{
    // TEMPORARY: Replace with texture
    DrawRectangleLines(
        blockBasic->pos.x,
        blockBasic->pos.y,
        blockBasic->width,
        blockBasic->height,
        WHITE
    );
}