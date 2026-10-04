#pragma once

#include "config.h"

typedef enum {
    ITEM_BOX_COIN,
    ITEM_MUSHROOM
} ItemType;

typedef struct {
    int mushroomsCollected;
} ItemPickups;

void resetItems(void);
bool spawnItem(ItemType type, Vector2 blockPos);
ItemPickups updateItems(Rectangle marioBounds, float dt);
void drawItems(float cameraX);
