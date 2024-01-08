#include "../KTrussIndex/KTrussIndex.h"
#include <vector>
#include <unordered_map>

#ifndef KTRUSS_V1_NODEINSERTION_H
#define KTRUSS_V1_NODEINSERTION_H

namespace KTrussUpdate {
    class NodeInsertion {
    public:
        NodeInsertion(KTrussIndex *index, int v, const std::vector<int> &neighbors);
        ~NodeInsertion();
        void run();

    private:
        void setBound(bool lowerBound);

        KTrussIndex *index;
        std::vector<int> neighbors;
        std::unordered_map<int, int> lowerBound, upperBound;
        int qV;
        Graph *graph;
        Graph *gNV;
    };
};


#endif //KTRUSS_V1_NODEINSERTION_H
