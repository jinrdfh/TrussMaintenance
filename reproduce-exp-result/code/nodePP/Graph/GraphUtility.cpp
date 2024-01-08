#include "GraphUtility.h"
#include <vector>
#include <unordered_set>

using namespace std;

Graph* GraphUtility::getInducedSubGraph(Graph* g, std::vector<int> qV) {
    unordered_set<int> qVSet(qV.begin(), qV.end());
    Graph *ansG = new Graph();
    for (auto v: qV) {
        ansG->addNode(v);
        Graph::Neighbors vNeighbors = g->neighbors(v);
        if (vNeighbors.size() < qV.size()) {
            for (auto e: vNeighbors) {
                int u = e.other(v);
                if (qVSet.count(u))
                    ansG->addEdge(e);
            }
        } else {
            for (auto u: qV) {
                Edge e(u, v);
                if (g->hasEdge(e))
                    ansG->addEdge(e);
            }
        }
    }
    return ansG;
}

