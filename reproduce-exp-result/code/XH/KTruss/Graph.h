#pragma once
#define SPEED_MODE
#define INFO_LEVEL_1
#ifndef H_GRAPH
#define H_GRAPH

#include "graphutils.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <map>
#include <cmath>

using namespace std;
using namespace sinz;

typedef long long __int64;

namespace sinz {
void QueryLoader(FileHandler& fd, vector<vector<pair<int,int> > >& Query);
void QueryDumper(FileHandler& fd, vector<vector<pair<int,int> > >& Query);
}
namespace sinz{
struct Edge {
  int from, to, id;
  Edge(int _from, int _to, int _id): from(_from), to(_to), id(_id) {}
  Edge(int _from, int _to): from(_from), to(_to) {}
};
struct NodeEdge {
  int to, id;
  NodeEdge() {}
  NodeEdge(int _to, int _id): to(_to), id(_id) {}
  NodeEdge(int _to): to(_to) {}
  bool operator==(const NodeEdge& rhs) {
    return rhs.to == to;
  }
  bool operator<(const NodeEdge& rhs) {
    return to < rhs.to;
  }
};
namespace graph {
template<>
class Vector2d<sinz::graph::edgeTruss> {
  int _n;

 public:
  vector<vector<edgeTruss>> _v;
  vector<vector<int>> _begin;
  int _maxTrust;

 public:
  Vector2d(){}
  Vector2d(int n):_n(n){}
  void resize(int n){
    _n = n;
    _v.resize(_n);
  }
  STRONG_INLINE void reserve_row(int row, int size){
    _v[row].reserve(size);
  }
  STRONG_INLINE void resize_row(int row, int size){
    _v[row].resize(size);
  }
  void BuildTrustIndexSimple(int maxTrust){
    maxTrust = _maxTrust;
  }
  void buildIndex(){
    cerr << "buildIndex sorting"<< endl;
    _begin.resize(_v.size());
    for (size_t nid = 0; nid < _v.size(); nid++){
      int _v_nid_size = _v[nid].size();
      if (_v[nid].size() == 0)continue;
      int max_k = _v[nid][0].truss;

      _begin[nid].resize(max_k + 1, _v_nid_size);
      auto & _v_nid = _v[nid];
      for_ (j_id, 0,  _v[nid].size()){
        if (_begin[nid][_v_nid[j_id].truss] == _v_nid_size )
          _begin[nid][_v_nid[j_id].truss] = j_id;
      }
      for (int j = 2; j<=max_k; j++){
        if (_begin[nid][j] == _v_nid_size )
          _begin[nid][j] = _begin[nid][j - 1];
      }
    }

  }
  inline int maxTruss(int v0){
    return _v[v0][0].truss;
  }
  STRONG_INLINE int beginInd(int v0, int k){
    return (k>_v[v0][0].truss) ? 0 : _begin[v0][k];
  }
};
};
class Graph{

 public:
  enum data_type{undirected, directed};

  typedef unordered_map< __int64, int> Hash64;
  typedef unordered_map< int, int> Hash;
 public:
  Graph(){}
  virtual int loadFromTXT(FileHandler fd, int offset, data_type t) = 0 ;
};

class kTrussGraph:public Graph{
 public:
  //vector<vector<sinz::NodeEdge > > g_;
  vector<Hash> node_hash_;

  //vector<NodeEdge*> edges_;

  vector<int> trussOfNode, Community;


 public:
  vector<pair<int, int> > _fromto;
  vector<vector<sinz::NodeEdge > > g_;
  vector<int> trussOfEdge;
  int _nbrEdge;
 public:
  ~kTrussGraph(){
    cout<<"~\n";
  }

