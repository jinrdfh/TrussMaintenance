#include "Graph.h"
#include "Edge.h"
#include <vector>
#include <unordered_set>
#include <cassert>
#include <algorithm>

using namespace std;

Graph::Graph() {
    maxN = 1;
}

Graph::Graph(const Graph &graph) {
    maxN = graph.maxN;
    ng = graph.ng;
    nodes = graph.nodes;
    edges = graph.edges;
    for (Edge e: edges)
        adjMatrix.addEdge(e.getX(), e.getY());
}

Graph::~Graph() {
}

const Graph::Neighbors& Graph::neighbors(int v) {
    return ng[v];
}

int Graph::deg(int v) const {
    if (ng.count(v) > 0)
        return (int) ng.at(v).size();
    return 0;
}

void Graph::addEdge(int x, int y) {
    if (hasEdge(x, y))
        return;
    if (x > y)
        swap(x, y);
    Edge e = Edge(x, y);
    edges.push_back(e);
    ng[x].push_back(e);
    ng[y].push_back(e);
    if (y >= maxN)
        maxN = y + 1;
    adjMatrix.addEdge(x, y);
    nodes.insert(x);
    nodes.insert(y);
}

void Graph::deleteEdge(Edge e) {
    if (!hasEdge(e))
        throw;
    // Remove from edges
    {
        auto it = find(edges.begin(), edges.end(), e);
        assert(it != edges.end());
        edges.erase(it);
    }
    int x = e.getX(), y = e.getY();
    // Remove from ng
    {
        auto it = find(ng[x].begin(), ng[x].end(), e);
        assert(it != ng[x].end());
        ng[x].erase(it);

        it = find(ng[y].begin(), ng[y].end(), e);
        assert(it != ng[y].end());
        ng[y].erase(it);
    }
    adjMatrix.removeEdge(x, y);
}

void Graph::deleteNode(int v) {
    if (!hasNode(v))
        throw;

    vector<Edge> edgesToBeDeleted = neighbors(v);
    for (Edge e: edgesToBeDeleted)
        deleteEdge(e);
    nodes.erase(v);
}

void Graph::addEdge(Edge e) {
    addEdge(e.getX(), e.getY());
}

const vector<Edge>& Graph::getEdges() const {
    return edges;
}

bool Graph::hasEdge(int x, int y) const {
    if (x > y)
        swap(x, y);
    return adjMatrix.hasEdge(x, y);
}

bool Graph::hasEdge(Edge e) const {
    return hasEdge(e.getX(), e.getY());
}

bool Graph::hasNode(int v) const {
    return nodes.count(v);
}

bool Graph::addNode(int v) {
    if (v > maxN)
        maxN = v;
    return nodes.insert(v).second;
}

int Graph::getMaxN() const {
    return maxN;
}

void Graph::unionGraph(const Graph& g) {
    for (Edge e: g.getEdges())
        this->addEdge(e);
}

int Graph::getN() const {
    return (int) nodes.size();
}

int Graph::getM() const {
    return (int) edges.size();
}

const std::unordered_set<int> & Graph::getNodes() const {
    return nodes;
}

std::ostream& operator << (std::ostream &out, const Graph &graph) {
    out << "G(" << graph.getN() << ", " << graph.getM() << ") [\n";
    for (auto e: graph.getEdges())
        out << e << ",\n";
    out << "]";
    return out;
}
