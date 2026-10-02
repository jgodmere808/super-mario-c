
#include "texture_map.h"

void initTextureMap()
{
    textureMap = (TextureMap){
        .marioSmall = LoadTexture("resources/mario-small.png"),
        .marioLarge = LoadTexture("resources/mario-large.png"),
        .blockDirt = LoadTexture("resources/block-dirt.png"),
        .blockBrick = LoadTexture("resources/block-brick.png")
    };
}