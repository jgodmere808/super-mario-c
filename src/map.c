
#include "map.h"

#define MAX_SCENERY_OBJECTS 64
#define MAX_BRICK_FRAGMENTS 64

#define BUMP_DURATION 0.2f
#define BUMP_HEIGHT (6.0f * FACTOR)
#define FRAGMENT_LIFETIME 0.8f

enum SceneryType {
    CLOUD_SMALL, CLOUD_MEDIUM, CLOUD_LARGE,
    BUSH_SMALL, BUSH_MEDIUM, BUSH_LARGE,
    HILL_SMALL, HILL_LARGE, CASTLE
};

typedef struct _scenery_object {
    enum SceneryType type;
    int x;
    int y;
} SceneryObject;

typedef struct _brick_fragment {
    Vector2 pos;
    Vector2 vel;
    Rectangle source;
    float remaining;
} BrickFragment;

typedef struct _map {
    enum TileType tileMap[MAX_TILE_ROWS][MAX_TILE_COLS];
    float bumpRemaining[MAX_TILE_ROWS][MAX_TILE_COLS];
    SceneryObject scenery[MAX_SCENERY_OBJECTS];
    int sceneryCount;
    BrickFragment fragments[MAX_BRICK_FRAGMENTS];
    float mysteryBoxAnimationTimer;
    int rows;
    int cols;
    const char *musicPath;
} Map;

static Map map;

static bool parseSceneryType(const char *name, enum SceneryType *type)
{
    static const char *names[] = {
        "cloud-small", "cloud-medium", "cloud-large",
        "bush-small", "bush-medium", "bush-large",
        "hill-small", "hill-large", "castle"
    };

    for (int i = 0; i < (int)(sizeof(names) / sizeof(names[0])); i++) {
        if (strcmp(name, names[i]) == 0) {
            *type = (enum SceneryType)i;
            return true;
        }
    }
    return false;
}

static Texture2D sceneryTexture(enum SceneryType type)
{
    switch (type) {
        case CLOUD_SMALL:  return textureMap.cloudSmall;
        case CLOUD_MEDIUM: return textureMap.cloudMedium;
        case CLOUD_LARGE:  return textureMap.cloudLarge;
        case BUSH_SMALL:   return textureMap.bushSmall;
        case BUSH_MEDIUM:  return textureMap.bushMedium;
        case BUSH_LARGE:   return textureMap.bushLarge;
        case HILL_SMALL:   return textureMap.hillSmall;
        case HILL_LARGE:   return textureMap.hillLarge;
        case CASTLE:       return textureMap.castle;
    }
    return (Texture2D){ 0 };
}

static float bumpOffsetPixels(int row, int col)
{
    float remaining = map.bumpRemaining[row][col];
    if (remaining <= 0.0f) return 0.0f;

    float progress = 1.0f - remaining / BUMP_DURATION;
    float height = progress < 0.5
        ? progress * 2.0f
        : (1.0f - progress) * 2.0f;

    return BUMP_HEIGHT * height;
}

