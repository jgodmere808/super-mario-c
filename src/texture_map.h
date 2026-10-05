#pragma once

#include "config.h"

typedef struct _texture_map {
    Texture2D marioSmall;
    Texture2D marioLarge;
    Texture2D blockDirt;
    Texture2D blockBrick;
    Texture2D blockStone;
    Texture2D mysteryBox;
    Texture2D coin;
    Texture2D mushroom;
    Texture2D goomba;
    Texture2D goombaSquashed;
    Texture2D koopaGreen;
    Texture2D koopaShellGreen;
    Texture2D cloudSmall;
    Texture2D cloudMedium;
    Texture2D cloudLarge;
    Texture2D bushSmall;
    Texture2D bushMedium;
    Texture2D bushLarge;
    Texture2D hillSmall;
    Texture2D hillLarge;
    Texture2D castle;
} TextureMap;

extern TextureMap textureMap;

void initTextureMap();
