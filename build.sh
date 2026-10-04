gcc src/main.c src/mario.c src/game.c src/texture_map.c src/map.c src/items.c -o main \
-I$(brew --prefix raylib)/include -L$(brew --prefix raylib)/lib -lraylib \
-framework OpenGL \
-framework IOKit \
-framework Cocoa \
-framework CoreVideo
