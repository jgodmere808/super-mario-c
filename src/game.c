
#include "game.h"

typedef struct _game {
    Map map;
    Mario mario;
} Game;

static Game game;

void initGame()
{
    Game game = {
        .mario = initMario(SMALL)
    };
}

void updateGame()
{
    return;
}

void drawGame()
{
    drawMario(&game.mario);
}