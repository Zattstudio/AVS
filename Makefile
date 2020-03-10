# DEPS: SDL2, SDL2-mixer,


# Specify compiler
CC=g++

# Specify linker
LINK=g++

OBJS=src/main.cc src/AudioHandler.cc

LINKER_FLAGS = -lSDL2_mixer -lSDL2 -lmicrohttpd

OPT_NAME=AudioVote

all: $(OBJS)
	$(CC) $(OBJS) -std=c++17 -w $(LINKER_FLAGS) -o $(OPT_NAME)


clean :
	rm AudioVote