#include <iostream>

#ifndef KTRUSS_V1_EDGE_H
#define KTRUSS_V1_EDGE_H

class Edge {
public:
    Edge(): x(-1), y(-1) {}
    Edge(int x, int y);
    void updateEdge(int x, int y) { setNodes(x, y); }
    int getX() const { return x; }
    int getY() const { return y; }
    int other(int z) const;
    bool operator < (const Edge& e) const;
    bool operator == (const Edge& e) const;

    friend std::ostream& operator << (std::ostream &out, const Edge &edge);
private:
    int x, y;
    void setNodes(int x, int y);
};

#endif //KTRUSS_V1_EDGE_H
