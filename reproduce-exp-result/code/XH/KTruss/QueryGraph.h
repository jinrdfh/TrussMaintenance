#pragma once
#ifndef H_QUERYGRAPH
#define H_QUERYGRAPH

//#define onging_20131120
//#define debug_2013_11_20

#include "Graph.h"
namespace sinz{
namespace graph{
class EdgeQueue:public sinz::queue<pair<int, int> >{
  int _nbrFirst;
  int qid;
  vector<int> s;
  vector<int> DirectVis;
 public:
  EdgeQueue(){
    qid = 0;
    _nbrFirst = -1;
  }
  void Init(vector<int>& v){
    s.resize(v.size());
    s[0] = 0;
    for (int i=1; i<v.size(); i++)
      s[i] = s[i-1] + v[i - 1];
    _nbrFirst = v.size();
    DirectVis.resize(s[s.size() - 1]);
  }
  void Reset(){
    assert(_nbrFirst > 0);
    qid++;
  }
  inline void push(pair<int, int>&& a){
    int t = s[a.first] + a.second;
    if (DirectVis[t] != qid){
      push_back(a);
      DirectVis[t] = qid;
    }
  }

  inline bool notIn(int x, int y){
    return DirectVis[s[x] + y] != qid;
  }


};
class Communities{
  typedef int flag_type;
#ifndef debug_2013_11_20
  vector<int> _v;
  vector<int> _B;
#else
  SeqVector2D<int> _c;
  vector<flag_type> _Vis;
#endif

  int queryID;
  flag_type lastEnd;
  //vector<pair<int, int> >* pt_fromto;
 public:
  Communities(){
    queryID = 0;
    reset();
  }
#ifndef debug_2013_11_20
  void append(vector<int>& new_v){
    //cerr << "append" <<endl;
    int v_size_org = _v.size();
    _v.resize(_v.size() + new_v.size());
    //cerr << _v.size() <<" " << new_v.size() << endl;
    _B.push_back(_v.size());
    //cerr << _B.size() <<" " << v_size_org << endl;

    move(new_v.begin(), new_v.end(), _v.begin() + v_size_org);
    //cerr << "end append" <<endl;
  }
#else
  inline int notIN(int eid){
    return _Vis[eid] != queryID;
  }
  inline void push_back(int id){
    //dbg(id);
    if (_Vis[id] != queryID){
      _c.push_back(id);
      _Vis[id] = queryID;
    }
  }
#endif
#ifndef debug_2013_11_20
  void resetBitmap(vector<char>& vis_){

    for_iter_ (c_it, _v){
      vis_[(*c_it)] = 0;
    }
  }

