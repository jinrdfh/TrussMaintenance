# CC=g++ -march=native -w -O3
CC=g++ -O3 --std=c++0x
# CC=g++

file = file.o file.h
delPart = delPart.o delPart.h
common = common.h tool/debug.h tool/myIntAHash.h tool/myAHash.h tool/myPriQueue.h graph.h
myG = myG.o myG.h

ballDel: ballDel.o $(common) $(file) $(myG) $(delPart)
	$(CC)  -rdynamic -o ballDel.out ballDel.o file.o myG.o delPart.o
	
ballDel.o: ballDel.cpp $(common) file.h myG.h delPart.h
	$(CC) -rdynamic -c ballDel.cpp

file.o: file.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c file.cpp

delPart.o: delPart.cpp $(common) delPart.h myG.h 
	$(CC) -rdynamic -c delPart.cpp
	
myG.o: myG.cpp myG.h $(common)
	$(CC) -rdynamic -c myG.cpp
	
clean:
	rm *.o