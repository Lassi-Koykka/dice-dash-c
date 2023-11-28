NAME = dice-dash
CC = gcc
CFLAGS = -g -std=c99 -Wall -pedantic
LFLAGS = -lraylib -lm
FILES = main.c grid.c

all: game

game: $(FILES)
	$(CC) $(CFLAGS) -o $(NAME) $(FILES) $(LFLAGS)

run:
	./dice-dash

clean:
	rm ./dice-dash
