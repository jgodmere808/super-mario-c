#pragma once

#include "config.h"

typedef struct _texture_map {
    Texture2D marioSmall;
    Texture2D marioLarge;
    Texture2D blockDirt;
    Texture2D blockBrick;
    Texture2D blockStone;
} TextureMap;

TextureMap textureMap;

void initTextureMap();