bool loadMap(enum MapSelection selection)
{
    if (selection != MAP_1_1) return false;

    FILE *file = fopen("maps/1/1-1.map", "r");
    if (!file) return false;

    char line[MAX_TILE_COLS + 3];
    int row = 0;
    int width = 0;
    bool inScenery = false;

    map.rows = 0;
    map.cols = 0;
    map.sceneryCount = 0;
    memset(map.fragments, 0, sizeof(map.fragments));

    while (row < MAX_TILE_ROWS && fgets(line, sizeof(line), file)) {
        size_t length = strcspn(line, "\r\n");

        if (
            length == 0 || length > MAX_TILE_COLS ||
            (width != 0 && length != (size_t)width)
        ) {
            goto invalid;
        }

        if (width == 0) width = (int)length;

        for (size_t col = 0; col < length; col++) {
            switch (line[col]) {
                case '.': map.tileMap[row][col] = BLOCK_EMPTY; break;
                case 'D': map.tileMap[row][col] = BLOCK_DIRT; break;
                case 'B': map.tileMap[row][col] = BLOCK_BRICK; break;
                case 'S': map.tileMap[row][col] = BLOCK_STONE; break;
                case 'P': map.tileMap[row][col] = BLOCK_MYSTERY_POWERUP; break;
                case 'M': map.tileMap[row][col] = BLOCK_MYSTERY_COIN; break;
                default: goto invalid;
            }
        }
        row++;
    }

    if (ferror(file) || row != MAX_TILE_ROWS) goto invalid;

    while (fgets(line, sizeof(line), file)) {
        size_t length = strcspn(line, "\r\n");
        line[length] = '\0';
        if (length == 0 || line[0] == '#') continue;

        if (!inScenery) {
            if (strcmp(line, "[scenery]") != 0) goto invalid;
            inScenery = true;
            continue;
        }

        char name[32];
        char extra;
        float tileX, tileY;
        enum SceneryType type;
        if (
            map.sceneryCount >= MAX_SCENERY_OBJECTS ||
            sscanf(line, "%31s %f %f %c", name, &tileX, &tileY, &extra) != 3 ||
            !parseSceneryType(name, &type) || !isfinite(tileX) || !isfinite(tileY)
        ) goto invalid;

        Texture2D texture = sceneryTexture(type);
        float pixelX = tileX * 16.0f;
        float pixelY = tileY * 16.0f;
        if (
            texture.width <= 0 || texture.height <= 0 ||
            pixelX < 0 || pixelY < 0 ||
            pixelX + texture.width > width * 16 ||
            pixelY + texture.height > SCREEN_HEIGHT / FACTOR
        ) goto invalid;

        map.scenery[map.sceneryCount++] = (SceneryObject){
            type, (int)lroundf(pixelX), (int)lroundf(pixelY)
        };
    }

    if (ferror(file)) goto invalid;

    fclose(file);
    map.rows = row;
    map.cols = width;
    map.mysteryBoxAnimationTimer = 0;
    map.musicPath = "resources/audio/1-1-overworld.mp3";
    
    // reset bump timers
    memset(map.bumpRemaining, 0, sizeof(map.bumpRemaining));

    return true;

invalid:
    fclose(file);
    return false;
}

const char *getMapMusicPath()
{
    return map.musicPath;
}

bool isMapSolidAt(int row, int col)
{
    if (row < 0 || row >= map.rows) return false; // Space above / below map
    if (col < 0 || col >= map.cols) return true;  // Level edges

    if (
        map.tileMap[row][col] == BLOCK_DIRT ||
        map.tileMap[row][col] == BLOCK_BRICK ||
        map.tileMap[row][col] == BLOCK_STONE ||
        map.tileMap[row][col] == BLOCK_MYSTERY_COIN ||
        map.tileMap[row][col] == BLOCK_MYSTERY_POWERUP ||
        map.tileMap[row][col] == BLOCK_USED
    ) {
        return true;
    }

    return false;
}

static void scatterBrick(int row, int col)
{
    int piece = 0;
    float x = col * 16 * FACTOR;
    float y = (row + 1) * 16 * FACTOR;

    for (int i = 0; i < MAX_BRICK_FRAGMENTS && piece < 4; i++) {
        BrickFragment *fragment = &map.fragments[i];
        if (fragment->remaining > 0.0f) continue;

        int side = piece % 2;
        int half = piece / 2;
        *fragment = (BrickFragment){
            .pos = { x + side * 8 * FACTOR, y + half * 8 * FACTOR },
            .vel = {
                side == 0 ? -120.0f : 120.0f,
                half == 0 ? -360.0f : -240.0f
            },
            .source = { side * 8, half * 8, 8, 8 },
            .remaining = FRAGMENT_LIFETIME
        };
        piece++;
    }
}

enum BlockReward hitMapBlock(int row, int col, bool smallMario)
{
    if (row < 0 || row >= map.rows || col < 0 || col >= map.cols) {
        return BLOCK_REWARD_NONE;
    }

    enum TileType tile = map.tileMap[row][col];
    if (tile == BLOCK_MYSTERY_COIN || tile == BLOCK_MYSTERY_POWERUP) {
        map.bumpRemaining[row][col] = BUMP_DURATION;
        map.tileMap[row][col] = BLOCK_USED;
        return tile == BLOCK_MYSTERY_COIN
            ? BLOCK_REWARD_COIN : BLOCK_REWARD_POWERUP;
    }

    if (tile == BLOCK_BRICK) {
        if (smallMario) {
            if (map.bumpRemaining[row][col] <= 0.0f) {
                map.bumpRemaining[row][col] = BUMP_DURATION;
            }
        } else {
            map.tileMap[row][col] = BLOCK_EMPTY;
            map.bumpRemaining[row][col] = 0.0f;
            scatterBrick(row, col);
        }
    }