  inline void makeEnd(){
    _B.push_back(_v.size());
  }

#else
  void resetBitmap(vector<char>& vis_, int nbrEdgeGlobal_){

    for_iter_ (c_it, _c){
      vis_[(*c_it)] = false;
    }
  }
  inline void makeEnd(){
    _c.makeEnd();
    lastEnd = _c.sizesize();
  }
  inline void makeEndNonEmpty(){
    if (lastEnd == _c.sizesize())
      return ;
    _c.makeEnd();
    lastEnd = _c.sizesize();
  }
#endif
  void reset(int nbrEdge = 0){

#ifndef debug_2013_11_20
    _v.clear();
    _B.clear();
    _B.push_back(0);
#else
    _c.reset();
    if (_Vis.size() != nbrEdge)
      _Vis.resize(nbrEdge, 0);
    queryID++;
    lastEnd = 0;
#endif


  }
#ifndef debug_2013_11_20
  size_t size(){
    return _v.size();
  }

#else
  size_t size(){
    return _c.sizesize();
  }
#endif

#ifndef debug_2013_11_20
  template<class _Pr, class _Pr2>
void DisplayCommunityAndOverlapping(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){


  cout<<"#Communities = "<<_B.size() - 1<<"\n";

  for (int b_i=0; b_i<_B.size() - 1; b_i++){
    set<int> nodeids;
    fprintf(stderr, "[%d %d) ", _B[b_i], _B[b_i + 1]);
    for_ (e_it, _B[b_i], _B[b_i + 1]){
      int e = _v[e_it];

      auto edge = fun_Edge(e);
      //printf("(%d, %d) ", fun_NodeName(edge.first), fun_NodeName(edge.second));
      if (edge.first == rid) continue;
      nodeids.insert(edge.first);
      if (edge.second == rid) continue;
      nodeids.insert(edge.second);
    }
    cout<<"Community "<<b_i + 1<<":";
    int newline_cnt = 0;
    for_iter_(it, nodeids){
      if (newline_cnt++ % 10 ==0)
        cout<<"\n";
      cout<<(it==nodeids.begin() ? " " : ", ");
      cout<<fun_NodeName(*it);

    }
    cout<<"\n";
  }

}
template<class _Pr, class _Pr2>
void DisplayCommunityAndSum(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){
  cout<<"#Communities = "<<_B.size() - 1<<"\n";
  for (int b_i=0; b_i<_B.size() - 1; b_i++){
    set<int> nodeids;
    fprintf(stderr, "[%d %d) ", _B[b_i], _B[b_i + 1]);
    for_ (e_it, _B[b_i], _B[b_i + 1]){
      int e = _v[e_it];

      auto edge = fun_Edge(e);
      //printf("(%d, %d) ", fun_NodeName(edge.first), fun_NodeName(edge.second));
      if (edge.first == rid) continue;
      nodeids.insert(edge.first);
      if (edge.second == rid) continue;
      nodeids.insert(edge.second);
    }
    __int64 s = 0LL;
    for_iter_ (it, nodeids){
      s+=*it;
    }
    cout<<"Community "<<b_i + 1<<":";
    cout << s <<endl;
  }

}
#else
template<class _Pr, class _Pr2>
void DisplayCommunityAndOverlapping(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){
  cout<<"#Communities = "<<_c.size()<<"\n";

  for (int b_i=0; b_i<_c.size(); b_i++){
    set<int> nodeids;
    printf("[%d %d) ", _c.begin(b_i) - _c.begin(0),  _c.begin(b_i + 1) - _c.begin(0));
    for_range_ (e_it, _c.begin(b_i), _c.end(b_i)){
      int e = *e_it;

      auto edge = fun_Edge(e);
      //printf("(%d, %d) ", fun_NodeName(edge.first), fun_NodeName(edge.second));
      if (edge.first == rid) continue;
      nodeids.insert(edge.first);
      if (edge.second == rid) continue;
      nodeids.insert(edge.second);
    }
    cout<<"Community "<<b_i + 1<<":";
    int newline_cnt = 0;
    for_iter_(it, nodeids){
      if (newline_cnt++ % 10 ==0)
        cout<<"\n";
      cout<<(it==nodeids.begin() ? " " : ", ");
      cout<<fun_NodeName(*it);

    }
    cout<<"\n";
  }

}
template<class _Pr, class _Pr2>
void DisplayCommunityAndSum(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){

  cout<<"#Communities = "<<_c.size()<<"\n";
  for (int b_i=0; b_i<_c.size(); b_i++){
    set<int> nodeids;
    //printf("[%d %d) ", _B[b_i], _B[b_i + 1]);
    for_range_ (e_it, _c.begin(b_i), _c.end(b_i)){
      int e = *e_it;

      auto edge = fun_Edge(e);
      //printf("(%d, %d) ", fun_NodeName(edge.first), fun_NodeName(edge.second));
      if (edge.first == rid) continue;
      nodeids.insert(edge.first);
      if (edge.second == rid) continue;
      nodeids.insert(edge.second);
    }
    __int64 s = 0LL;
    for_iter_ (it, nodeids){
      s+=*it;
    }
    cout<<"Community "<<b_i + 1<<":";
    cout << s <<endl;
  }

}

#endif

/*
   template<class _Pr, class _Pr2>
void DisplayCommunityIntoFile(_Pr fun_Edge, _Pr2 fun_NodeName, FileHandler& fd){
dbg(_v.size());
unordered_map<int, int> node2new;
auto getID = [&node2new](const int id) -> int{
if (node2new.find(id) ==node2new.end())
node2new[id] = node2new.size();
return node2new[id];
};
for (int i=0; i < min (int(_v.size()), 1); i++){

for_iter_ (e_it, _v[i]){
auto& edge = fun_Edge(*e_it);
int a(getID(edge.first) + 1), b(getID(edge.second) + 1);
if (a > b)
swap(a,b);
fprintf(fd(), "%d %d\n", a, b);
}

}

}
*/
};
class OntologyGraph{


