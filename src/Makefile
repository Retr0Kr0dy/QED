CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lGL -lSDL2_ttf -lm

SRC = BQEDPS.c
OBJ = $(SRC:.c=.o)
EXEC = BQEDPS

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: clean all
