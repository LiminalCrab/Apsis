CC=gcc
CFLAGS?= -Wextra -Wall -O0 -g3 -lpthread -ldl -lm

main: main.o
	$(CC) -o main main.o -lSDL2 -lSDL2_ttf $(CFLAGS)

clean:
	rm -f main.o main
