#include "items.h"

#include "map.h"
#include "texture_map.h"

#define MAX_ITEMS 32
#define TILE_SIZE (16 * FACTOR)
#define COIN_DURATION 0.5f
#define COIN_RISE_SPEED (96.0f * FACTOR)
#define EMERGE_DURATION 0.4f
#define MUSHROOM_SPEED 90.0f

typedef enum {
    ITEM_EMERGING,
    ITEM_MOVING
} ItemState;

typedef struct {
    bool active;
    ItemType type;
    ItemState state;
    Vector2 pos;
    Vector2 vel;
    float blockY;
    float timer;
} Item;

static Item items[MAX_ITEMS];

static Rectangle itemBounds(const Item *item)
{
    return (Rectangle){ item->pos.x, item->pos.y, TILE_SIZE, TILE_SIZE };
}

static int mapRowAt(float y)
{
    // Map row 0 begins at screen y = TILE_SIZE.
    return (int)floorf(y / TILE_SIZE) - 1;
}

static int mapColAt(float x)
{
    return (int)floorf(x / TILE_SIZE);
}

void resetItems(void)
{
    memset(items, 0, sizeof(items));
}

bool spawnItem(ItemType type, Vector2 blockPos)
{
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (items[i].active) continue;

        items[i] = (Item){
            .active = true,
            .type = type,
            .state = ITEM_EMERGING,
            .pos = blockPos,
            .blockY = blockPos.y
        };
        return true;
    }

    return false;
}

static void updateCoin(Item *item, float dt)
{
    item->timer += dt;
    item->pos.y = item->blockY - item->timer * COIN_RISE_SPEED;

    if (item->timer >= COIN_DURATION) item->active = false;
}

static void updateMushroom(Item *item, float dt)
{
    if (item->state == ITEM_EMERGING) {
        item->timer += dt;
        float progress = item->timer / EMERGE_DURATION;
        if (progress >= 1.0f) {
            progress = 1.0f;
            item->state = ITEM_MOVING;
            item->vel.x = MUSHROOM_SPEED;
        }
        item->pos.y = item->blockY - progress * TILE_SIZE;
        return;
    }

    item->pos.x += item->vel.x * dt;
    int leadingCol = mapColAt(item->vel.x > 0.0f
        ? item->pos.x + TILE_SIZE - 0.001f : item->pos.x);
    int topRow = mapRowAt(item->pos.y);
    int bottomRow = mapRowAt(item->pos.y + TILE_SIZE - 0.001f);

    if (isMapSolidAt(topRow, leadingCol) ||
        isMapSolidAt(bottomRow, leadingCol)) {
        item->pos.x = item->vel.x > 0.0f
            ? leadingCol * TILE_SIZE - TILE_SIZE
            : (leadingCol + 1) * TILE_SIZE;
        item->vel.x = -item->vel.x;
    }

    item->vel.y += GRAVITY * dt;
    if (item->vel.y > 900.0f) item->vel.y = 900.0f;
    item->pos.y += item->vel.y * dt;

    if (item->vel.y > 0.0f) {
        int floorRow = mapRowAt(item->pos.y + TILE_SIZE - 0.001f);
        int leftCol = mapColAt(item->pos.x);
        int rightCol = mapColAt(item->pos.x + TILE_SIZE - 0.001f);

        if (isMapSolidAt(floorRow, leftCol) ||
            isMapSolidAt(floorRow, rightCol)) {
            item->pos.y = floorRow * TILE_SIZE;
            item->vel.y = 0.0f;
        }
    }

    if (item->pos.y > SCREEN_HEIGHT + TILE_SIZE) item->active = false;
}

ItemPickups updateItems(Rectangle marioBounds, float dt)
{
    ItemPickups pickups = { 0 };
    if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;

    for (int i = 0; i < MAX_ITEMS; i++) {
        Item *item = &items[i];
        if (!item->active) continue;

        if (item->type == ITEM_BOX_COIN) {
            updateCoin(item, dt);
        } else {
            updateMushroom(item, dt);
            if (item->active && item->state == ITEM_MOVING &&
                CheckCollisionRecs(itemBounds(item), marioBounds)) {
                pickups.mushroomsCollected++;
                item->active = false;
            }
        }
    }

    return pickups;
}

void drawItems(float cameraX)
{
    for (int i = 0; i < MAX_ITEMS; i++) {
        const Item *item = &items[i];
        if (!item->active) continue;

        Texture2D texture;
        Rectangle source = { 0, 0, 16, 16 };
        Rectangle dest = {
            item->pos.x - cameraX, item->pos.y, TILE_SIZE, TILE_SIZE
        };

        if (item->type == ITEM_BOX_COIN) {
            texture = textureMap.coin;
            source.x = ((int)(item->timer / (COIN_DURATION / 6.0f)) % 6) * 16;
        } else {
            texture = textureMap.mushroom;
        }

        // Only show the part that has risen above the mystery block.
        if (item->state == ITEM_EMERGING &&
            item->pos.y + TILE_SIZE > item->blockY) {
            float visibleHeight = item->blockY - item->pos.y;
            if (visibleHeight <= 0.0f) continue;
            source.height = visibleHeight / FACTOR;
            dest.height = visibleHeight;
        }

        DrawTexturePro(texture, source, dest, (Vector2){ 0, 0 }, 0, WHITE);
    }
}
