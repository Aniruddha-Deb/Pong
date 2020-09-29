CC := gcc

CFLAGS := -std=c99 -Wall 
LDFLAGS := -lSDL2 -lSDL2_image -lm

all:
	$(CC) $(CFLAGS) *.c *.h -o bin/pong $(LDFLAGS)
