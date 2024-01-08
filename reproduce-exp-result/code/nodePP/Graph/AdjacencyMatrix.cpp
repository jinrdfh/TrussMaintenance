#include "AdjacencyMatrix.h"

using namespace std;

AdjacencyMatrix::AdjacencyMatrix() {
}

void AdjacencyMatrix::addEdge(int x, int y) {
    matrix.insert(Edge(x, y));
}

void AdjacencyMatrix::removeEdge(int x, int y) {
    matrix.erase(Edge(x, y));
}

bool AdjacencyMatrix::hasEdge(int x, int y) const {
    return matrix.find(Edge(x, y)) != matrix.end();
}

