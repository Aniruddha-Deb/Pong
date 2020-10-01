CC = g++

CPPFLAGS = -std=c++14 -Wall 
LDFLAGS = -lSDL2 -lSDL2_image -lm

OBJDIR = obj

DEPS = panel.hpp physics.hpp settings.hpp

_OBJ = panel.o physics.o pong.o settings.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

$(OBJDIR)/%.o: %.cpp $(DEPS)
	$(CC) $(CPPFLAGS) -c $< -o $@

all: $(OBJ) 
	$(CC) $(OBJ) -o exec/pong $(LDFLAGS)

clean:
	rm $(OBJ)
	rm exec/pong