 public:
  vector<pair<int,int> > H;	//(c, k) c is the Community ID and k is the ktruss of this edge
  vector<vector<pair<int,int> > > Edge;
#ifndef debug_2013_11_14
  vector<vector<int> > C;	//inverted list of H, random access
#endif
  //			vector<char> vised;			//denote whether the edge is visited
  BFSqueue<int> q;

  //unordered_map<int, int> indexOfNode; //not used by  member
  int _n, _id;
#ifdef debug_2013_11_14
  vector<int> C_ID;
  vector<int> C_begin;
#endif
 public:
  void dump(FileHandler& fd){
    IO::dump(fd, _id);
    IO::dumpVector(fd, H);
    IO::dumpVectorVector(fd, Edge);
#ifndef debug_2013_11_14
    IO::dumpVectorVector(fd, C);
#endif
  }
  void read(FileHandler& fd){
    IO::read(fd, _id);
    IO::readVector(fd, H);
    //cerr<<"H";
    IO::readVectorVector(fd, Edge);
    _n = H.size();
    //				vised.resize(_n, false);
#ifndef debug_2013_11_14
    IO::readVectorVector(fd, C);
#else
    BuildCommunity([&](const int& nid){
                   return nid + 1;
                   });
#endif


  }
  OntologyGraph(){}
  OntologyGraph(int n):_n(n){
    H.resize(_n);
    //Edge.resize(_n);
  }
  void setID(int id_){
    _id = id_;
  }
  void resize(int n){
    _n = n;
    H.resize(_n);
    //				vised.resize(_n);
    //Edge.resize(_n);
  }
  STRONG_INLINE void labelNode(int x, int cid, int k){
    H[x] = make_pair(cid, k);
    //#undef debug_2013_11_14
#ifndef debug_2013_11_14
    while (C.size() <= cid ) {
      C.push_back(vector<int>());
    }
    C[cid].push_back(x);
#endif
  }
#ifdef debug_2013_11_14
  template<class _Pr>
void BuildCommunity(_Pr Pred){
  //dbg("BuildCommunity");
  int max_C = 0;
  vector<int> ind(H.size());
  for (int i=0; i<H.size(); i++){
    max_C =  max(H[i].first, max_C );
    ind[i] = i;
  }
  //dbg2(max_C, H.size());
  sort(ind.begin(), ind.end(), [&](const int a, const int b){
       return 	H[a].first< H[b].first;
       });
  C_ID.resize(H.size());
  C_begin.resize(max_C + 2);
  for (int i_id=0; i_id<ind.size(); i_id++){
    int w = ind[i_id];
    C_ID[i_id] = w;
    if ( i_id==0 || H[w].first != H[ind[i_id - 1]].first)
      C_begin[H[w].first] = i_id;
  }
  C_begin[max_C + 1] = C_ID.size();
  for_iter_ (edge, Edge){
    sort(edge->begin(), edge->end(), [](const pair<int, int>& a,  const pair<int, int>& b  ){
         //ordered by truss
         return a.second > b.second;
         });
  }
}
#endif
STRONG_INLINE void addEdge(int f, int t, int k){

  while (Edge.size() <= max(f,t) )
    Edge.push_back(vector<pair<int,int> >());
  Edge[f].push_back(make_pair(t,k));
  Edge[t].push_back(make_pair(f,k));

}
bool SearchSetByUID(int u_id, int k, EdgeQueue& directed_edge_queue, vector<edgeTruss>& gv,
                    vector<char>& vis);
#ifndef debug_2013_11_14
template<class _Pr, class _Pr2>
void Display(_Pr Pred, _Pr2 Pred2){

  cout <<"Edge.size() = "<< Edge.size()<<" ";
  cout <<"C.size() = "<< C.size()<<"\n";
  for_ (c_id, 0, C.size()){
    cout<<Pred2(c_id)<< "[";
    for_iter_ (c, C[c_id]){
      cout<<Pred(*c)<<":"<<H[*c].second<<", ";
    }
    cout<<"]";

    if (Edge.size() > c_id ){
      for_iter_(edge, Edge[c_id]){
        if (c_id < edge->first)
          cout<<"("<<Pred2(c_id)<<"," <<Pred2(edge->first) << "," << edge->second<<") ";
      }
    }

    cout<<endl;
  }
#else
  template<class _Pr, class _Pr2>
void Display(_Pr Pred, _Pr2 Pred2){

  cout <<"Edge.size() = "<< Edge.size()<<" ";
  cout <<"C.size() = "<< C_begin.size() - 1<<"\n";
  for_ (c_id, 0, C_begin.size() - 1){
    cout<<Pred2(c_id)<< "[";
    for_ (node_id, C_begin[c_id], C_begin[c_id+ 1]){
      int node = C_ID[node_id];
      cout<<Pred(node)<<":"<<H[node].second<<", ";
    }
    cout<<"]";

    if (Edge.size() > c_id ){
      for_iter_(edge, Edge[c_id]){
        if (c_id < edge->first)
          cout<<"("<<Pred2(c_id)<<"," <<Pred2(edge->first) << "," << edge->second<<") ";
      }
    }

    cout<<endl;
  }
#endif
}

};

class QueryGraph{
 public:
  struct _WeightEdge{
    int from, to, truss;
    _WeightEdge(){}
    _WeightEdge(int f_, int t_, int tr_):from(f_), to(t_), truss(tr_){}
  };
  Vector2d<graph::edgeTruss> edgeTrusses;
  Pair_Hash<int, int> _edgehash;
  vector<char> _vised;
  vector<OntologyGraph> OIndex;
  vector<_WeightEdge> _edge;
  Pair_Hash<int, int> _indexOfNode;
  vector2d<int> revertEdgeID;
  EdgeQueue edgeQueue;
  int _nbrEdge;
  int _nbrNode;
  int _maxTruss;
 public:


