
#include "game.h"
#include "items.h"

typedef struct _game {
    Mario mario;
    float cameraX;
    int coins;
    Music music;
} Game;

static Game game;

bool initGame()
{
    // unload previous music if any
    if (IsMusicValid(game.music)) {
        UnloadMusicStream(game.music);
    }

    if (!loadMap(MAP_1_1)) {
        return false;
    }

    game = (Game){
        .mario = initMario((Vector2){ 32 * FACTOR, 176 * FACTOR }, SMALL)
    };
    resetItems();

    game.music = LoadMusicStream(getMapMusicPath());
    if (!IsMusicValid(game.music)) return false;

    game.music.looping = true;
    PlayMusicStream(game.music);

    return true;
}

void endGame()
{
    UnloadMusicStream(game.music);
}

void updateGame()
{
    UpdateMusicStream(game.music);

    BlockHit blockHit;
    updateMario(&game.mario, &blockHit);

    if (blockHit.happened) {
        enum BlockReward reward = hitMapBlock(
            blockHit.row, blockHit.col, game.mario.size == SMALL
        );
        Vector2 blockPos = {
            blockHit.col * 16 * FACTOR,
            (blockHit.row + 1) * 16 * FACTOR
        };

        if (reward == BLOCK_REWARD_COIN) {
            game.coins++;
            spawnItem(ITEM_BOX_COIN, blockPos);
        } else if (reward == BLOCK_REWARD_POWERUP) {
            spawnItem(ITEM_MUSHROOM, blockPos);
        }
    }

    updateMap();
    ItemPickups pickups = updateItems(
        (Rectangle){
            game.mario.pos.x, game.mario.pos.y,
            game.mario.width, game.mario.height
        },
        GetFrameTime()
    );
    if (pickups.mushroomsCollected > 0) growMario(&game.mario);

    float marioCenter = game.mario.pos.x + game.mario.width / 2.0f;
    float cameraTarget = marioCenter - SCREEN_WIDTH / 2.0f;

    // Scroll once Mario reaches the center; never scroll back to the left.
    if (cameraTarget > game.cameraX) {
        game.cameraX = cameraTarget;
    }

    float maxCameraX = getMapWidthPixels() - SCREEN_WIDTH;
    if (maxCameraX < 0.0f) maxCameraX = 0.0f;
    if (game.cameraX > maxCameraX) game.cameraX = maxCameraX;
}

void drawGame()
{
    drawMap(game.cameraX);
    drawItems(game.cameraX);
    drawMario(&game.mario, game.cameraX);
    DrawText(TextFormat("COINS %02d", game.coins), 12, 12, 20, WHITE);
}
