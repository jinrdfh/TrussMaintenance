#include "EdgeInsertion.h"
#include "../KTrussIndex/VariantBinMapBased.h"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstring>

using namespace std;
using namespace KTrussUpdate;

EdgeInsertion::EdgeInsertion(KTrussIndex *index, Edge e) {
    this->index = index;
    this->qEdge = e;
    this->graph = index->getGraph();
}

EdgeInsertion::~EdgeInsertion() {
}

void EdgeInsertion::run() {
    index->addEdgeToIndex(qEdge);
    int qEdgeId = index->getId(qEdge);

    int lowerBound, upperBound;
    tie(lowerBound, upperBound) = getBound();
    index->setTruss(qEdgeId, lowerBound);

    int maxK = upperBound;
    vector<int> *edgeLevel = new vector<int>[maxK];
    unordered_set<int> *edgeSet = new unordered_set<int>[maxK];

    if (lowerBound < upperBound) {
        edgeLevel[lowerBound].push_back(qEdgeId);
        edgeSet[lowerBound].insert(qEdgeId);
    }

    {
        int v = qEdge.getX(), u = qEdge.getY();
        if (graph->deg(v) > graph->deg(u))
            swap(u, v);
        for (Edge tmpEdge: graph->neighbors(v)) {
            int w = tmpEdge.other(v);
            if (graph->hasEdge(w, u)) {
                int e1Id = index->getId(v, w);
                int e2Id = index->getId(u, w);
                int e1Truss = index->getTruss(e1Id);
                int e2Truss = index->getTruss(e2Id);
                int minTruss = min(e1Truss, e2Truss);
                if (minTruss >= maxK)
                    continue;

                if (e1Truss == minTruss) {
                    edgeLevel[e1Truss].push_back(e1Id);
                    edgeSet[e1Truss].insert(e1Id);
                }
                if (e2Truss == minTruss) {
                    edgeLevel[e2Truss].push_back(e2Id);
                    edgeSet[e2Truss].insert(e2Id);
                }
            }
        }
    }

	int tmpCnt = 0;

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
            for (Edge tmpE: graph->neighbors(v)) {
				tmpCnt++;
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
                for (Edge tmpE: graph->neighbors(v)) {
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
            if (edgeSet[k].find(edgeId) != edgeSet[k].end())
            {
                assert(support[edgeId] > k - 2);
                int eTruss = index->getTruss(edgeId);
                assert(eTruss == k);
                index->setTruss(edgeId, eTruss + 1);
                //cout << "set eid: " << edgeId << " edge: " << index->getEdge(edgeId)<< " trussness: " << eTruss + 1 << endl;
            }
        }
    }

    for (int i = 0; i < maxK; i++)
        edgeLevel[i].clear(), edgeSet[i].clear();
    delete[] edgeLevel;
    delete[] edgeSet;

	//cout << " At " << qEdge << " cnt: " << tmpCnt << endl;
}

pair<int, int> EdgeInsertion::getBound() {
    int v = qEdge.getX(), u = qEdge.getY();
    if (graph->deg(v) > graph->deg(u))
        swap(v, u);

    int kMax = graph->deg(v) + 2;
    int *count = new int[kMax + 1];
    memset(count, 0, (kMax + 1) * sizeof(int));
    for (Edge tmpEdge: graph->neighbors(v)) {
        int w = tmpEdge.other(v);
        if (graph->hasEdge(w, u)) {
            int e1Id = index->getId(v, w);
            int e2Id = index->getId(u, w);
            int e1Truss = index->getTruss(e1Id);
            int e2Truss = index->getTruss(e2Id);
            int k = min(kMax, min(e1Truss, e2Truss));
            for (int i = 1; i <= k; i++)
                count[i]++;
        }
    }
    int lowerBound = 2;
    int upperBound = 2;
    for (int k = 2; k <= kMax; k++) {
        if (count[k] >= k - 2)
            lowerBound = k;
        if (count[k - 1] >= k - 2)
            upperBound = k;
    }
    delete[] count;
    return pair<int, int>(lowerBound, upperBound);
}
