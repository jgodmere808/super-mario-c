#pragma once

#include "config.h"
#include "texture_map.h"

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

bool loadMap(enum MapSelection selection);
bool isMapSolidAt(int row, int col);
int getMapWidthPixels();
void drawMap(float cameraX);
