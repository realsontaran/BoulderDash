CC = gcc
GAME = game.c
HEADERS = prototypes.h LevelEditor.h

LIBS = ./Lib/libSDL2.so ./Lib/libSDL2_mixer.so ./Lib/libSDL2_image.so ./Lib/libSDL2_ttf.so ./Lib/libfreetype.so -L.

TEMP = game

target: all

all: $(GAME)
	$(CC) $(GAME) $(LIBS) -o $(TEMP)


