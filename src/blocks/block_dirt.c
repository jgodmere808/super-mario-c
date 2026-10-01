
#include "block_dirt.h"

BlockDirt initBlockDirt(Vector2 pos)
{
    BlockDirt blockDirt = {
        .frameTimeCounter = 0,
        .pos = pos,
        .vel = { 0, 0 },
        .width = 16 * FACTOR,
        .height = 16 * FACTOR,
        .frameRect = { 0, 0, 16, 16 },
        .texture = LoadTexture("resources/block-dirt.png")
    };

    return blockDirt;
}

void updateBlockDirt(BlockDirt *blockDirt)
{
    return;
}

void drawBlockDirt(BlockDirt *blockDirt)
{
    DrawTexturePro(
        blockDirt->texture,
        blockDirt->frameRect,
        (Rectangle){ blockDirt->pos.x, blockDirt->pos.y, blockDirt->width, blockDirt->height },
        (Vector2){ 0, 0 },
        0.0f,
        WHITE
    );
}
