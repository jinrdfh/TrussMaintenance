#include "KTrussDecomposition.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <cstring>

using namespace std;

KTrussDecomposition::KTrussDecomposition(Graph *graph) {
    this->graph = graph;
    index = new KTrussIndex(graph);
    finalIndex = nullptr;
}

KTrussDecomposition::~KTrussDecomposition() {
//	delete index;
}

KTrussIndex* KTrussDecomposition::getFinalIndex() const {
    return finalIndex;
}

vector<int> KTrussDecomposition::getNodesDegreeSorted() const {
    vector<pair<int, int> > nodes;
    for (int node: graph->getNodes())
        nodes.push_back(pair<int, int>(graph->deg(node), node));
    sort(nodes.begin(), nodes.end());

    vector<int> sortedNodes;
    for (int i = 0; i < (int) nodes.size(); i++)
        sortedNodes.push_back(nodes[i].second);

    return sortedNodes;
}

void KTrussDecomposition::calculateSupport() { // Forward Algorithm
    // sort nodes by degree
    vector<int> nodes = getNodesDegreeSorted();
    reverse(nodes.begin(), nodes.end()); // descending
    int n = nodes.size();
    unordered_map<int, int> place;
    for (int i = 0; i < n; i++)
        place[nodes[i]] = i;

    // create forward graph
    unordered_map<int, vector<int> > ngEdgeId;
    for (int i = 0; i < n; i++) {
        int v = nodes[i];
        Graph::Neighbors neighbors = graph->neighbors(v);
        for (int j = 0; j < (int) neighbors.size(); j++) {
            Edge e = neighbors[j];
            int u = e.other(v);
            if (place[u] < place[v])
                ngEdgeId[v].push_back(index->getId(e));
        }
    }

    // calculate support
    for (int i = 0; i < n; i++) {
        int v = nodes[i];
        for (int j = 0; j < (int) ngEdgeId[v].size(); j++)
            for (int k = 0; k < j; k++) {
                int e1Id = ngEdgeId[v][j];
                int e2Id = ngEdgeId[v][k];
                int u = index->getEdge(e1Id).other(v);
                int w = index->getEdge(e2Id).other(v);
                if (graph->hasEdge(u, w)) {
                    index->incSupport(e1Id);
                    index->incSupport(e2Id);
                    int e3Id = index->getId(Edge(u, w));
                    index->incSupport(e3Id);
                }
            }
    }
}

void KTrussDecomposition::initiateVariantBin() {
    int m = graph->getM();
    int maxSupport = 0;
    for (int i = 0; i < m; i++)
        maxSupport = max(maxSupport, index->getSupport(i));
    variantBin.init(m, maxSupport + 1);
    for (int i = 0; i < m; i++)
        variantBin.insert(i, index->getSupport(i));
}

void KTrussDecomposition::calculateTruss() {
    vector<int> bin0 = variantBin.getBin(0);
    for (int i = 0; i < (int) bin0.size(); i++)
        index->setTruss(bin0[i], 2);

    AdjacencyMatrix adjMatrix;
    unordered_map<int, vector<int> > ngEdgeId;
    int m = graph->getM();
    bool *available = new bool[m];
    memset(available, 0, m * sizeof(bool));
    for (int i = 0; i < m; i++)
        if (index->getSupport(i)) {
            Edge e = index->getEdge(i);
            int u = e.getX(), v = e.getY();
            ngEdgeId[u].push_back(i);
            ngEdgeId[v].push_back(i);
            adjMatrix.addEdge(u, v);
            available[i] = true;
        }

    for (int k = 3; k < variantBin.getMaxScore() + 2; k++) {
        int minTriangles = k - 2;
        for (int j = 0; j < (int) variantBin.getBin(minTriangles).size(); j++) {
            int edgeId = variantBin.getBin(minTriangles)[j];
            Edge e = index->getEdge(edgeId);
            int v = e.getX(), u = e.getY();
            if (ngEdgeId[v].size() > ngEdgeId[u].size())
                swap(v, u);
            vector<int>& ngV = ngEdgeId[v];
            for (int i = 0; i < (int) ngV.size(); i++) {
                int e1Id = ngV[i];
                if (available[e1Id]) {
                    int w = index->getEdge(e1Id).other(v);
                    if (adjMatrix.hasEdge(u, w)) {
                        int e2Id = index->getId(Edge(u, w));
                        if (available[e2Id]) {
                            int e2Sup = index->getSupport(e2Id);
                            if (e2Sup > minTriangles) {
                                index->decSupport(e2Id);
                                variantBin.update(e2Id, e2Sup - 1, e2Sup);
                            }
                            int e1Sup = index->getSupport(e1Id);
                            if (e1Sup > minTriangles) {
                                index->decSupport(e1Id);
                                variantBin.update(e1Id, e1Sup - 1, e1Sup);
                            }
                        }
                    }
                } else {
                    swap(ngV[i], ngV[(int) ngV.size() - 1]);
                    ngV.pop_back();
                    i--;
                }
            }
            available[edgeId] = false;
            index->setTruss(edgeId, k);
            adjMatrix.removeEdge(v, u);
        }
    }
    delete[] available;
}

void KTrussDecomposition::createIndex() {
    calculateSupport();
    initiateVariantBin();
    calculateTruss();
//    finalIndex = new KTrussIndex(*index);
    finalIndex = index;
}

