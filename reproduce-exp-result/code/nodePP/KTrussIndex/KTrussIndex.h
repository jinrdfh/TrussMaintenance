#include "../Graph/Graph.h"
#include <unordered_map>
#include <map>

#ifndef KTRUSS_V1_KTRUSSINDEX_H
#define KTRUSS_V1_KTRUSSINDEX_H

class KTrussIndex {
public:
    KTrussIndex(Graph *graph);
    KTrussIndex(const KTrussIndex& kTruss);
    KTrussIndex();
    ~KTrussIndex();
    void init(Graph* graph);

    // Truss
    int getTruss(int eId) const;
    int getTruss(int x, int y) const;
    int getTruss(Edge e) const;
    void setTruss(int eId, int value);
    void setTruss(int x, int y, int value);
    void setTruss(Edge e, int value);

    // Support
    int getSupport(int eId) const;
    void setSupport(int eId, int value);
    void incSupport(int eId);
    void decSupport(int eId);

    // Graph related
    void createEdgeIndex();
    int addEdgeToIndex(Edge e);
    void removeEdgeFromIndex(Edge e);
    void removeNodeFromIndex(int v);

    int getId(Edge e) const;
    int getId(int x, int y) const;
    Edge getEdge(int id) const;
    bool hasEdgeInIndex(Edge e) const;
    Graph* getGraph() const;

    friend std::ostream& operator << (std::ostream &out, const KTrussIndex &kTrussIndex);
private:
    void setEdgeId(Edge e, int id);
    std::unordered_map<int, int> truss;
    std::unordered_map<int, int> sup;
    std::map<Edge, int> edgeId;
    std::vector<Edge> edges;
    Graph *graph;
};

namespace KTrussIndexConstants {
    const Edge nilEdge = Edge(-1, -1);
};

#endif //KTRUSS_V1_KTRUSSINDEX_H
