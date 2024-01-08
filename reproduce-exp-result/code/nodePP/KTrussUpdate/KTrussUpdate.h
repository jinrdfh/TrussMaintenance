#include "../KTrussIndex/KTrussIndex.h"
#include <vector>
#include <unordered_map>

#ifndef KTRUSS_V1_KTRUSSUPDATE_H
#define KTRUSS_V1_KTRUSSUPDATE_H

namespace KTrussUpdate {
    void updateNodeInsertion(int v, const std::vector<int> &neighbors, KTrussIndex *index);
    void updateEdgeInsertion(Edge e, KTrussIndex *index);
    void updateNodeDeletion(int v, KTrussIndex *index);
};

#endif //KTRUSS_V1_KTRUSSUPDATE_H
