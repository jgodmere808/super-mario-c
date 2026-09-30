#pragma once

#include "raylib.h"

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
    Texture2D smallMarioTexture;
    Texture2D largeMarioTexture;
} Mario;

Mario initMario();
void updateMario(Mario *mario);
void drawMario(Mario *mario);