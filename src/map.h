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
    BLOCK_REWARD_POWERUP,
    BLOCK_REWARD_BRICK_SMASH
};

enum EnemyType {
    ENEMY_GOOMBA,
    ENEMY_KOOPA_GREEN
};

typedef struct {
    enum EnemyType type;
    Vector2 pos;
} EnemySpawn;

typedef struct {
    float poleX;
    float topY;
    float baseY;
    float castleDoorX;
} MapGoal;

bool loadMap(enum MapSelection selection);
const char *getMapMusicPath();
bool isMapSolidAt(int row, int col);
enum BlockReward hitMapBlock(int row, int col, bool smallMario);
int getMapWidthPixels();
const EnemySpawn *getMapEnemySpawns(int *count);
const MapGoal *getMapGoal(void);
void updateMap();
void drawMap(float cameraX);
