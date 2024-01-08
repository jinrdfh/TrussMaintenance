#include "Graph.h"

namespace sinz{

class TriangleCounterNodeListing{
  vector<char> vised;
  int n, n_remain;

 public:
  void operator()(const vector<vector<NodeEdge>>& g_,const vector<Graph::Hash>& nodemap, vector<int> & count, const char* fn = 0){
    if (fn && FileHandler::testOpen(fn)){
      cerr <<"loading sup"<<endl;
      IO::readVector(FileHandler(fn, "rb"), count);
      for (int i = count.size()-20; i < count.size(); i++)
        cerr<< i<<" "<< count[i]<<endl;
      return ;
    }
    Timer info_triangle_listing;
    //FileHandler fout("triangle.txt", "w");
    typedef KeyValue<int, int, true> KViig ;
    int m = count.size(), n = g_.size();
    cerr<<"Listing... "<<endl;
    vised.resize(n, false);
    n_remain = n = sz_(g_);
    priority_queue<KeyValue<int, int , true> > q;
    vector<int> deg(n);
    vector<char> inQueue(n, false);
    for_(nid, 0, g_.size()){
      q.push(KeyValue<int, int, true>(sz_(g_[nid]), nid));
      deg[nid] = int(g_[nid].size());
      inQueue[nid] = true;
    }
    int nbrOfNode = 0, maxSup = -1;
    while (!q.empty()){
      auto top = q.top(); q.pop();
      int nid = top.se;
      if (!inQueue[nid])
        continue;
      if (++nbrOfNode % 1000 ==0)
        fprintf(stderr, "\rnbrOfNode = %d/%d, maxSup = %d", nbrOfNode, g_.size(), maxSup);

      inQueue[nid] = false;
      dbg(nid);
      const vector<NodeEdge>& node = g_[nid];
      const Graph::Hash& edge_nid = nodemap[nid];
      for_ (eind, 0, node.size()){
        int u = node[eind].to;
        if (!inQueue[u]) continue; // the node is already removed from queue i.e. removed from graph
        const Graph::Hash& edge_u = nodemap[u];
        int e_vu = edge_nid.find(u)->second;
        //for_ (eind2, eind + 1, node.size()){
        for_ (eind2, eind + 1, node.size()){
          int w = node[eind2].to;
          if (!inQueue[w]) continue; // the node is already removed from queue i.e. removed from graph
          int e_vw(edge_nid.find(w)->second);
          auto edgeiter = edge_u.find(w);
          int e_uw = (edgeiter == edge_u.end()) ? -1 : edgeiter->second;
          if (e_uw < 0)
            continue;
          count[e_vu]++;
          count[e_vw]++;
          count[e_uw]++;
          maxSup = max(maxSup, max(count[e_uw] , max(count[e_vw], count[e_uw])));
          //fprintf(fout(), "TRI: %d %d %d\n", nid, u, w);  // commented for speeding up when not listing is required.
        }

      }
      //updating degree
      for_iter_(edge, node){
        deg[edge->to]--;
        if (inQueue[edge->to])
          q.push(KeyValue<int, int, true>(deg[edge->to], edge->to));
      }

      //cerr<<"\n";
      }
      global::time_tr_list = info_triangle_listing.getTick();
      global::stdinfo << "Triangle Listing : "<<info_triangle_listing.getTick() << " ms\n";

      cerr << "Triangle Listing Finished\n";
      if (fn){
        IO::dumpVector(FileHandler(fn, "wb"), count);
        cerr << "Triangle Dump Finished\n";
      }

      //system("pause");
    }
    //#define ongoing_2013_11_30_simple_triangle
#ifdef ongoing_2013_11_30_simple_triangle
    void Simple(const char* databin , const char* supbin = 0){
      vector2d<NodeEdge> Edges;
      IO::readVectorVectorIntoVec2D(FileHandler(databin, "wb"), Edges);
      vector<int> count;
      if (supbin && FileHandler::testOpen(supbin)){
        IO::readVector(FileHandler(supbin, "rb"), count);
        return ;
      }
      int nbrEdge  = Edges.size() / 2;
      count.resize(nbrEdge);

      //FileHandler fout("triangle.txt", "w");

      int m = count.size(), n = Edges.size1d();
      cerr<<" Triangle Listing "<<endl;
      vised.resize(n, false);
      n_remain = n = Edges.size1d();

      int nbrOfNode = 0, maxSup = -1;
      for (IndexType x= 0; x< Edges.size1d(); x++){
        if (++nbrOfNode % 10000 ==0)
          fprintf(stderr, "\rnbrOfNode = %d/%d, maxSup = %d", nbrOfNode, Edges.size1d(), maxSup);
        //dbg(nid);

        const Graph::Hash& edge_nid = nodemap[x];
        NodeEdge *begin_x = Edges.beginPt(x);
        for (NodeEdge *y_it = begin_x, *end = Edges.beginPt(x + 1); y_it < end; y_it++ ){
          const int& y = y_it->to;
          //dbg3(y, Edges.size2d(x), Edges.size2d(y));

          if (Edges.size2d(x) > Edges.size2d(y)){
            //dbg(y);
            NodeEdge *begin_y = Edges.beginPt(y);

            for (NodeEdge *z_it = Edges.beginPt(y), *end = Edges.beginPt(y + 1); z_it < end; z_it++ ){
              const int& z = z_it->to;

              auto xz_id_it  = nodemap[x].find(z);
              int xz_id = xz_id_it==nodemap[x].end() ? - 1: xz_id_it->second;

              if (xz_id < 0 )
                continue;
              //dbg(xz_id);
              //int w = min(min(y_it->truss, z_it->truss), Edges[xz_id].truss);

            }
          }
          else {
            //dbg(y);
            //dbg2(Edges.size1d(), y + 1);
            for (NodeEdge *z_it = Edges.beginPt(x), *end = Edges.beginPt(x + 1); z_it < end; z_it++ ){
              const int& z = z_it->to;
              auto yz_id_it  = nodemap[y].find(z);
              int yz_id = yz_id_it==nodemap[y].end() ? - 1: yz_id_it->second;
              //dbg(yz_id);
              if (yz_id < 0 )
                continue;
              //int w = min(min(y_it->truss, z_it->truss), Edges[yz_id].truss);

            }
          }
        }
        //updating degree

        //cerr<<"\n";
      }
      cerr << "Triangle Listing Finished ";
      if (fn && FileHandler::testOpen(fn)){
        IO::dumpVector(FileHandler(fn, "wb"), count);
      }
      cerr << "Triangle Dump Finished ";
      //system("pause");

    }
#endif
  };

