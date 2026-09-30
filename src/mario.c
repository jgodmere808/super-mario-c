
#include "mario.h"

Mario initMario(enum MarioSize size)
{
    Mario mario = {
        .size = size,
        .pos = { 0, 0 },
        .vel = { 0, 0 },
        .smallMarioTexture = LoadTexture("resources/mario-small.png"),
        .largeMarioTexture = LoadTexture("resources/mario-large.png")
    };

    switch (size) {
        case SMALL:
            mario.width = 10;
            mario.height = 20;
            break;
        case LARGE:
            mario.width = 20;
            mario.height = 40;
            break;
    }

    return mario;
}

void updateMario()
{
    return;
}

void drawMario()
{
    return;
}