#pragma once

#include "raylib.h"
#include "blocks/blocks.h"

#define MAX_TILE_ROWS   13
#define MAX_TILE_COLS 1024

enum MapSelection {
    MAP_1_1
};

enum TileType {
    BLOCK_EMPTY,
    BLOCK_DIRT,
    BLOCK_BRICK
};

void loadMap(enum MapSelection selection);