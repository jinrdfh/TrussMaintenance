#include "../KTrussIndex/KTrussIndex.h"


#ifndef KTRUSS_V1_NODEDELETION_H
#define KTRUSS_V1_NODEDELETION_H

namespace KTrussUpdate {
    class NodeDeletion {
    public:
        NodeDeletion(KTrussIndex *index, int v);
        ~NodeDeletion();
        void run();
    private:
        KTrussIndex *index;
        Graph *graph;
        int qV;
    };
};

#endif //KTRUSS_V1_NODEDELETION_H
