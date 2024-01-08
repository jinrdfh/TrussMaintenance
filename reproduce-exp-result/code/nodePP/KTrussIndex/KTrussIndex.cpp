#include "KTrussIndex.h"
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <cassert>

using namespace std;

KTrussIndex::KTrussIndex(Graph* graph) {
    init(graph);
}

KTrussIndex::KTrussIndex() {}

void KTrussIndex::init(Graph* graph) {
    this->graph = graph;
    edges = graph->getEdges();
    for (int i = 0; i < (int) edges.size(); i++) {
        Edge e = edges[i];
        setEdgeId(e, i);
    }
}

KTrussIndex::~KTrussIndex() {
}

KTrussIndex::KTrussIndex(const KTrussIndex& kTruss) {
    init(kTruss.graph);
    for (int i = 0; i < (int) edges.size(); i++) {
        int id = kTruss.getId(edges[i]);
        truss[i] = kTruss.truss.at(id);
        sup[i] = kTruss.sup.at(id);
    }
    edgeId = kTruss.edgeId;
}

int KTrussIndex::getId(Edge e) const {
    return edgeId.at(e);
}

int KTrussIndex::getId(int x, int y) const {
    return getId(Edge(x, y));
}

Edge KTrussIndex::getEdge(int id) const {
    if (edges[id] == KTrussIndexConstants::nilEdge)
        throw ;
    return edges[id];
}

int KTrussIndex::getTruss(int eId) const {
    return truss.at(eId);
}

int KTrussIndex::getTruss(Edge e) const {
    int eId = getId(e);
    return getTruss(eId);
}

int KTrussIndex::getTruss(int x, int y) const {
    int eId = getId(x, y);
    return getTruss(eId);
}

int KTrussIndex::getSupport(int eId) const {
    return sup.at(eId);
}

bool KTrussIndex::hasEdgeInIndex(Edge e) const {
    return edgeId.count(e);
}

int KTrussIndex::addEdgeToIndex(Edge e) {
    if (hasEdgeInIndex(e))
        throw;
    assert(e.getX() != e.getY());
    int id = edges.size();
    edges.push_back(e);
    setEdgeId(e, id);
    return id;
}

void KTrussIndex::removeEdgeFromIndex(Edge e) {
    if (!hasEdgeInIndex(e))
        throw;

    int id = getId(e);
    edges[id] = KTrussIndexConstants::nilEdge;
    edgeId.erase(e);
    truss.erase(id);
    sup.erase(id);
}

void KTrussIndex::removeNodeFromIndex(int v) {
    if (!graph->hasNode(v))
        throw;

    for (Edge e: graph->neighbors(v))
        removeEdgeFromIndex(e);
}

void KTrussIndex::setEdgeId(Edge e, int id) {
    edgeId[e] = id;
    truss[id] = 0;
    sup[id] = 0;
}

void KTrussIndex::setTruss(int eId, int value) {
    truss[eId] = value;
}

void KTrussIndex::setTruss(Edge e, int value) {
    setTruss(getId(e), value);
}

void KTrussIndex::setTruss(int x, int y, int value) {
    setTruss(getId(x, y), value);
}

void KTrussIndex::setSupport(int eId, int value) {
    sup[eId] = value;
}

void KTrussIndex::incSupport(int eId) {
    sup[eId]++;
}

void KTrussIndex::decSupport(int eId) {
    sup[eId]--;
}

Graph* KTrussIndex::getGraph() const {
    return graph;
}

std::ostream& operator << (std::ostream &out, const KTrussIndex &kTrussIndex) {
    Graph *graph = kTrussIndex.getGraph();
    out << "Index(" << graph->getN() << ", " << graph->getM() << ") [\n";
    for (int i = 0; i < (int) kTrussIndex.edges.size(); i++)
        out << "(" << i << ": " << kTrussIndex.edges[i] << " Truss:" << kTrussIndex.truss.at(i) << "),\n";
    out << "]";
    return out;
}
