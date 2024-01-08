# CC=g++ -march=native -w -O3
CC=g++ -O3 --std=c++0x
# CC=g++

file = file.o file.h
# common = common.h tool/debug.h tool/myIntA.h graph.h
common = common.h tool/debug.h tool/myIntAHash.h tool/myAHash.h tool/myPriQueue.h graph.h
myG = myG.o myG.h


total: graphSampleNew

graphSample2Hop: graphSample2Hop.o $(common) $(file) $(myG)
	$(CC)  -rdynamic -o graphSample2Hop.out graphSample2Hop.o file.o myG.o
	
graphSample2Hop.o: graphSample2Hop.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c graphSample2Hop.cpp

graphSampleNew: graphSampleNew.o $(common) $(file) $(myG)
	$(CC)  -rdynamic -o graphSampleNew.out graphSampleNew.o file.o myG.o
	
graphSampleNew.o: graphSampleNew.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c graphSampleNew.cpp

graphSampleBFS: graphSampleBFS.o $(common) $(file) $(myG)
	$(CC)  -rdynamic -o graphSampleBFS.out graphSampleBFS.o file.o myG.o
	
graphSampleBFS.o: graphSampleBFS.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c graphSampleBFS.cpp

graphSampleConnected: graphSampleConnected.o $(common) $(file) $(myG)
	$(CC)  -rdynamic -o graphSampleConnected.out graphSampleConnected.o file.o myG.o
	
graphSampleConnected.o: graphSampleConnected.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c graphSampleConnected.cpp

graphSample: graphSample.o $(common) $(file) $(myG)
	$(CC)  -rdynamic -o graphSample.out graphSample.o file.o myG.o
	
graphSample.o: graphSample.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c graphSample.cpp

file.o: file.cpp $(common) file.h myG.h
	$(CC) -rdynamic -c file.cpp
	
myG.o: myG.cpp myG.h $(common)
	$(CC) -rdynamic -c myG.cpp
	
clean:
	rm *.o