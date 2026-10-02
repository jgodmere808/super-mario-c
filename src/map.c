
#include "map.h"

typedef struct _map {
    enum TileType tileMap[MAX_TILE_ROWS][MAX_TILE_COLS];
    int rows;
    int cols;
} Map;

static Map map;

bool loadMap(enum MapSelection selection)
{
    if (selection != MAP_1_1) return false;

    FILE *file = fopen("maps/1/1-1.map", "r");
    if (!file) return false;

    char line[MAX_TILE_COLS + 3];
    int row = 0;
    int width = 0;

    while (fgets(line, sizeof(line), file)) {
        size_t length = strcspn(line, "\r\n");

        if (
            row >= MAX_TILE_ROWS || length == 0 ||
            length > MAX_TILE_COLS || (width != 0 && length != (size_t)width)
        ) {
            goto invalid;
        }

        if (width == 0) width = (int)length;

        for (size_t col = 0; col < length; col++) {
            switch (line[col]) {
                case '.': map.tileMap[row][col] = BLOCK_EMPTY; break;
                case 'D': map.tileMap[row][col] = BLOCK_DIRT; break;
                case 'B': map.tileMap[row][col] = BLOCK_BRICK; break;
                default: goto invalid;
            }
        }
        row++;
    }

    if (ferror(file) || row != MAX_TILE_ROWS) goto invalid;

    fclose(file);
    map.rows = row;
    map.cols = width;
    return true;

invalid:
    fclose(file);
    return false;
}

bool isMapSolidAt(int row, int col)
{
    if (
        row < 0 || row >= map.rows ||
        col < 0 || col >= map.cols
    ) {
        return false;
    }

    if (
        map.tileMap[row][col] == BLOCK_DIRT ||
        map.tileMap[row][col] == BLOCK_BRICK
    ) {
        return true;
    }

    return false;
}

void drawMap(int cameraX)
{
    int row, col;
    int firstCol, lastCol;

    if (map.cols == 0) return;
    if (cameraX < 0) cameraX = 0;

    firstCol = cameraX / 16;
    lastCol = (cameraX + 255) / 16;

    if (lastCol >= map.cols) lastCol = map.cols - 1;

    for (row = 0; row < map.rows; row++) {
        for (col = firstCol; col <= lastCol; col++) {
            Texture2D texture;

            switch (map.tileMap[row][col]) {
                case BLOCK_DIRT:  texture = textureMap.blockDirt;  break;
                case BLOCK_BRICK: texture = textureMap.blockBrick; break;
                case BLOCK_EMPTY: continue;
            }

            DrawTexturePro(
                texture,
                (Rectangle){ 0, 0, 16, 16 },
                (Rectangle){
                    (col * 16 - cameraX) * FACTOR,
                    (row * 16 + 16) * FACTOR,
                    16 * FACTOR,
                    16 * FACTOR
                },
                (Vector2){ 0, 0 },
                0,
                WHITE
            );
        }
    }
}