
#include "game.h"

typedef struct _game {
    Mario mario;
    float cameraX;
} Game;

static Game game;

bool initGame()
{
    if (!loadMap(MAP_1_1)) {
        return false;
    }

    game = (Game){
        .mario = initMario((Vector2){ 32 * FACTOR, 176 * FACTOR }, SMALL)
    };

    return true;
}

void updateGame()
{
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
