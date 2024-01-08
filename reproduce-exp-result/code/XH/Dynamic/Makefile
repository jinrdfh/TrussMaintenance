CC=/bin/g++ -march=native -w -O3
CFLAGS=-I../KTruss -I../TCP -std=c++0x -Wfatal-errors

all: tcp

tcp:
	$(CC) $(CFLAGS) *.cpp ../KTruss/util.cpp -o XH.out
clean: 
	rm -rf tcp
