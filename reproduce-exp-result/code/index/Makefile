CC=g++ -march=native -O3
CFLAGS=-c -I. -std=c++11 -Wfatal-errors

index: index.o decomp.o
	$(CC) index.o decomp.o -o index.out

index.o: index.cc decomp.h
	$(CC) $(CFLAGS) index.cc -o index.o

decomp.o: decomp.cc decomp.h
	$(CC) $(CFLAGS) decomp.cc -o decomp.o
