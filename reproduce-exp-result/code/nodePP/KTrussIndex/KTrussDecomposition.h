#include "KTrussIndex.h"
#include "VariantBin.h"

#ifndef KTRUSS_V1_KTRUSSDECOMPOSITION_H
#define KTRUSS_V1_KTRUSSDECOMPOSITION_H

class KTrussDecomposition {
public:
    KTrussDecomposition(Graph *graph);
    ~KTrussDecomposition();

    KTrussIndex* getFinalIndex() const;
    void createIndex();
private:
    void calculateSupport();
    void initiateVariantBin();
    void calculateTruss();

    std::vector<int> getNodesDegreeSorted() const;
    Graph *graph;
    KTrussIndex *index;
    KTrussIndex *finalIndex;
    VariantBin variantBin;
};

#endif //KTRUSS_V1_KTRUSSDECOMPOSITION_H
