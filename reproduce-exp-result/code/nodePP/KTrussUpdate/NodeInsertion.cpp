#include "../Graph/Edge.h"
#include "../Graph/GraphUtility.h"
#include "KTrussUpdate.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../KTrussIndex/VariantBinMapBased.h"
#include "NodeInsertion.h"
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <cassert>

using namespace std;
using namespace KTrussUpdate;

NodeInsertion::NodeInsertion(KTrussIndex *index, int v, const std::vector<int> &neighbors) {
    this->index = index;
    this->qV = v;
    this->neighbors = neighbors;
    this->graph = index->getGraph();
    this->gNV = GraphUtility::getInducedSubGraph(graph, neighbors);
}

NodeInsertion::~NodeInsertion() {
    delete gNV;
}

void NodeInsertion::run() {
    for (int u: neighbors)
        index->addEdgeToIndex(Edge(u, qV));

    setBound(true);
    setBound(false);
    for (int u: neighbors)
        assert(lowerBound[u] <= upperBound[u] && lowerBound[u] + 1 >= upperBound[u]);

    int maxK = 0;
    for (int u: neighbors) {
        index->setTruss(qV, u, lowerBound[u]);
        maxK = max(maxK, upperBound[u]);
    }

    vector<int> *edgeLevel = new vector<int>[maxK];
    unordered_set<int> *edgeSet = new unordered_set<int>[maxK];
    for (int u: neighbors)
        if (lowerBound[u] < upperBound[u]) {
            int level = lowerBound[u];
            edgeLevel[level].push_back(index->getId(qV, u));
            edgeSet[level].insert(index->getId(qV, u));
        }

    for (Edge e: gNV->getEdges()) {
        int edgeId = index->getId(e);
        int edgeTruss = index->getTruss(edgeId);
        if (edgeTruss < min(upperBound[e.getX()], upperBound[e.getY()])) {
            edgeLevel[edgeTruss].push_back(edgeId);
            edgeSet[edgeTruss].insert(edgeId);
        }
    }

    for (int k = maxK - 1; k >= 2; k--) {
        if (!edgeLevel[k].size())
            continue;
        VariantBinMapBased variantBin;
        unordered_map<int, int> support;
        // Edge Expansion
        for (int i = 0; i < (int) edgeLevel[k].size(); i++) {
            int edgeId = edgeLevel[k][i];
            Edge e = index->getEdge(edgeId);
            int v = e.getX(), u = e.getY();
            if (graph->deg(v) > graph->deg(u))
                swap(v, u);
            int count = 0;
            for (auto tmpE: graph->neighbors(v)) {
                int w = tmpE.other(v);
                if (graph->hasEdge(w, u)) {
                    int e1Id = index->getId(v, w);
                    int e1Truss = index->getTruss(e1Id);
                    if (e1Truss < k)
                        continue;
                    int e2Id = index->getId(u, w);
                    int e2Truss = index->getTruss(e2Id);
                    if (e2Truss < k)
                        continue;
                    count++;
                    if (e1Truss == k && edgeSet[k].find(e1Id) == edgeSet[k].end()) {
                        edgeLevel[k].push_back(e1Id);
                        edgeSet[k].insert(e1Id);
                    }
                    if (e2Truss == k && edgeSet[k].find(e2Id) == edgeSet[k].end()) {
                        edgeLevel[k].push_back(e2Id);
                        edgeSet[k].insert(e2Id);
                    }
                }
            }
            support[edgeId] = count;
            variantBin.insert(edgeId, count);
            assert(count >= k - 2);
        }
        // Edge Eviction
        if (variantBin.hasAnyBin(k - 2)) {
            for (int i = 0; i < (int) variantBin.getBin(k - 2).size(); i++) {
                int edgeId = variantBin.getBin(k - 2)[i];
                edgeSet[k].erase(edgeId);
                Edge e = index->getEdge(edgeId);
                int v = e.getX(), u = e.getY();
                if (graph->deg(v) > graph->deg(u))
                    swap(v, u);
                for (auto tmpE: graph->neighbors(v)) {
                    int w = tmpE.other(v);
                    if (graph->hasEdge(w, u)) {
                        int e1Id = index->getId(v, w);
                        int e2Id = index->getId(u, w);
                        int e1Truss = index->getTruss(e1Id);
                        int e2Truss = index->getTruss(e2Id);
                        if (e1Truss < k || e2Truss < k)
                            continue;
                        if (e1Truss == k && edgeSet[k].find(e1Id) == edgeSet[k].end())
                            continue;
                        if (e2Truss == k && edgeSet[k].find(e2Id) == edgeSet[k].end())
                            continue;
                        if (e1Truss == k && support[e1Id] > k - 2) {
                            support[e1Id]--;
                            variantBin.update(e1Id, support[e1Id], support[e1Id] + 1);
                        }
                        if (e2Truss == k && support[e2Id] > k - 2) {
                            support[e2Id]--;
                            variantBin.update(e2Id, support[e2Id], support[e2Id] + 1);
                        }
                    }
                }
            }
        }
        // Truss Update
        for (int edgeId: edgeLevel[k]) {
            if (edgeSet[k].find(edgeId) != edgeSet[k].end()) {
                assert(support[edgeId] > k - 2);
                int eTruss = index->getTruss(edgeId);
                assert(eTruss == k);
                index->setTruss(edgeId, eTruss + 1);
            }
        }
    }

    for (int i = 0; i < maxK; i++)
        edgeLevel[i].clear(), edgeSet[i].clear();
    delete[] edgeLevel;
    delete[] edgeSet;
}

