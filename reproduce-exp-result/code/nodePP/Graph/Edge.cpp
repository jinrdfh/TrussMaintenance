#include "Edge.h"
#include <algorithm>

Edge::Edge(int x, int y) {
    setNodes(x, y);
}

void Edge::setNodes(int x, int y) {
    if (x > y)
        std::swap(x, y);
    this->x = x;
    this->y = y;
}

bool Edge::operator < (const Edge& e) const {
    return (this->getX() < e.getX() || (this->getX() == e.getX() && this->getY() < e.getY()));
}

bool Edge::operator == (const Edge& e) const {
    return this->getX() == e.getX() && this->getY() == e.getY();
}

int Edge::other(int z) const {
    if (z != x && z != y)
        throw;
    return x + y - z;
}

std::ostream& operator << (std::ostream &out, const Edge &edge) {
    out << "(" << edge.getX() << ", " << edge.getY() << ")";
    return out;
}
