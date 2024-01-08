#include "NodeDeletion.h"
#include "../Graph/GraphUtility.h"
#include "../KTrussIndex/VariantBinMapBased.h"
#include <queue>
#include <map>

using namespace std;
using namespace KTrussUpdate;

NodeDeletion::NodeDeletion(KTrussIndex *index, int v) {
    this->index = index;
    this->qV = v;
    this->graph = index->getGraph();
}

NodeDeletion::~NodeDeletion() {
}

void NodeDeletion::run() {
    int maxK = 0;
    vector<int> neighbors;
    map<int, int> neighborTruss;
    for (Edge e: graph->neighbors(qV)) {
        int u = e.other(qV);
        int qvUTruss = index->getTruss(e);
        maxK = max(maxK, qvUTruss);
        neighbors.push_back(u);
        neighborTruss[u] = qvUTruss;
    }

    // First level affected edges
    vector<int> *edgeLevel = new vector<int>[maxK + 1];
    unordered_set<int> *edgeSet = new unordered_set<int>[maxK + 1];
    Graph *gNV = GraphUtility::getInducedSubGraph(graph, neighbors);
    for (Edge e: gNV->getEdges()) {
        int edgeId = index->getId(e);
        int edgeTruss = index->getTruss(edgeId);
        if (edgeTruss <= min(neighborTruss[e.getX()], neighborTruss[e.getY()])) {
            edgeSet[edgeTruss].insert(edgeId);
            edgeLevel[edgeTruss].push_back(edgeId);
        }
    }

    for (int k = 3; k <= maxK; k++) {
        if (!edgeLevel[k].size())
            continue;
        unordered_map<int, int> support;

        queue<int> edgeIDsToDelete;
        for (int i = 0; i < (int) edgeLevel[k].size(); i++) {
            // Calculate sup(e)
            {
                int edgeId = edgeLevel[k][i];
                Edge e = index->getEdge(edgeId);
                int v = e.getX(), u = e.getY();
                if (graph->deg(v) > graph->deg(u))
                    swap(v, u);
                int count = 0;
                for (auto tmpE: graph->neighbors(v)) {
                    int w = tmpE.other(v);
                    if (w == qV)
                        continue;

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
                    }
                }
                support[edgeId] = count;

                if (count <= k - 3)
                    edgeIDsToDelete.push(edgeId);
            }

            while (!edgeIDsToDelete.empty()) {
                int edgeId = edgeIDsToDelete.front();
                edgeIDsToDelete.pop();
                assert(index->getTruss(edgeId) == k);
                index->setTruss(edgeId, k - 1);

                Edge edgeToDelete = index->getEdge(edgeId);
                int v = edgeToDelete.getX(), u = edgeToDelete.getY();
                if (graph->deg(v) > graph->deg(u))
                    swap(v, u);

                for (auto tmpE: graph->neighbors(v)) {
                    int w = tmpE.other(v);
                    if (w == qV)
                        continue;

                    if (graph->hasEdge(w, u)) {
                        int e1Id = index->getId(v, w);
                        int e1Truss = index->getTruss(e1Id);
                        if (e1Truss < k)
                            continue;
                        int e2Id = index->getId(u, w);
                        int e2Truss = index->getTruss(e2Id);
                        if (e2Truss < k)
                            continue;

                        if (e1Truss == k) {
                            if (edgeSet[k].find(e1Id) == edgeSet[k].end()) {
                                edgeLevel[k].push_back(e1Id);
                                edgeSet[k].insert(e1Id);
                            } else {
                                support[e1Id]--;
                                if (support[e1Id] == k - 3)
                                    edgeIDsToDelete.push(e1Id);
                            }
                        }
                        if (e2Truss == k) {
                            if (edgeSet[k].find(e2Id) == edgeSet[k].end()) {
                                edgeLevel[k].push_back(e2Id);
                                edgeSet[k].insert(e2Id);
                            } else {
                                support[e2Id]--;
                                if (support[e2Id] == k - 3)
                                    edgeIDsToDelete.push(e2Id);
                            }
                        }
                    }
                }
            }
        }
    }

    index->removeNodeFromIndex(qV);
    delete gNV;
}
