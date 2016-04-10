CC=gcc
#CFLAGS=-g -Wall
CFLAGS=-Wall

shell: lib.o lexer.o cachenode.o parser.o execute.o shell.o
	$(CC) $(CFLAGS) -o shell lib.o lexer.o cachenode.o parser.o execute.o shell.o


clean:
	rm -rf *.o
