#ifndef H_EVOLVINGGRAPH
#define H_EVOLVINGGRAPH
#include "util.h"
#include "graphutils.h"
#include "QueryTCPGraph.h"
//#define option_ordered_Edges
namespace sinz{
typedef unsigned int IndexType;
typedef unsigned int NodeIndex;
namespace util{
template<class _VT, class _FT>
void showVV(vector<vector<_VT> >& v, _FT _Output){
  cerr <<"{";
  for (size_t i = 0; i<v.size(); i++){
    if (i > 0 )
      cerr <<", ";
    cerr << i << ":{";
    for (size_t j= 0; j< v[i].size(); j++){
      if (j > 0 )
        cerr <<", ";
      _Output(v[i][j]);

    }
    cerr <<"}";
  }
  cerr << "}\n";
}
template<class T, class T2>
bool KeyValueCmp(const pair<T, T2>& a, const pair<T, T2>& b){
  return a.first < b.first;
}
template<class T, class T2>
bool KeyValueCmpGreater(const pair<T, T2>& a, const pair<T, T2>& b){
  return a.first > b.first;
}
template<class T, class T2>
class KeyValueGreater{
 public:
  bool operator()(const pair<T, T2>& a, const pair<T, T2>& b){
    return a.first > b.first;
  }
};
template<class T, class T2>
int getValue(vector<pair<T, T2> > & v, T i){
  auto it = lower_bound(v.begin(), v.end(), make_pair(i, -1), KeyValueCmp<T, T2>);
  if (it == v.end() || i != it->first)
    return -1;
  return it->second;
}
template<class T, class T2>
int getValueInc( pair<T, T2>*& v_begin, pair<T, T2>* v_end, const T& key, size_t min_len = 10000){
  if (v_begin != v_end && key > (v_end - 1)->first)
    return -1;
  if (v_end - v_begin > min_len){
    v_begin = lower_bound(v_begin, v_end, make_pair(key, -1), KeyValueCmp<T, T2>);
  }
  else{
    while (v_begin != v_end && (v_begin->first < key))
      v_begin++;
  }

  if (v_begin == v_end || key != v_begin->first)
    return -1;
  int ret = v_begin->second;
  v_begin++;
  return ret;
}
template<class T, class T2>
int getValueIncStep( pair<T, T2>*& v_begin, pair<T, T2>* v_end, const T& key){
  while (v_begin != v_end && v_begin->first < key)
    v_begin++;
  if (v_begin == v_end || key != v_begin->first)
    return -1;
  int ret = v_begin->second;
  v_begin++;
  return ret;
}
template<class T, class T2>
T2 InsertIntoVector(vector<pair<T, T2> > & v, pair<T, T2>&& a){
#define debug_2013_5_12
#ifndef debug_2013_5_12
  v.push_back(make_pair((T2)-1, (T)-1));
  size_t j;
  for ( j = v.size() - 1 ; j>=0; j--){
    if (j ==0 || a.first > v[j - 1].first)
      break;
    v[j] = v[j - 1];
  }
  v[j] = a;
  return j;
#else
  v.push_back(a);
  sort(v.begin(), v.end(), util::KeyValueCmp<T, T2>);
  return -1;
#endif
}
};
struct _WeightNodeEdge{
  NodeIndex to;
  IndexType id;
  int truss;
  _WeightNodeEdge(){}
  _WeightNodeEdge(int t_, int i_, int tr_): to(t_), id(i_), truss(tr_){}
  bool operator<(const _WeightNodeEdge& rhs) const{
    return to < rhs.to;
  }
  static int find(vector<_WeightNodeEdge>& v, NodeIndex i){
    auto it = lower_bound(v.begin(), v.end(), _WeightNodeEdge(i, -1, -1));
    if (it == v.end() || i != it->to)
      return -1;
    return it - v.begin();
  }
};
struct ToTrussPair{
  NodeIndex to;
  int truss;
  ToTrussPair(){}
  ToTrussPair(int to_, int truss_): to(to), truss(truss_){}
  bool operator<(const ToTrussPair& rhs) const{
    return to < rhs.to;
  }
};
struct _WeightEdge{
  /*
     this the structure of old TCP index, which use vector2d<_WeightEdge> to store edges.
     */
  int from, to, truss;
  _WeightEdge(){}
  _WeightEdge(int f_, int t_, int tr_):from(f_), to(t_), truss(tr_){}
  static bool trussDec(const _WeightEdge& a, const _WeightEdge& b){
    return a.truss > b.truss;
  }
};
class AdjGraph{

