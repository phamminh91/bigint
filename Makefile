CC=clang
CFLAGS=-std=c99 -Wall -O2

all: bigint.o test.c
	$(CC) $(CFLAGS) bigint.o test.c -o test

bigint: bigint.c bigint.h
	$(CC) $(CFLAGS) -c bigint.c

bm: bigint.o benchmark/bm.c
	$(CC) $(CFLAGS) bigint.o benchmark/bm.c -o benchmark/bm

gmp: benchmark/benchmark-gmp.c
	$(CC) $(CFLAGS) benchmark/benchmark-gmp.c -o benchmark/benchmark-gmp -lgmp

bmjava: benchmark/Benchmark.java
	javac benchmark/Benchmark.java

clean:
	rm bigint.o test bm
