#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "AdjacencyMatrix.h"
#include "Edge.h"

#ifndef KTRUSS_V1_GRAPH_H
#define KTRUSS_V1_GRAPH_H

class Graph {
public:
    typedef std::vector<Edge> Neighbors;
    Graph();
    Graph(const Graph& graph);
    ~Graph();

    // Insert / Delete
    void addEdge(int x, int y);
    void addEdge(Edge e);
    bool addNode(int v);
    void deleteEdge(Edge e);
    void deleteNode(int v);
    void unionGraph(const Graph& g);

    // Existence check
    bool hasEdge(int x, int y) const;
    bool hasEdge(Edge e) const;
    bool hasNode(int v) const;

    // Properties
    int getMaxN() const;
    int getM() const;
    int getN() const;
    int deg(int v) const;
    const std::unordered_set<int>& getNodes() const;
    const Neighbors& neighbors(int v);
    const std::vector<Edge>& getEdges() const;

    friend std::ostream& operator << (std::ostream &out, const Graph &graph);
private:
    AdjacencyMatrix adjMatrix;
    std::unordered_map<int, Neighbors> ng;
    std::unordered_set<int> nodes;
    std::vector<Edge> edges;
    int maxN;
};

#endif //KTRUSS_V1_GRAPH_H
