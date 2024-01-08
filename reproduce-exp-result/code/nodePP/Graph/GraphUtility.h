#include <vector>
#include "Graph.h"

#ifndef KTRUSS_V1_GRAPHUTILITY_H
#define KTRUSS_V1_GRAPHUTILITY_H

namespace GraphUtility {
    Graph* getInducedSubGraph(Graph *g, std::vector<int> qV);
};

#endif //KTRUSS_V1_GRAPHUTILITY_H
