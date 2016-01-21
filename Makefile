CC=clang
CFLAGS=-std=c99 -Wall -O2

all: bigint.o test.c
	$(CC) $(CFLAGS) bigint.o test.c -o test

bigint: bigint.c bigint.h
	$(CC) $(CFLAGS) -c bigint.c

bm: bigint.o bm.c
	$(CC) $(CFLAGS) bigint.o bm.c -o bm

clean:
	rm bigint.o test bm
