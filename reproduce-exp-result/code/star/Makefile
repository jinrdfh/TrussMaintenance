# CC=g++ -march=native -w -O3
CC=g++ -O3 --std=c++0x
# CC=g++

star: star.o 
	$(CC)  -rdynamic -o star.out star.o
	
star.o: star.cpp
	$(CC) -rdynamic -c star.cpp
	
clean:
	rm *.o