 public:


  vector<vector<_WeightNodeEdge> > Edges;
  vector<vector<IndexType> > EIDrev;
  vector<pair<IndexType, IndexType> > Edgepos;
  vector<IndexType> EdgeIDHead;
  IndexType insertEdge(NodeIndex x, NodeIndex y, int truss = -1);
  void deleteEdge(NodeIndex x, NodeIndex y);
  //vector<IndexType> gdeIDrev;
#ifndef option_ordered_Edges
  vector<vector<pair<NodeIndex, NodeIndex> > > ordMap;
#endif
  int TrueNbrEdge;
  AdjGraph(){

  }
  IndexType getRevID(IndexType eid ){
    return Edgepos.size() + Edgepos.size() - eid;
  }
  inline IndexType get_eid(NodeIndex x, NodeIndex y){
    /*
       for (int i=_last_insertion_x + 1; i<Edges.size(); i++)
       EdgeIDHead[i] = EdgeIDHead[i - 1] + Edges.size();
       */
  }
  bool hasEdge(NodeIndex x, NodeIndex y){
    return util::getValue(ordMap[x], y) >= 0;
  }
  _WeightNodeEdge& getEdge(IndexType e){
    return Edges[Edgepos[e].first][Edgepos[e].second];
  }
  size_t getNbrEdgeofNode(NodeIndex x){
    return Edges[x].size();
  }
  NodeIndex getEdgeofNode(NodeIndex x, NodeIndex y_id){
    return Edges[x][y_id].to;
  }
};

class DynamicGraph:public AdjGraph{
 protected:
  void BuildTCPGraphInside();
  vector<vector<vector<_WeightEdge> > >  Forest;
  IndexType _nbrEdge;
  vector<int> _in_L, _sup;
  NodeIndex _nbrNode;

 public:
  int getNbrNode() {
    return _nbrNode;
  }
  DynamicGraph(){};
  ~DynamicGraph(){};
  void loadFromTCPIndex(const char * filename, bool option_require_sort = true);
  void loadFromkTrussIndex(const char * filename);
  void updatingEdge();
  pair<int, int> estimateTruss(NodeIndex x, NodeIndex y);
  //void UpdateTrussofInsert(NodeIndex u, NodeIndex v, vector<IndexType>& out);
  void UpdateTrussofInsert2(NodeIndex u, NodeIndex v, vector<IndexType>& out);
  int insertEdgeOnVector(NodeIndex x, NodeIndex y, IndexType eid = IndexType(-1), int truss = -1);
  void TrussUpdate(vector<pair<NodeIndex, NodeIndex>>& new_edges, string fn, int silent_lvl = 1);
  void TrussUpdateDelete(vector<pair<NodeIndex, NodeIndex> >& new_edges, string fn, int silent_lvl = 1);

  //void UpdateTCPofDelete(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl);
  void convertTrussBin2Txt(const char* fn,int offset = 0);
  void BuildTCPGraph(const char* fn);
  void dumpTrussIntoTXT(const char* fn, int offset = 0);
  void dumpSortedEdgeIntoTXT(const char* fn, int offset = 0);
  void UpdateTrussofDelete(NodeIndex u, NodeIndex v, vector<IndexType>& out);

