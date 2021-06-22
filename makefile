CC=gcc
CFLAGS?=-Werror -Wextra -Wall -O0 -g3 -pthread
main: main.o synapsys.o metronome.o
	$(CC) $(CFLAGS) -o main main.o synapsys.o metronome.o

clean:
	rm -f main.o synapsys.o metronome.o main 