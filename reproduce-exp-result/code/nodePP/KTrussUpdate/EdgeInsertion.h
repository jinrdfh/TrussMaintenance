#include "../KTrussIndex/KTrussIndex.h"
#include <vector>

#ifndef KTRUSS_V1_EDGEINSERTION_H
#define KTRUSS_V1_EDGEINSERTION_H

namespace KTrussUpdate {
    class EdgeInsertion {
    public:
        EdgeInsertion(KTrussIndex *index, Edge e);
        ~EdgeInsertion();
        void run();

    private:
        std::pair<int, int> getBound();

        Edge qEdge;
        KTrussIndex *index;
        Graph *graph;
    };
};

#endif //KTRUSS_V1_EDGEINSERTION_H