    return BLOCK_REWARD_NONE;
}

int getMapWidthPixels()
{
    return map.cols * 16 * FACTOR;
}

void updateMap()
{
    int row, col;
    float dt = GetFrameTime();
    if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;
    float *remaining;

    map.mysteryBoxAnimationTimer =
        fmodf(map.mysteryBoxAnimationTimer + GetFrameTime(), 6 * 0.12f);

    for (row = 0; row < map.rows; row++) {
        for (col = 0; col < map.cols; col++) {
            remaining = &map.bumpRemaining[row][col];
            if (*remaining > 0.0f) {
                *remaining -= dt;
                if (*remaining < 0.0f) *remaining = 0.0f;
            }
        }
    }

    for (int i = 0; i < MAX_BRICK_FRAGMENTS; i++) {
        BrickFragment *fragment = &map.fragments[i];
        if (fragment->remaining <= 0.0f) continue;

        fragment->remaining -= dt;
        fragment->pos.x += fragment->vel.x * dt;
        fragment->pos.y += fragment->vel.y * dt;
        fragment->vel.y += GRAVITY * dt;
    }
}

void drawMap(float cameraX)
{
    int row, col;
    int firstCol, lastCol;
    int frame;
    Rectangle source;

    if (map.cols == 0) return;
    if (cameraX < 0) cameraX = 0;

    for (int i = 0; i < map.sceneryCount; i++) {
        SceneryObject object = map.scenery[i];
        Texture2D texture = sceneryTexture(object.type);
        DrawTexturePro(
            texture,
            (Rectangle){ 0, 0, texture.width, texture.height },
            (Rectangle){
                object.x * FACTOR - cameraX, object.y * FACTOR,
                texture.width * FACTOR, texture.height * FACTOR
            },
            (Vector2){ 0, 0 }, 0, WHITE
        );
    }

    firstCol = (int)(cameraX / (16 * FACTOR));
    lastCol = (int)((cameraX + SCREEN_WIDTH - 1) / (16 * FACTOR));

    if (lastCol >= map.cols) lastCol = map.cols - 1;

    for (row = 0; row < map.rows; row++) {
        for (col = firstCol; col <= lastCol; col++) {
            Texture2D texture;

            switch (map.tileMap[row][col]) {
                case BLOCK_DIRT:  texture = textureMap.blockDirt;  break;
                case BLOCK_BRICK: texture = textureMap.blockBrick; break;
                case BLOCK_STONE: texture = textureMap.blockStone; break;
                case BLOCK_MYSTERY_COIN: texture = textureMap.mysteryBox; break;
                case BLOCK_MYSTERY_POWERUP: texture = textureMap.mysteryBox; break;
                case BLOCK_USED: texture = textureMap.mysteryBox; break;
                case BLOCK_EMPTY: continue;
            }

            if (
                map.tileMap[row][col] == BLOCK_MYSTERY_POWERUP ||
                map.tileMap[row][col] == BLOCK_MYSTERY_COIN
            ) {
                frame = (int)(map.mysteryBoxAnimationTimer / 0.12f);
                source = (Rectangle){ frame * 16, 0, 16, 16 };
            } else if (map.tileMap[row][col] == BLOCK_USED) {
                source = (Rectangle){ 6 * 16, 0, 16, 16 };
            } else {
                source = (Rectangle){ 0, 0, 16, 16 };
            }

            DrawTexturePro(
                texture,
                source,
                (Rectangle){
                    col * 16 * FACTOR - cameraX,
                    (row * 16 + 16) * FACTOR - bumpOffsetPixels(row, col),
                    16 * FACTOR,
                    16 * FACTOR
                },
                (Vector2){ 0, 0 },
                0,
                WHITE
            );
        }
    }

    for (int i = 0; i < MAX_BRICK_FRAGMENTS; i++) {
        BrickFragment fragment = map.fragments[i];
        if (fragment.remaining <= 0.0f) continue;

        DrawTexturePro(
            textureMap.blockBrick,
            fragment.source,
            (Rectangle){
                fragment.pos.x - cameraX, fragment.pos.y,
                8 * FACTOR, 8 * FACTOR
            },
            (Vector2){ 0, 0 }, 0, WHITE
        );
    }
}