  STRONG_INLINE int& getTruss(int eid_){
    return trussOfEdge[eid_];
  }
  void loadDataAdaptive(const char* prefix, const char *suffix, int offset){
    string binaryDataPath = string(prefix) + ".bin";
    Timer timer;
    if (FileHandler::testOpen(binaryDataPath.c_str())) {
      loadDataFromBin(FileHandler(binaryDataPath.c_str(), "rb"));
      timer.End();
    }
    else {
      loadFromTXT(FileHandler((string(prefix) + string(suffix)).c_str(), "r"), offset);
      timer.End();
      dumpGraphIntoBinary(FileHandler(binaryDataPath.c_str(), "wb"));
    }

  }
  bool loadDataFromBin(FileHandler fd, bool option_build_hash = true){
    cerr<<"Load graph from binary format\n";
    int n = 0;
    IO::readVectorVector(fd, g_);
    n = g_.size();
    _nbrEdge = 0;
    size_t max_Degree =  0;
    for (int i=0; i<n; i++){
      _nbrEdge += g_[i].size();
      max_Degree = max(max_Degree, g_[i].size());
    }
    _nbrEdge /= 2;
    cerr<<"#node = "<<g_.size()<<" #edge = "<<_nbrEdge<<" max_degree = "<< max_Degree<<endl;
    show_min_du_dv();
    int max_node_id = -1;
    for (int i=0; i<n; i++)
      for (int j=0; j<g_[i].size(); j++)
        if (g_[i][j].to > max_node_id)
          max_node_id = g_[i][j].to;
    output1(max_node_id);

    cout <<"Loaded"<<endl;
    cerr <<"Loaded"<<endl;
#ifndef debug_2013_11_27
    if (option_build_hash){
      node_hash_.resize(n);
      for (int i=0; i<n; i++){
        for (int j=0; j<g_[i].size(); j++){
          node_hash_[i][g_[i][j].to] = g_[i][j].id;
          //fprintf("%d %d", i, g_[i][j].to);
        }
      }
    }
#endif

#ifndef INFO_LEVEL_1
    cout <<"hashed"<<endl;
    cerr <<"hashed"<<endl;
#endif
    return true;
  }
  void show_min_du_dv(){

#define stat_2014_02_11 // stat_2014_02_11
#ifdef stat_2014_02_11
    int n = g_.size();
    long long sum_of_min_du_dv = 0;
    double sum_of_min_du_du_times_log_max_du_dv = 0;
    for (int i = 0; i<n; i++) {
      for (int j = 0; j< g_[i].size(); j++) {
        long long du = g_[i].size();
        long long dv = g_[g_[i][j].to].size();
        sum_of_min_du_dv += min(g_[i].size(), g_[g_[i][j].to].size());
        sum_of_min_du_du_times_log_max_du_dv += min(du, dv) * log(max(du,dv) + .0)/log(2.0);
      }
    }
    cout << "sum_of_min_du_dv = " << sum_of_min_du_dv <<"\n";
    cerr << "sum_of_min_du_dv = " << sum_of_min_du_dv <<"\n";
    cout << "sum_of_min_du_du_times_log_max_du_dv = " << sum_of_min_du_du_times_log_max_du_dv <<"\n";
    cerr << "sum_of_min_du_du_times_log_max_du_dv = " << sum_of_min_du_du_times_log_max_du_dv <<"\n";
#endif
  }
  void dumpGraphIntoBinary(FileHandler fd){
#ifndef INFO_LEVEL_1
    cerr<<"dump graph into binary format";
#endif
    IO::dumpVectorVector(fd, g_);
  }
  int loadFromTXT(FileHandler fd, int offset = 0, data_type t=directed){
    int a,b;
    int edgeID = 0;
    int readEdge = 0;
    map<string, int> id;
    for (;fscanf(fd(), "%d%d", &a, &b) != EOF;){
      //cerr<<"read"<<++readEdge<<" "<<a<<" "<<b<<endl;
      a-=offset;b-=offset;

      while (g_.size()<=a || g_.size()<=b ){
        g_.push_back(vector<sinz::NodeEdge >());
        node_hash_.push_back(Hash());
      }
      if (a==b || node_hash_[a].count(b) + node_hash_[b].count(a)> 0)
      {
        //cerr<<"skip "<<a<<" "<<b<<endl;
        continue;
      }
      g_[a].push_back( NodeEdge(b, edgeID) );
      g_[b].push_back( NodeEdge(a, edgeID) );
      node_hash_[a][b] = edgeID;
      node_hash_[b][a] = edgeID;
      edgeID++;
    }
#ifndef INFO_LEVEL_1
    cerr<<("loaded\n");
    cerr<<("uniqued\n");
#endif
    _nbrEdge = edgeID;
    cerr<<"#node = "<<g_.size()<<" #edge = "<<_nbrEdge<<endl;
    return 0;
  }
  void testAccess(){
    int c=0;
    for (int i=0; i < g_.size(); i++)
      c+=g_[i].size();
    vector<NodeEdge> sg(c);
    int cit = 0;
    for (int i=0; i<g_.size(); i++){
      for (int j=0; j<g_[i].size(); j++){
        sg[cit++] = g_[i][j];
      }
    }
    cout << "sg size "<< sg.size()<<endl;
    Timer timer;
    cout <<"vv"<<endl;
    timer.Start();
    int time_nbr = 100;
    int count = 0;
    for_ (time_, 0, time_nbr){
      count = time_ ;
      for (int i=0; i<g_.size(); i++){
        for (int j=0; j<g_[i].size(); j++){
          count += g_[i][j].id > g_[i][j].to;
        }
      }

    }
    cout <<count<<" ";
    timer.End(time_nbr);
    cout <<"seq"<<endl;
    timer.Start();
    for_ (time_, 0, time_nbr){
      count = time_ ;
      for (int i=0; i<sg.size(); i++){
        count += sg[i].id > sg[i].to;
      }
    }
    cout <<count<<" ";
    timer.End(time_nbr);

    cout <<"seq_it"<<endl;
    timer.Start();
    for_ (time_, 0, time_nbr){
      count = time_ ;
      for_iter_ (it, sg){
        count += it->id > it->to;
      }
    }
    cout <<count<<" ";
    timer.End(time_nbr);
  }
  void build_edges(){
    //Original edge direction is base on node in asc order. Here reorder them by degree of node.
    //edges_.resize(nbrEdge, NULL);
    _fromto.resize(_nbrEdge, make_pair(-1, -1));
    int max_eid  =  - 1;
    for_ (nid, 0, g_.size()){
      for_iter_(eiter, g_[nid]){
        int to(eiter->to), eid(eiter->id);
        if (_fromto[eid].first < 0 || g_[nid].size() < g_[_fromto[eid].first].size()) {
          _fromto[eid] = make_pair(int(nid), to);
        }
        max_eid  =  max(max_eid, eid);
      }
    }
    output2(max_eid, _nbrEdge);
    for (int eid=0; eid<_fromto.size(); eid++){
      if (_fromto[eid].first < 0 || _fromto[eid].second < 0 )
        cerr <<  eid;
    }
  }
  void kTruss(const char* kTrussName = NULL);
  void kTrussNoMap(const char* kTrussName);
  void kTrussNoMap2(const char* kTrussName);
  void kTrussNoMapTruss(const char* kTrussName);
  void dumpWeightedEdge(const char* filename){
#ifndef INFO_LEVEL_1
    cerr << "dumpWeightedEdge  with truss " << endl;
#endif
    FileHandler fd(filename, "wb");
    int m = trussOfEdge.size();
    IO::WriteBlock(fd, &m, sizeof(m));
    output1(m);
    for (int i=0; i<m; i++){
      IO::WriteBlock(fd, &_fromto[i].first, sizeof(_fromto[i].first));
      IO::WriteBlock(fd, &_fromto[i].second, sizeof(_fromto[i].second));
      IO::WriteBlock(fd, &trussOfEdge[i], sizeof(trussOfEdge[i]));
      //IO::WriteBlock(fd, &trussOfEdge[i], sizeof(trussOfEdge[i]));
    }
    fd.close();
    cerr << " Finished" << endl;
  }
};
}
#endif
