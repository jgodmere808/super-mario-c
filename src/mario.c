
#include "mario.h"

Mario initMario(enum MarioSize size)
{
    Mario mario = {
        .size = size,
        .animation = IDLE,
        .frameTimeCounter = 0,
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

void updateSmallAnimation(Mario *mario)
{
    int frame;
    float frameTime;
    mario->frameTimeCounter += GetFrameTime();

    switch (mario->animation) {
        case IDLE:
            mario->smallFrameRect.x = 0;
            break;
        case RUNNING:
            frameTime = 0.5 / 3;
            if (mario->frameTimeCounter >= 0.5) {
                mario->frameTimeCounter = 0;
            }
            frame = mario->frameTimeCounter / frameTime;
            mario->smallFrameRect.x = 16 * (frame + 1);
            break;
    }
}

void updateLargeAnimation(Mario *mario)
{
    int i;

    switch (mario->animation) {
        case IDLE:
            mario->smallFrameRect.x = 0;
            break;
        case RUNNING:
            mario->smallFrameRect.x = 16 * 1;
            break;
    }
}

void updateMario(Mario *mario)
{
    if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
        if (mario->animation != RUNNING) mario->frameTimeCounter = 0;
        mario->animation = RUNNING;
    }
    if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        if (mario->animation != RUNNING) mario->frameTimeCounter = 0;
        mario->animation = RUNNING;
    }
    if (
        (IsKeyDown(KEY_LEFT) && IsKeyDown(KEY_RIGHT)) ||
        (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
    ) {
        if (mario->animation != IDLE) mario->frameTimeCounter = 0;
        mario->animation = IDLE;
    }

    switch (mario->size) {
        case SMALL:
            updateSmallAnimation(mario);
            break;
        case LARGE:
            updateLargeAnimation(mario);
            break;
    }
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