
#include "block_brick.h"

BlockBrick initBlockBrick(Vector2 pos)
{
    BlockBrick blockBrick = {
        .frameTimeCounter = 0,
        .pos = pos,
        .vel = { 0, 0 },
        .width = 16 * FACTOR,
        .height = 16 * FACTOR,
        .frameRect = { 0, 0, 16, 16 },
        .texture = LoadTexture("resources/block-brick.png")
    };

    return blockBrick;
}

void updateBlockBrick(BlockBrick *blockBrick)
{
    return;
}

void drawBlockBrick(BlockBrick *blockBrick)
{
    DrawTexturePro(
        blockBrick->texture,
        blockBrick->frameRect,
        (Rectangle){ blockBrick->pos.x, blockBrick->pos.y, blockBrick->width, blockBrick->height },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}
