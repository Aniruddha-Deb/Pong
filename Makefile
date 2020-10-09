CC = gcc

CFLAGS = -std=c99 -Wall 
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lm

OBJDIR = obj

DEPS = pong.h vector.h

_OBJ = pong.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(OBJ) 
	$(CC) $(OBJ) -o exec/pong $(LDFLAGS)

clean:
	rm $(OBJ)/*
	rm exec/*