  QueryGraph(){

  }
  QueryGraph(kTrussGraph& kG, const char* prefix, const char *suffix, int offset){
    kG.loadDataAdaptive(prefix, suffix , offset);
    kG.kTruss();
  }
  STRONG_INLINE pair<int, int> getEdge(int eid_){
    return make_pair(_edge[eid_].from, _edge[eid_].to);
  }
  void OIQuery(int v0, int k, Communities & C );
  void getTrussGraph(const char* truss_file_name, bool option_build_index = true){
    cerr << " SpQuery Graph getTrussGraph"<<endl;

    if (!FileHandler::testOpen(truss_file_name)) return ;

    FileHandler fd(truss_file_name, "rb");
    IO::readVector(fd, _edge);
    int m =_edge.size();
    //fd.close();
    int maxTrust(0);
    _nbrNode = 0;
    for (int i=0; i<m; i++){
      _nbrNode = max(_nbrNode, max(_edge[i].from, _edge[i].to));
      maxTrust = max(maxTrust, _edge[i].truss);
      if (i> m - 10)
      {
        fprintf(stderr, "%d %d %d\n", _edge[i].from, _edge[i].to, _edge[i].truss);fflush(stderr);
      }
    }

    _nbrNode += 1;
    _nbrEdge = m;
    fprintf(stderr, "#nbrNode = %d, #nbrEdge = %d, maxTrust = %d\n", _nbrNode, _nbrEdge, maxTrust);
    ContrustSimpleIndex(option_build_index);
  }

