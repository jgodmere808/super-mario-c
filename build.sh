gcc src/main.c src/mario.c src/blocks/block_brick.c \
src/blocks/block_dirt.c src/game.c src/texture_map.c -o main \
-I$(brew --prefix raylib)/include -L$(brew --prefix raylib)/lib -lraylib \
-framework OpenGL \
-framework IOKit \
-framework Cocoa \
-framework CoreVideo
