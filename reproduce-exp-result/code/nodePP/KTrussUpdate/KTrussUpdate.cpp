#include "../Graph/Edge.h"
#include "../Graph/GraphUtility.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "../KTrussIndex/VariantBinMapBased.h"
#include "KTrussUpdate.h"
#include "NodeInsertion.h"
#include "EdgeInsertion.h"
#include "NodeDeletion.h"
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <cassert>

using namespace std;
using namespace KTrussUpdate;

void KTrussUpdate::updateNodeInsertion(int v, const vector<int> &neighbors, KTrussIndex *index) {
    NodeInsertion nodeInsertion(index, v, neighbors);
    nodeInsertion.run();
}

void KTrussUpdate::updateEdgeInsertion(Edge e, KTrussIndex *index) {
    EdgeInsertion edgeInsertion(index, e);
    edgeInsertion.run();
}

void KTrussUpdate::updateNodeDeletion(int v, KTrussIndex *index) {
    NodeDeletion nodeDeletion(index, v);
    nodeDeletion.run();
}
