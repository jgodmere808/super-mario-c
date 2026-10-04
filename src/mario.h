#pragma once

#include <math.h>

#include "config.h"
#include "texture_map.h"
#include "map.h"

enum MarioSize {
    SMALL,
    LARGE
};

enum MarioAnimation {
    IDLE,
    RUNNING,
    JUMPING,
    SKIDDING,
    DIEING,
    SWIMMING,
    CLIMBING,
    FLAGPOLE
};

typedef struct _mario {
    enum MarioSize size;
    enum MarioAnimation animation;
    float frameTimeCounter;
    bool facingLeft;
    bool onGround;
    Vector2 pos;
    Vector2 vel;
    int width;
    int height;
    Rectangle smallFrameRect;
    Rectangle largeFrameRect;
    Texture2D smallMarioTexture;
    Texture2D largeMarioTexture;
} Mario;

typedef struct {
    bool happened;
    int row;
    int col;
} BlockHit;

Mario initMario(Vector2 pos, enum MarioSize size);
void updateMario(Mario *mario, BlockHit *blockHit);
void growMario(Mario *mario);
void drawMario(Mario *mario, float cameraX);
