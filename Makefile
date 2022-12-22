CFLAGS = -Wall -g -I./include

SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

all: main

main: $(OBJS)
	gcc $(CFLAGS) $^ -o kursor