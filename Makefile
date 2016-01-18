CC=clang
CFLAGS=-std=c11 -Wall -O2

all: bigint.o
	$(CC) $(CFLAGS) bigint.o test.c -o test

bigint: bigint.c bigint.h
	$(CC) $(CFLAGS) -c bigint.c

clean:
	rm bigint.o test
