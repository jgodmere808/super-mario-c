
#include "game.h"

typedef struct _game {
    Mario mario;
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
}

void drawGame()
{
    drawMario(&game.mario);
    drawMap(0);
}