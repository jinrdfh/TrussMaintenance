

LIBS = -lpthread

TARGET = nodePP.out

FLAGS = --std=c++0x -Wall -DNDEBUG -g

path_KTrussIndex = KTrussIndex
vpath KTrussIndex.cpp $(path_KTrussIndex)
vpath KTrussIndex.h $(path_KTrussIndex)
vpath VariantBinMapBased.h $(path_KTrussIndex)


path_fileIO = fileIO
vpath fileIO.cpp $(path_fileIO)
vpath fileIO.h $(path_fileIO)

path_Graph = Graph
vpath Graph.cpp $(path_Graph)
vpath Graph.h $(path_Graph)
vpath Edge.cpp $(path_Graph)
vpath Edge.h $(path_Graph)
vpath AdjacencyMatrix.cpp $(path_Graph)
vpath AdjacencyMatrix.h $(path_Graph)
vpath GraphUtility.cpp $(path_Graph)
vpath GraphUtility.h $(path_Graph)
Graph_o = Graph.o Edge.o AdjacencyMatrix.o GraphUtility.o

path_KTrussUpdate = KTrussUpdate
vpath KTrussUpdate.cpp $(path_KTrussUpdate)
vpath KTrussUpdate.h $(path_KTrussUpdate)
vpath NodeInsertion.cpp $(path_KTrussUpdate)
vpath NodeInsertion.h $(path_KTrussUpdate)
vpath EdgeInsertion.cpp $(path_KTrussUpdate)
vpath EdgeInsertion.h $(path_KTrussUpdate)
vpath NodeDeletion.cpp $(path_KTrussUpdate)
vpath NodeDeletion.h $(path_KTrussUpdate)

KTrussUpdate_o = KTrussUpdate.o NodeInsertion.o EdgeInsertion.o NodeDeletion.o

all: ${TARGET} 

${TARGET}: main.o KTrussIndex.o fileIO.o $(Graph_o) $(KTrussUpdate_o)
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -o ${TARGET} $^ ${LIBS}

main.o: main.cpp KTrussIndex.h fileIO.h KTrussUpdate.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c main.cpp

KTrussIndex.o: KTrussIndex.cpp KTrussIndex.h Graph.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_KTrussIndex)/KTrussIndex.cpp

fileIO.o: fileIO.cpp fileIO.h KTrussIndex.h Graph.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_fileIO)/fileIO.cpp

Graph.o: Graph.cpp Graph.h Edge.h AdjacencyMatrix.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_Graph)/Graph.cpp

Edge.o: Edge.cpp Edge.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_Graph)/Edge.cpp

AdjacencyMatrix.o: AdjacencyMatrix.cpp AdjacencyMatrix.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_Graph)/AdjacencyMatrix.cpp

GraphUtility.o: GraphUtility.cpp GraphUtility.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_Graph)/GraphUtility.cpp

KTrussUpdate.o: KTrussUpdate.cpp KTrussUpdate.h Edge.h GraphUtility.h KTrussIndex.h VariantBinMapBased.h NodeInsertion.h EdgeInsertion.h NodeDeletion.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_KTrussUpdate)/KTrussUpdate.cpp

NodeInsertion.o: NodeInsertion.cpp NodeInsertion.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_KTrussUpdate)/NodeInsertion.cpp

EdgeInsertion.o: EdgeInsertion.cpp EdgeInsertion.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_KTrussUpdate)/EdgeInsertion.cpp

NodeDeletion.o: NodeDeletion.cpp NodeDeletion.h
	${CXX} $(FLAGS) $(CXXFLAGS) $(CPPFLAGS) -c $(path_KTrussUpdate)/NodeDeletion.cpp

clean:
	@$(RM) */*.o
	@$(RM) *.o
	@$(RM) $(TARGET)

.PHONY: all clean
