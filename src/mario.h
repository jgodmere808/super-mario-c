#pragma once

#include "raylib.h"
#include "config.h"

enum MarioSize {
    SMALL,
    LARGE
};

typedef struct _mario {
    enum MarioSize size;
    Vector2 pos;
    Vector2 vel;
    int width;
    int height;
    Rectangle smallFrameRect;
    Rectangle largeFrameRect;
    Texture2D smallMarioTexture;
    Texture2D largeMarioTexture;
} Mario;

Mario initMario(enum MarioSize size);
void updateMario(Mario *mario);
void drawMario(Mario *mario);