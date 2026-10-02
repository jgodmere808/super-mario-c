#pragma once

#include "raylib.h"
#include "../texture_map.h"
#include "../config.h"

typedef struct _block_dirt {
    float frameTimeCounter;
    Vector2 pos;
    Vector2 vel;
    int width;
    int height;
    Rectangle frameRect;
    Texture2D texture;
} BlockDirt;

BlockDirt initBlockDirt(Vector2 pos);
void updateBlockDirt(BlockDirt *blockDirt);
void drawBlockDirt(BlockDirt *blockDirt);
