#pragma once

#include "raylib.h"
#include "../config.h"

typedef struct _block_basic {
    float frameTimeCounter;
    Vector2 pos;
    Vector2 vel;
    int width;
    int height;
    Rectangle frameRect;
    Texture2D texture;
} BlockBasic;

BlockBasic initBlockBasic();
void updateBlockBasic(BlockBasic *blockBasic);
void drawBlockBasic(BlockBasic *blockBasic);