void NodeInsertion::setBound(bool forLowerBound) {
    unordered_map<int, unordered_set<int> > relativeTriangles;
    typedef pair<int, int> pii;
    set <pii> degreeSet;
    vector <pair<int, Edge>> allEdges;

    const vector <Edge> &edges = gNV->getEdges();
    for (Edge edge: edges) {
        int v = edge.getX();
        int u = edge.getY();
        int truss = index->getTruss(edge.getX(), edge.getY());
        allEdges.push_back(pair<int, Edge>(
                truss, edge
        ));
        relativeTriangles[v].insert(u);
        relativeTriangles[u].insert(v);
    }
    sort(allEdges.begin(), allEdges.end());
    reverse(allEdges.begin(), allEdges.end());
    for (int node: neighbors)
        degreeSet.insert(pii(relativeTriangles[node].size(), node));

    int k = 2;
    int kDiff = forLowerBound ? 2 : 1;
    while (allEdges.size() && degreeSet.size()) {
        while (allEdges.size() && allEdges.back().first < k) {
            int truss;
            Edge e;
            tie(truss, e) = allEdges.back();
            allEdges.pop_back();
            int u = e.getX(), v = e.getY();
            if (relativeTriangles.count(u) && relativeTriangles.count(v)) {
                relativeTriangles[u].erase(v);
                relativeTriangles[v].erase(u);
                degreeSet.erase(pii(relativeTriangles[u].size() + 1, u));
                degreeSet.insert(pii(relativeTriangles[u].size(), u));
                degreeSet.erase(pii(relativeTriangles[v].size() + 1, v));
                degreeSet.insert(pii(relativeTriangles[v].size(), v));
            }
        }
        while (degreeSet.size() && degreeSet.begin()->first < k - kDiff) {
            int v = degreeSet.begin()->second;
            degreeSet.erase(degreeSet.begin());
            for (int u: relativeTriangles[v]) {
                relativeTriangles[u].erase(v);
                degreeSet.erase(pii(relativeTriangles[u].size() + 1, u));
                degreeSet.insert(pii(relativeTriangles[u].size(), u));
            }
            relativeTriangles.erase(v);
            if (forLowerBound) {
                lowerBound[v] = k - 1;
            } else {
                upperBound[v] = k;
            }
        }
        k++;
    }
    for (int v: neighbors) {
        lowerBound[v] = max(lowerBound[v], 2);
        upperBound[v] = max(upperBound[v], 2);
    }
}
