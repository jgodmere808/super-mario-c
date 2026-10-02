#pragma once

#include "blocks/blocks.h"

#define MAX_TILE_ROWS   13
#define MAX_TILE_COLS 1024

typedef struct _tile_map {
    enum BlockType blocks[MAX_TILE_ROWS][MAX_TILE_COLS];
} TileMap;

typedef struct _map {
    TileMap tileMap;
} Map;