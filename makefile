CC=gcc
CFLAGS?=-Werror -Wextra -Wall -O0 -g3 -pthread -lSDL2

main: main.o
	$(CC) $(CFLAGS)  -o main main.o

clean:
	rm -f main.o main
