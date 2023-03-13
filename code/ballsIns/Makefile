# CC=g++ -march=native -w -O3
CC=g++ -O3 --std=c++0x
# CC=g++

file = file.o file.h
insertPart = insertPart.o insertPart.h
common = common.h tool/debug.h tool/myIntAHash.h tool/myAHash.h tool/myPriQueue.h graph.h
myG = myG.o myG.h


ballsIns: ballsIns.o $(common) $(file) $(myG) $(insertPart)
	$(CC)  -rdynamic -o ballsIns.out ballsIns.o file.o myG.o insertPart.o
	
ballsIns.o: ballsIns.cpp $(common) file.h myG.h insertPart.h
	$(CC) -rdynamic -c ballsIns.cpp

file.o: file.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c file.cpp

insertPart.o: insertPart.cpp $(common) insertPart.h myG.h
	$(CC) -rdynamic -c insertPart.cpp
	
myG.o: myG.cpp myG.h $(common)
	$(CC) -rdynamic -c myG.cpp
	
clean:
	rm *.o