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
    BLOCK_BRICK,
    BLOCK_STONE,
    BLOCK_MYSTERY_COIN,
    BLOCK_MYSTERY_POWERUP,
    BLOCK_USED
};

enum BlockReward {
    BLOCK_REWARD_NONE,
    BLOCK_REWARD_COIN,
    BLOCK_REWARD_POWERUP
};

bool loadMap(enum MapSelection selection);
const char *getMapMusicPath();
bool isMapSolidAt(int row, int col);
enum BlockReward hitMapBlock(int row, int col, bool smallMario);
int getMapWidthPixels();
void updateMap();
void drawMap(float cameraX);
