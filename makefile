CC=gcc
CFLAGS?=-Werror -Wextra -Wall -O0 -g3 -pthread
main: main.o server.o
	$(CC) $(CFLAGS) -o main main.o server.o

clean:
	rm -f main.o server.o