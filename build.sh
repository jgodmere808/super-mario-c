gcc src/main.c src/mario.c -o main \
-I$(brew --prefix raylib)/include -L$(brew --prefix raylib)/lib -lraylib \
-framework OpenGL \
-framework IOKit \
-framework Cocoa \
-framework CoreVideo