  void ContrustSimpleIndex(bool option_build_index = true){

    auto& v = edgeTrusses._v;

    edgeTrusses.resize(_nbrNode);

    _maxTruss = 0;
    for_ (eid, 0, _nbrEdge) {
      v[_edge[eid].from].push_back( edgeTruss(_edge[eid].to, eid, _edge[eid].truss));
      v[_edge[eid].to].push_back( edgeTruss(_edge[eid].from, eid, _edge[eid].truss));
      _maxTruss = max(_maxTruss,  _edge[eid].truss);
    }
    cout <<"Construct Ktruss Index"<<endl;
    Timer timer;
    cout <<"Sorting by (truss, deg[to])"<<endl;
    size_t max_degree = 0;
    for (size_t i =0; i<v.size(); i++){
      max_degree = max(max_degree, v[i].size());
    }
    cout << " max_degree = "<< max_degree << endl;
    for_iter_ (node , v){

      sort(node->begin(), node->end(), [&v](const edgeTruss& a, const edgeTruss& b){
           return (a.truss == b.truss) ? 	(v[a.to].size() == v[b.to].size() ?  a.to < b.to : v[a.to].size() < v[b.to].size()) : (a.truss > b.truss);
           });

    }
    cout <<"build hash"<<endl;
    if (option_build_index){

      _edgehash = Pair_Hash<int, int>(_nbrNode);
      size_t nid = 0;
      for_iter_ (node , v){
        if (nid % 10000==0)
          fprintf(stderr, "\r\t\t\t %d", nid);
        for_ (e_i,0, node->size())
            _edgehash[nid][(*node)[e_i].to] = (*node)[e_i].eid;
        nid++;
      }
      output1(_nbrEdge);
      _vised.resize(_nbrEdge, false);
      fill(_vised.begin(), _vised.end(), false);
    }

    edgeTrusses.buildIndex();
    timer.End("Builded Simple Index");
    //timer.getTick("Build Simple Index", global::ts);


  }
  void getTrussGraph(vector<pair<int, int> >& fromto_, vector<int>& trussOfEdge_, int nbrNode_){
    fprintf(stderr, "noting");
    assert(false);
  }
  void BuildOntologyGraph(const char* fn = 0){

    _indexOfNode = Pair_Hash<int, int>(_nbrNode);
    auto& gr = edgeTrusses._v;

    for (int v = 0; v < gr.size(); v++)
      for_ (e_id, 0 , gr[v].size())
          _indexOfNode[v][gr[v][e_id].to] = e_id;
    auto& gv = edgeTrusses._v;
    revertEdgeID.reserve(_nbrEdge * 2);
    for (int i=0; i<gv.size(); i++){
      for (int j=0; j<gv[i].size(); j++){
        revertEdgeID.push_back(_indexOfNode[gr[i][j].to][i]);
      }

      revertEdgeID.makeEnd();
    }
    cerr << "BuildOntologyGraph"<<endl;
    if (true && fn && FileHandler::testOpen(fn)){

      FileHandler fd(fn, "rb");
      cerr <<"< Loading Ontology Graph>"<<endl;
      OIndex.resize(_nbrNode);
      for (int v = 0; v < gr.size(); v++){
        //cerr<<v<<" : ";
        OIndex[v].read(fd);
        //2013 1121 - 1744/////
      }

    }else {
      cerr << "BuildOntologyGraph_Inside"<< endl;
      BuildOntologyGraph_Inside();
      if (fn){
        FileHandler fd(fn, "wb");
        cout <<"Dumping Ontology"<<endl;
        for (int v = 0; v < gr.size(); v++)
          OIndex[v].dump(fd);
      }
    }
    cerr << "edgeTrusses._v.size() = " << edgeTrusses._v.size()<<endl;
    vector<int> s(edgeTrusses._v.size());
    for (int i=0; i<s.size();i++){
      s[i] = edgeTrusses._v[i].size();
    }

    edgeQueue.Init(s);
    for (int i=0; i<OIndex.size(); i++)
      OIndex[i].q.clear(OIndex[i].C_begin.size() -1);
    //can deallocate indexofEdge here
#ifdef DEBUG_MODE

    OIndex[0].Display([&](const int& nid){
                      return gr[0][nid].to + 1;
                      },[&](const int& eid){
                      return eid + 1;
                      });

    system("pause");
#endif
  }
  void BuildOntologyGraph_Inside(){
    //require _nbrNode
    auto& gr = edgeTrusses._v;

    Timer timer;
    OIndex.resize(_nbrNode);
    Timer T0, T1, T2, T3;
    for (int v = 0; v < gr.size(); v++)
    {
      if (gr[v].size()==0)
        continue;
      OIndex[v].resize(gr[v].size());
      OIndex[v].setID(v);
      //cerr<<v<<" "<<gr[v].size()<<endl;
    }

    //timer.End();//0.41 sec
    ; //ximprove: build for every node thus can not improved.
    int test_size = 0, c_g_size = 0, test_c_size = 0;
    UnionSet T;
    unordered_map<int, int> Cid;
    for_ (v, 0, gr.size()){
      auto& H = OIndex[v];
      auto& indexOfNodeFromV = _indexOfNode[v];
      auto& gr_v = gr[v];

      if (gr_v.size()==0)
        continue;
      T.reset(gr_v.size());
      //for (int v = __debug_start; v < __debug_start + 1; v++){
      dbg2("building", v);
      //cout << v <<" ";
      if (v%10000 == 0)
        cerr<<".";
      //cerr<<v<<"#"<<gr_v.size()<<" ";

      int k_max = gr_v[0].truss;
      int v_end = gr_v.size();

      Cid.clear();

      for (int k = k_max; k >= 2; k--){
        int v_k_begin = edgeTrusses.beginInd(v, k);
        int v_k_end = edgeTrusses.beginInd(v, k -1);

        int v_k_begin_new = v_k_begin;
#define ongoing20131115
#ifdef ongoing20131115
        T0.Wake();
        int u_k_counter = 0;
        for (int u_id = v_k_begin; u_id <v_k_end; u_id++)
          u_k_counter += edgeTrusses.beginInd(gr_v[u_id].to, k -1);

        for (int u_id = v_k_begin; u_id <v_k_end; u_id++){

          int u = gr_v[u_id].to;
          int u_k_end = edgeTrusses.beginInd(u, k -1);

          v_k_begin_new = u_id;
          if (u_k_counter > (v_k_end - u_id) * (v_k_end - u_id) /2)
            break;
          else
            u_k_counter -= u_k_end;
          for (int w_id_from_u = 0 ; w_id_from_u < u_k_end; w_id_from_u++){
            int w = gr[u][w_id_from_u].to;
            int e_vw = _edgehash.getValue(v, w);
            if (e_vw < 0 || _edge[e_vw].truss != k) continue;
            int w_id = indexOfNodeFromV[w];
            T.Union(u_id, w_id);
          }

        }
        T0.Sleep();
#endif
        T1.Wake();
        for_ (u_id, v_k_begin_new , v_k_end){
          for_ (w_id, u_id + 1, v_k_end){
            //require: cannot avoid to use hash
            int e_uw = _edgehash.getValue(gr_v[u_id].to, gr_v[w_id].to);
            if (e_uw < 0 || _edge[e_uw].truss < k) continue;
            T.Union(u_id, w_id);
            //dbg3(k, gr_v[u_id].to + 1, gr_v[w_id].to + 1);
          }

        }
        T1.Sleep();
        //complexity: linear to O(1) amortized
        for_ (u_id, v_k_begin , v_k_end){
          //dbg2(u_id, util::incGet(Cid, T.FindSet(u_id)));
          H.labelNode(u_id, util::incGet(Cid, T.FindSet(u_id)), k);
        }
        int v_k_P1_end = edgeTrusses.beginInd(v, k);  //trust(v, u) >= k + 1
#ifdef ongoing20131115
        T2.Wake();
        for_ (w_id, v_k_begin, v_k_end){ //truss(v, w) =  k
          int w_k_end = edgeTrusses.beginInd(v, k - 1);  //trust(v, u) >=k
          int w = gr[v][w_id].to;
          if (w_k_end > v_k_P1_end) {
            for_ (u_id, 0 , v_k_P1_end){
              int u = gr[v][u_id].to;
              int e_uw = _edgehash.getValue(w, u); //require: _edgehash
              if (e_uw < 0 || _edge[e_uw].truss < k) continue;  //truss(u, w) >=  k

              if (T.FindSet(u_id) != T.FindSet(w_id)) {
                T.Union(u_id, w_id);
                H.addEdge(H.H[u_id].first, H.H[w_id].first, k);
              }
            }
          }
          else {
            for_ (u_id_from_w, 0 , w_k_end){
              int u = gr[w][u_id_from_w].to;
              int e_vw = _edgehash.getValue(v, u); //require: _edgehash
              if (e_vw < 0 || _edge[e_vw].truss <= k) continue;  //truss(v, w) >  k
              int u_id = indexOfNodeFromV[u];
              if (T.FindSet(u_id) != T.FindSet(w_id)) {
                T.Union(u_id, w_id);
                H.addEdge(H.H[u_id].first, H.H[w_id].first, k);
              }
            }

          }
        }
        T2.Sleep();
#endif

        for_ (u_id, 0 , v_k_P1_end){
          int& u = gr_v[u_id].to;
          int u_k_begin = edgeTrusses.beginInd(u, k);
          int u_k_end = edgeTrusses.beginInd(u, k - 1);
#ifndef ongoing20131115
          T2.Wake();

          //requirecheck:2013-11-11

          for_ (w_id, v_k_begin, v_k_end){ //truss(v, w) =  k
            int w = gr[v][w_id].to;
            int e_uw = _edgehash.getValue(u, w); //require: _edgehash
            if (e_uw < 0 || _edge[e_uw].truss < k) continue;  //truss(u, w) >=  k

            if (T.FindSet(u_id) != T.FindSet(w_id)) {
              //dbg2("tt", w_id);
              T.Union(u_id, w_id);
              //#ifndef debug_2013_11_14
              H.addEdge(H.H[u_id].first, H.H[w_id].first, k);
              ///#endif
            }
          }
          T2.Sleep();
#endif
          T3.Wake();
          for_ (w_id_from_u, u_k_begin, u_k_end){ //trust(u, w) = k
            int w = gr[u][w_id_from_u].to;
            //dbg3(k, u + 1, w + 1);

            int e_vw = _edgehash.getValue(v, w); //require: _edgehash
            if (e_vw < 0 || _edge[e_vw].truss <= k) continue;  //trust(v, w) > k
            int w_id = indexOfNodeFromV[w]; //id_from_u is a hashmap that map nodeID to index in adjlist of u
            /*
               int w_id = indexOfNodeFromV[w];
               if (w_id >= edgeTrusses.beginInd(v, k))  // truss(v, w) <= k continue
               continue;
               */
            //improve: this hash can be replace if e_vw - _begin[v] is true
            //    but the below may be the bottleneck instead of this one
            //dbg2(u + 1, w + 1);
            if (T.FindSet(u_id) != T.FindSet(w_id)) {
              T.Union(u_id, w_id);
              //#ifndef debug_2013_11_14
              H.addEdge(H.H[u_id].first, H.H[w_id].first, k);
              //#endif
            }
          }
          T3.Sleep();

        }

      }
#ifdef debug_2013_11_14
      OIndex[v].BuildCommunity([&](const int& nid){
                               return gr[v][nid].to + 1;
                               });
#endif
      for (int q = 0; q<H.Edge.size(); q++)
        test_size += H.Edge[q].size();
      c_g_size += gr_v.size();
      //test_c_size += H.C.size();
      if (v % 10000==0)
      {
        cerr << test_size<< " " << c_g_size<<" " << test_c_size <<endl;
        T0.Sum("t0");
        T1.Sum("t1");
        T2.Sum("t2");
        T3.Sum("t3");
      }
    }

#ifdef DEBUG_MODE

    OIndex[0].Display([&](const int& nid){
                      return gr[0][nid].to + 1;
                      },[&](const int& eid){
                      return eid + 1;
                      });

    system("pause");
#endif
    //			timer.getTick("build ontology graph", global::ts);
    timer.End("build ontology graph");
    fill(_vised.begin(), _vised.end(), false);

    }

