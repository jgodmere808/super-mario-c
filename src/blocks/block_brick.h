#pragma once

#include "raylib.h"
#include "../config.h"

typedef struct _block_brick {
    float frameTimeCounter;
    Vector2 pos;
    Vector2 vel;
    int width;
    int height;
    Rectangle frameRect;
    Texture2D texture;
} BlockBrick;

BlockBrick initBlockBrick(Vector2 pos);
void updateBlockBrick(BlockBrick *blockBrick);
void drawBlockBrick(BlockBrick *blockBrick);
