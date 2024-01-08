#include "KTrussQuery.h"
#include <queue>
#include <unordered_map>

using namespace std;

Graph KTrussQuery::findKTruss(int qV, int k, KTrussIndex *index) {
    Graph* graph = index->getGraph();
    Graph result;
    queue<int> q;
    unordered_set<int> visited;
    q.push(qV);
    visited.insert(qV);
    while (!q.empty()) {
        int v = q.front(); q.pop();
        Graph::Neighbors ngV = graph->neighbors(v);
        for (Edge e: ngV) {
            int u = e.other(v);
            if (index->getTruss(u, v) < k)
                continue;
            result.addEdge(e);
            if (!visited.count(u)) {
                q.push(u);
                visited.insert(u);
            }
        }
    }
    return result;
}