    void SimpleQuery(int v0, int k, Communities& C){
      C.reset(_nbrEdge);
      //if ( edgeTrusses._v[v0].size() == 0 || edgeTrusses._v[v0][0].truss<4) //cmt: for btc test allowing k = 3

      if (edgeTrusses._v[v0].size() == 0)
        return ;


      //using trussOfEdge, _fromto
#ifndef SPEED_MODE
      cerr <<"SimpleQuery "<<v0<<" "<<k<<endl;
      cout <<"SimpleQuery "<<v0<<" "<<k<<endl;
#endif
      //Communities C(&_fromto);
      auto& gr = edgeTrusses._v;
      auto& gr_v = gr[v0];
      //dbg(edgeTrusses.begin(v0, k)->truss);
#ifndef debug_2013_11_20
      vector<int> cur_C;
#endif

      for (int e_i = 0; e_i < edgeTrusses.beginInd(v0,k - 1); e_i++){
        int eid(gr_v[e_i].eid);
        if (gr_v[e_i].truss >= k && !_vised[eid]){

          std::queue<int> q;
          q.push(eid);
          _vised[eid] = true ;
#ifndef debug_2013_11_20

          cur_C.clear();
#endif


          while (!q.empty()){
            int e_xy = q.front(); q.pop();
            //int e_xy = q.pop();
#ifndef debug_2013_11_20
            cur_C.push_back(e_xy);
#else
            C.push_back(e_xy);
#endif
            int x(_edge[e_xy].from), y(_edge[e_xy].to);
            //	dbg2(x,y);
            for (int e_xz_i = 0; e_xz_i != edgeTrusses.beginInd(x,k - 1); e_xz_i++ ){
              int& e_xz = gr[x][e_xz_i].eid, &z = gr[x][e_xz_i].to, t = gr[x][e_xz_i].truss;
              if (t < k)
                continue;
              if (z==y || !_edgehash.hasKey(x,z)) continue;
              int e_yz = _edgehash.getValue(y, z);
              if (e_yz < 0 || _edge[e_yz].truss < k) continue;

              if (!_vised[e_xz]){
                q.push(e_xz);
                _vised[e_xz] = true ;
              }
              if (!_vised[e_yz]){
                q.push(e_yz);
                _vised[e_yz] = true ;
              }

            }
          }

#ifndef debug_2013_11_20
          dbg(cur_C.size());
          C.append(cur_C);
          dbg(cur_C.size());
#else
          C.makeEndNonEmpty();
#endif

        }

      }
#ifndef debug_2013_11_20
      C.resetBitmap(_vised);
#else
      C.resetBitmap(_vised, _nbrEdge);
#endif
    }
  };

};
}
#endif
