
#include "game.h"

typedef struct _game {
    Mario mario;
    float cameraX;
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

    updateMario(&game.mario);

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
    drawMario(&game.mario, game.cameraX);
}
