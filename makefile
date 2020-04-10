CC=gcc
CFLAGS=-I.
DEPS = circular.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.o circular.o
	$(CC) -o main main.o circular.o

clean:
	rm -f *.o main