  void BuildTCPofNode(NodeIndex x);
  void TriangleListingXY(NodeIndex _x, NodeIndex _y,  vector<pair<NodeIndex, NodeIndex> >& out);
  void buildForestFromEdges(NodeIndex x, vector<_WeightEdge>& v, NodeIndex max_node_id);
  IndexType findEdgeInForeset(NodeIndex x, NodeIndex y, NodeIndex z, bool isconvert = true);
  void BuildUnionFromForest(vector<vector<_WeightEdge> >& forest_x, UnionSet& out);
  void TCPUpdateofMultiDeleteMain(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl);
  void flushTruss(vector<int>& old_truss, vector<IndexType>& ChangedEdges);
  void unzipBelowLvlK(vector<vector<_WeightEdge> >& forest_x, int K, vector<_WeightEdge>& out);
  void insertEdgesIntoExistingForest(NodeIndex x, vector<_WeightEdge>& tmpEdge);
  void UpdateTrussofDelete2(NodeIndex u, NodeIndex v, vector<IndexType>& out);
  void expandLvlK(NodeIndex x, NodeIndex z_id, int k, vector<pair<NodeIndex, NodeIndex> >& out, vector<int>& truss);
  void insertEdgeIntoForest(NodeIndex x, NodeIndex a, NodeIndex b, int t, bool option_order_of_K_maintain = false);
  void removeEdgefromForest(NodeIndex x, NodeIndex a, NodeIndex b_id_a);
  void expandLvlK_e(NodeIndex x, NodeIndex z_id, int k, vector<pair<int, NodeIndex> >& out);
  void displayForest(NodeIndex x);

  //
  void Check(const char* fn) const;
  //void expandLvlKwithtruss(NodeIndex x, NodeIndex z_id, int k, vector<pair<int, NodeIndex> >& out);
};

class InsertGraph: public DynamicGraph {
 public:
  InsertGraph(){}
  void updateForestOfBatchInsertTwo(
      pair<NodeIndex, pair<IndexType, IndexType>>* begin,
      pair<NodeIndex, pair<IndexType, IndexType>>* end);

  void updateForestOfBatchInsert(pair<NodeIndex, IndexType>* begin,
                                 pair<NodeIndex, IndexType>* end);
  void updateForestOfInsert(pair<NodeIndex, IndexType>* begin,
                            pair<NodeIndex, IndexType>* end);
  void TCPUpdateofMultiInsertMain(vector<pair<NodeIndex, NodeIndex>>& new_edges,
                                  string fn, int silent_lvl);
  void UpdateTCPofInsert(vector<pair<NodeIndex, NodeIndex>>& new_edges,
                         string fn, int silent_lvl);
  void UpdateTCPofInsertOneByOne(vector<IndexType>& all_updated_edges,
                                 vector<int>& old_truss);
  int dfsInForest(vector<vector<_WeightEdge>>& Forest_x, NodeIndex crt,
                  NodeIndex src, NodeIndex dst,
                  pair<NodeIndex, NodeIndex>& edge);
};
class DeleteGraph:public DynamicGraph{

 public:
  void UpdateTCPofDelete(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl);
  void TCPUpdateofMultiDeleteMain(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl);
  void removeNodeFromForestX(NodeIndex x, NodeIndex y);
  void removeEdgeFromForest(NodeIndex x, NodeIndex y, NodeIndex z_id_f_y, int w = 1 , bool not_exist = false );
  bool updateDeleteEdgeFromForest(NodeIndex x, NodeIndex y, NodeIndex z_id_f_y);
  void findBridge(NodeIndex x,
                  vector<pair<NodeIndex, NodeIndex> > & edges_z,
                  vector<pair<NodeIndex, NodeIndex>> & edges_y,
                  NodeIndex& best_zp_id, NodeIndex& best_yp_id, int& max_weight );
  void findBridgePrune(NodeIndex x,
                       vector<pair<int, NodeIndex> > & edges_z,
                       vector<pair<int, NodeIndex>> & edges_y,
                       NodeIndex& best_zp_id, NodeIndex& best_yp_id, int& max_weight, bool case_1 );

  _WeightEdge connectTwoComponent(NodeIndex x, NodeIndex y, NodeIndex z_id_f_y, int w = 1);
  void removeNodeFromForesIntoQuery(NodeIndex x, NodeIndex y, vector<NodeIndex>& out);
  vector<vector<pair<int, NodeIndex> > > ordTruss;
  void resortTruss(vector<IndexType>& ChangedEdges);
  // seem not practical, truss always changed.
  // resort after update for dynamic, move this part out of timer.... :)
};
};

#endif
