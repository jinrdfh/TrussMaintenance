#include <set>
#include "Edge.h"

#ifndef KTRUSS_V1_ADJACENCYMATRIX_H
#define KTRUSS_V1_ADJACENCYMATRIX_H

class AdjacencyMatrix {
public:
    AdjacencyMatrix();

    void addEdge(int x, int y);
    void removeEdge(int x, int y);
    bool hasEdge(int x, int y) const;
private:
    std::set<Edge> matrix;
};

#endif //KTRUSS_V1_ADJACENCYMATRIX_H
