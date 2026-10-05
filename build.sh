gcc src/main.c src/mario.c src/game.c src/texture_map.c src/map.c src/items.c \
    src/enemies.c src/goomba.c src/koopa.c -o main \
-I$(brew --prefix raylib)/include -L$(brew --prefix raylib)/lib -lraylib \
-framework OpenGL \
-framework IOKit \
-framework Cocoa \
-framework CoreVideo
