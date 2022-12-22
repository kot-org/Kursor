CFLAGS = -Wall -g

SRC = $(wildcard src/*.c)
OBJS = $(SRC:.c=.o)

all: main link

main: $(OBJS)
	gcc $(CFLAGS) $^ -o $@

link:
	ld $(OBJS) -o kursor