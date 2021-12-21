
#COMPILER_FLAGS specifies the additional compilation options we're using 
COMPILER_FLAGS = -c -Wall -O2

#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -lstdc++ -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm

SDL_WIN= ${HOME}/sdl_win/SDL2-2.0.18/x86_64-w64-mingw32
SDL_WIN_IMAGE= ${HOME}/sdl_win/SDL2_image-2.0.5/x86_64-w64-mingw32
SDL_WIN_TTF= ${HOME}/sdl_win/SDL2_ttf-2.0.15/x86_64-w64-mingw32
SDL_WIN_MIXER= ${HOME}/sdl_win/SDL2_mixer-2.0.4/x86_64-w64-mingw32

# Include Windows
INCL_WIN=-I$(SDL_WIN)/include -I$(SDL_WIN)/include/SDL2 -I${SDL_WIN_IMAGE}/include -I${SDL_WIN_TTF}/include -I${SDL_WIN_MIXER}/include

# Lib Windows
LIB_WIN=-L$(SDL_WIN)/lib -L$(SDL_WIN_IMAGE)/lib -L$(SDL_WIN_TTF)/lib -L$(SDL_WIN_MIXER)/lib

all: game game.dat

microtar.o: microtar.h microtar.c
	gcc $(COMPILER_FLAGS) microtar.c -o microtar.o

jengine.o: jengine.cpp jengine.hpp
	gcc $(COMPILER_FLAGS) jengine.cpp -o jengine.o
	
game.o: game.cpp game.hpp
	gcc $(COMPILER_FLAGS) game.cpp -o game.o


main.o: main.cpp
	gcc $(COMPILER_FLAGS) main.cpp -o main.o

game: main.o jengine.o game.o microtar.o
	gcc main.o jengine.o microtar.o game.o $(LINKER_FLAGS) -o game
	
debug: jengine.cpp jengine.hpp juego.cpp game.hpp microtar.c main.cpp
	gcc -g $(LINKER_FLAGS) jengine.cpp game.cpp main.cpp -o debug

clean:
	rm -f *.o *.exe *.zip *.dat game debug

jengine.win.o: jengine.cpp jengine.hpp
	x86_64-w64-mingw32-gcc $(INCL_WIN) $(LIB_WIN) $(COMPILER_FLAGS) jengine.cpp -o jengine.win.o

microtar.win.o: microtar.h microtar.c
	x86_64-w64-mingw32-gcc $(COMPILER_FLAGS) microtar.c -o microtar.win.o
	
game.win.o: game.cpp game.hpp
	x86_64-w64-mingw32-gcc $(INCL_WIN) $(LIB_WIN) $(COMPILER_FLAGS) game.cpp -o game.win.o

main.win.o: main.cpp
	x86_64-w64-mingw32-g++ $(INCL_WIN) $(COMPILER_FLAGS) winmain.cpp -o main.win.o

game.exe: main.win.o jengine.win.o game.win.o microtar.win.o
	x86_64-w64-mingw32-g++ $(LIB_WIN) -static main.win.o jengine.win.o microtar.win.o game.win.o `$(SDL_WIN)/bin/sdl2-config --static-libs` -lSDL2_image -lSDL2_ttf.dll -lSDL2_ttf -lSDL2_mixer.dll -lSDL2_mixer -o game.exe

game.dat: background.png sprites.png
	tar cvf game.dat background.png sprites.png

game.zip: game.exe game.dat
	zip -j game.zip game.exe game.dat $(SDL_WIN)/bin/SDL2.dll $(SDL_WIN_IMAGE)/bin/libpng16-16.dll $(SDL_WIN_IMAGE)/bin/zlib1.dll $(SDL_WIN_TTF)/bin/SDL2_ttf.dll $(SDL_WIN_TTF)/bin/libfreetype-6.dll $(SDL_WIN_MIXER)/bin/SDL2_mixer.dll background.png