  void QueryLoader(FileHandler& fd, vector<vector<pair<int,int> > >& Query){
    int group, n;

    fscanf(fd(), "%d", &group);
    output1(group);
    Query.resize(group);
    for (int i=0; i<group; i++){
      fscanf(fd(), "%d", &n);
      for (int j=0; j<n; j++){
        int nid, k ;
        fscanf(fd(), "%d %d", &nid, &k);
        Query[i].push_back(make_pair(nid, k));
      }
    }
  }
  void QueryDumper(FileHandler& fd, vector<vector<pair<int,int> > >& Query){
    int group = Query.size();
    fprintf(fd(), "%d\n", group);
    for (int i=0; i<group; i++){
      size_t n = Query[i].size();
      fprintf(fd(), "%d\n", n);
      for (int j=0; j<n; j++){
        int nid(Query[i][j].first), k(Query[i][j].second) ;
        fprintf(fd(), "%d %d\n", nid, k);
        //Query[i].push_back(make_pair(nid, k));
      }
    }
  }
}
namespace sinz{

void kTrussGraph::kTruss(const char* kTrussName){
  //cerr <<kTrussName<<endl;
  if (kTrussName){

    if (FileHandler::testOpen(kTrussName))
      return ;
  }
  Timer timer;
  int n = g_.size();
  int m = _nbrEdge;
  build_edges();
  timer.Start();
  vector<int> sup(m);
  trussOfEdge.resize(m, -1);
#define optmized_2013_12_1
#ifndef debug_2013_11_27
  Timer __profiling_tr_listing;
  TriangleCounterNodeListing TriangleListing;
  __profiling_tr_listing.Start();
  TriangleListing(g_, node_hash_, sup, (string(kTrussName) + "_aux").c_str()); //write to $sup$
  __profiling_tr_listing.End("__profiling_tr_listing"); //2014_02_15
  int max_sup = max_v(max_sup, -1, sup);
  sinz::graph::MovableBinVec bin(sup, max_sup + 2);
  //dbg(max_sup);
  cerr<<"kTruss Decomposition...\n";
  Timer __profiling_ktruss;
#define __profiling_20140215
#ifdef __profiling_20140215
  long long sum_of_sup = 0;
  for (int i = 0; i<sup.size(); i++) {
    sum_of_sup += sup[i];
  }
  cerr << "\nSum_of_sup = " << sum_of_sup << "\n";
#endif
  Timer info_ktruss_decomp;
  int nbrEdge_processed = 0;
  for (int _k = 2; _k <= bin.max_bin_pos(_fromto) + 2; _k++ ) {
    cerr<<"\r\t\t\t\t\rK:"<<_k<<" "<<" #E = "<<nbrEdge_processed<<" time:"<<int(timer.getTick());
    for (int e_uv = bin.next(_k - 2); e_uv != bin.END; e_uv = bin.next(_k - 2)){
      //dbg2("t",e_uv);
      int u(_fromto[e_uv].first), v(_fromto[e_uv].second); // ( deg(u) < deg(v) )by construction.
      //dbg3("--",u + 1, v + 1);
#ifndef optmized_2013_12_1
      if (node_hash_[u].find(v) ==node_hash_[u].end())
        continue;
      if (node_hash_[v].find(u) ==node_hash_[v].end())
        continue;
#else
      if (trussOfEdge[e_uv] >= 0)
        continue;
#endif
      //dbg2(u + 1, v + 1);
      auto& g_u = g_[u];
      auto& hash_v = node_hash_[v];
      //dbg2(u + 1, v + 1);
      for_iter_ (e_uw_it, g_u){
        int e_uw(e_uw_it->id), w(e_uw_it->to);

#ifndef optmized_2013_12_1
        if (node_hash_[u].find(w) == node_hash_[u].end())
          continue;
#else
        if (trussOfEdge[e_uw] >= 0)
          continue;
#endif
        auto vw_iter= hash_v.find(w);
        if (vw_iter == hash_v.end())continue;
        int e_vw = vw_iter->second;
        if (trussOfEdge[e_vw] >= 0)
          continue;
        bin.forward(e_uw);
        bin.forward(e_vw);

      }
      //dbg2(e_uv, _k);
      trussOfEdge[e_uv] = _k;
      nbrEdge_processed++;
#ifndef optmized_2013_12_1
      node_hash_[u].erase(v);
      hash_v.erase(u);
#endif

    }
  }
  //	timer.getTick("Triangle and Ktruss", global::ts);
#endif
  __profiling_ktruss.End("__profiling_ktruss");
  timer.End("Triangle and Ktruss");
  global::time_ktruss = info_ktruss_decomp.getTick();
  global::stdinfo << "kTruss Decomposition : "<<info_ktruss_decomp.getTick() << " ms\n";

#ifdef DEBUG_MODE
  //system("pause");
#endif
  cout<<"Finished\n";
  //dumpTrussComp(trussOfEdge);
  //dumpTrussCounter(sup);
  if (kTrussName)
    dumpWeightedEdge(kTrussName);
}
void kTrussGraph::kTrussNoMap(const char* kTrussNameOld){
  string kTrussName = string(kTrussNameOld) +"_nomap";
  //cerr <<kTrussName<<endl;
  if (kTrussName.length() >= 0){

    if (FileHandler::testOpen(kTrussName.c_str()))
      return ;
  }
  Timer timer;
  int n = g_.size();
  int m = _nbrEdge;
  build_edges();
  //<sort>
  for (int i=0; i<g_.size();i++){
    sort(g_[i].begin(), g_[i].end());
  }
  //</sort>
  timer.Start();

  vector<int> sup(m);
  TriangleCounterNodeListing Listing;
  cerr << (string(kTrussName) + "_aux").c_str() << endl;
  Listing(g_, node_hash_, sup, (string(kTrussName) + "_aux").c_str()); //write to $sup$
  trussOfEdge.resize(m, -1);

  int max_sup = max_v(max_sup, -1, sup);
  sinz::graph::MovableBinVec bin(sup, max_sup + 2);
  output1(max_sup);
  cerr<<"kTruss Decomposition\n";
  int nbrEdge_processed = 0;
  for (int _k = 2; _k <= bin.max_bin_pos(_fromto) + 2; _k++ ) {
    cerr<<"\r\t\t\t\t\rK:"<<_k<<" "<<" #E = "<<nbrEdge_processed<<" time:"<<int(timer.getTick());
    for (int e_uv = bin.next(_k - 2); e_uv != bin.END; e_uv = bin.next(_k - 2)){

      int u(_fromto[e_uv].first), v(_fromto[e_uv].second); // ( deg(u) < deg(v) )by construction.
      if (trussOfEdge[e_uv] >= 0)
        continue;
      auto& g_u = g_[u];
      //output2(u + 1, v + 1);
      trussOfEdge[e_uv] = _k;
      for_iter_ (e_uw_it, g_u){
        int e_uw(e_uw_it->id), w(e_uw_it->to);
        if (trussOfEdge[e_uw] >= 0)
          continue;

        auto vw_iter = lower_bound(g_[v].begin(), g_[v].end(), NodeEdge(w, -1));
        if (vw_iter==g_[v].end() || vw_iter->to != w)
          continue;
        int e_vw = vw_iter->id;
        if (trussOfEdge[e_vw] >= 0)
          continue;
        //output3(e_uw, u + 1, w + 1);
        bin.forward(e_uw);
        bin.forward(e_vw);

      }
      //dbg2(e_uv, _k);

      nbrEdge_processed++;

    }
  }
  //	timer.getTick("Triangle and Ktruss", global::ts);

  timer.End("Triangle and Ktruss");

  cout<<"Finished\n";
  //dumpTrussComp(trussOfEdge);
  //dumpTrussCounter(sup);
  if (kTrussName.length()>0)
    dumpWeightedEdge(kTrussName.c_str());
}
void kTrussGraph::kTrussNoMap2(const char* kTrussNameOld){
  string kTrussName = string(kTrussNameOld) +"_nomap2";
  cerr <<kTrussName<<endl;
  if (kTrussName.length() >= 0){

    if (FileHandler::testOpen(kTrussName.c_str()))
      return ;
  }
  Timer timer;
  int n = g_.size();
  int m = _nbrEdge;
  build_edges();
  for (int i=0; i<g_.size();i++){
    sort(g_[i].begin(), g_[i].end());
  }
  timer.Start();

  vector<int> sup(m);


  TriangleCounterNodeListing Listing;
  cerr << (string(kTrussName) + "_aux").c_str() << endl;
  Listing(g_, node_hash_, sup, (string(kTrussName) + "_aux").c_str()); //write to $sup$
  trussOfEdge.resize(m, -1);
  vector<int> count(m);
  copy(sup.begin(), sup.end(), count.begin());
  int max_sup = max_v(max_sup, -1, sup);
  sinz::graph::movableVector2d<unsigned int> bin;
  vector<unsigned int> edgepos(m);

  bin.build(sup.size(), sup.data(), edgepos.data());
  output1(max_sup);
  cerr<<"kTruss Decomposition\n";
  int nbrEdge_processed = 0;
  for (int _k = 2; nbrEdge_processed < sup.size() && !bin.empty(); _k++ ) {
    cerr<<"\r\t\t\t\t\rK:"<<_k<<" "<<" #E = "<<nbrEdge_processed<<" time:"<<int(timer.getTick());
    while (bin.hasNext(_k - 2)){
      unsigned int e_uv = bin.Next(_k - 2);
      int u(_fromto[e_uv].first), v(_fromto[e_uv].second); // ( deg(u) < deg(v) )by construction.
      if (trussOfEdge[e_uv] >= 0)
        continue;
      auto& g_u = g_[u];
      output3(u + 1, v + 1, e_uv);
      trussOfEdge[e_uv] = _k;
      for_iter_ (e_uw_it, g_u){
        unsigned int e_uw(e_uw_it->id), w(e_uw_it->to);
        if (trussOfEdge[e_uw] >= 0)
          continue;

        auto vw_iter = lower_bound(g_[v].begin(), g_[v].end(), NodeEdge(w, -1));
        if (vw_iter==g_[v].end() || vw_iter->to != w)
          continue;
        unsigned int e_vw = vw_iter->id;
        if (trussOfEdge[e_vw] >= 0)
          continue;
        //output3(e_uw, u + 1, w + 1);
        output1(trussOfEdge[e_uw]);
        bin.forward(e_uw);
        bin.forward(e_vw);

      }
      //dbg2(e_uv, _k);

      nbrEdge_processed++;

    }
  }
  //	timer.getTick("Triangle and Ktruss", global::ts);

  timer.End("Triangle and Ktruss");

  cout<<"Finished\n";
  for (int i=0; i<m; i++)
    cerr << _fromto[i].first + 1<< " "<<  _fromto[i].second + 1<< " " << trussOfEdge[i]<<" " << count[i]<<"\n";
  system("pause");
  //dumpTrussComp(trussOfEdge);
  //dumpTrussCounter(sup);
  if (kTrussName.length()>0)
    dumpWeightedEdge(kTrussName.c_str());
}
void kTrussGraph::kTrussNoMapTruss(const char* kTrussNameOld){
  string kTrussName = string(kTrussNameOld);
  cerr <<kTrussName<<endl;
  if (kTrussName.length() >= 0){

    if (FileHandler::testOpen(kTrussName.c_str()))
      return ;
  }
  Timer timer;
  int n = g_.size();
  int m = _nbrEdge;
  build_edges();
  //<sort>
  for (int i=0; i<g_.size();i++){
    sort(g_[i].begin(), g_[i].end());
  }
  //</sort>
  timer.Start();

  vector<int> sup(m);

  Timer __profiling_triangle_listing;
  TriangleCounterNodeListing Listing;
  cerr << (string(kTrussName) + "_aux").c_str() << endl;
  __profiling_triangle_listing.Start();
  Listing(g_, node_hash_, sup, (string(kTrussName) + "_aux").c_str()); //write to $sup$
  __profiling_triangle_listing.End("__profiling_triangle_listing");
  trussOfEdge.resize(m, -1);
  vector<int> count(m);
  copy(sup.begin(), sup.end(), count.begin());
  int max_sup = max_v(max_sup, -1, sup);
  sinz::graph::movableVector2d<unsigned int> bin;
  vector<unsigned int> edgepos(m);

  bin.build(sup.size(), sup.data(), edgepos.data());
  output1(max_sup);
  cerr<<"kTruss Decomposition\n";
  int nbrEdge_processed = 0;
  for (int _k = 2; nbrEdge_processed < sup.size() && !bin.empty(); _k++ ) {
    cerr<<"\r\t\t\t\t\rK:"<<_k<<" "<<" #E = "<<nbrEdge_processed<<" time:"<<int(timer.getTick());
    while (bin.hasNext(_k - 2)){
      unsigned int e_uv = bin.Next(_k - 2);
      int u(_fromto[e_uv].first), v(_fromto[e_uv].second); // ( deg(u) < deg(v) )by construction.
      if (trussOfEdge[e_uv] >= 0)
        continue;
      auto& g_u = g_[u];
      output3(u + 1, v + 1, e_uv);
      trussOfEdge[e_uv] = _k;
      for_iter_ (e_uw_it, g_u){
        unsigned int e_uw(e_uw_it->id), w(e_uw_it->to);
        if (trussOfEdge[e_uw] >= 0)
          continue;

        auto vw_iter = lower_bound(g_[v].begin(), g_[v].end(), NodeEdge(w, -1));
        if (vw_iter==g_[v].end() || vw_iter->to != w)
          continue;
        unsigned int e_vw = vw_iter->id;
        if (trussOfEdge[e_vw] >= 0)
          continue;
        //output3(e_uw, u + 1, w + 1);
        output1(trussOfEdge[e_uw]);
        bin.forward(e_uw);
        bin.forward(e_vw);

      }
      //dbg2(e_uv, _k);

      nbrEdge_processed++;

    }
  }
  //	timer.getTick("Triangle and Ktruss", global::ts);

  timer.End("Triangle and Ktruss");

  cout<<"Finished\n";
  for (int i=0; i<m; i++)
    cerr << _fromto[i].first + 1<< " "<<  _fromto[i].second + 1<< " " << trussOfEdge[i]<<" " << count[i]<<"\n";


  system("pause");

  //dumpTrussComp(trussOfEdge);
  //dumpTrussCounter(sup);
  if (kTrussName.length()>0)
    dumpWeightedEdge(kTrussName.c_str());
}
}
