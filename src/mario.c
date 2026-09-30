
#include "mario.h"

Mario initMario(enum MarioSize size)
{
    Mario mario = {
        .size = size,
        .pos = { 0, 0 },
        .vel = { 0, 0 },
        .smallFrameRect = { 0, 0, 16, 16 },
        .largeFrameRect = { 0, 0, 16, 32 },
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

void updateMario(Mario *mario)
{
    return;
}

void drawMario(Mario *mario)
{
    if (mario->size == SMALL) {
        DrawTexturePro(
            mario->smallMarioTexture,
            mario->smallFrameRect,
            (Rectangle){ mario->pos.x, mario->pos.y, 16 * FACTOR, 16 * FACTOR },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
    } else if (mario->size == LARGE) {
        DrawTexturePro(
            mario->largeMarioTexture,
            mario->largeFrameRect,
            (Rectangle){ mario->pos.x, mario->pos.y, 16 * FACTOR, 32 * FACTOR },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
    }
}