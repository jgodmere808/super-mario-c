
#include "texture_map.h"

void initTextureMap()
{
    textureMap = (TextureMap){
        .marioSmall = LoadTexture("resources/mario-small.png"),
        .marioLarge = LoadTexture("resources/mario-large.png"),
        .blockDirt = LoadTexture("resources/block-dirt.png"),
        .blockBrick = LoadTexture("resources/block-brick.png"),
        .blockStone = LoadTexture("resources/block-stone.png"),
        .cloudSmall = LoadTexture("resources/scenery/cloud-small.png"),
        .cloudMedium = LoadTexture("resources/scenery/cloud-medium.png"),
        .cloudLarge = LoadTexture("resources/scenery/cloud-large.png"),
        .bushSmall = LoadTexture("resources/scenery/bush-small.png"),
        .bushMedium = LoadTexture("resources/scenery/bush-medium.png"),
        .bushLarge = LoadTexture("resources/scenery/bush-large.png"),
        .hillSmall = LoadTexture("resources/scenery/hill-small.png"),
        .hillLarge = LoadTexture("resources/scenery/hill-large.png"),
        .castle = LoadTexture("resources/scenery/castle.png")
    };
}
