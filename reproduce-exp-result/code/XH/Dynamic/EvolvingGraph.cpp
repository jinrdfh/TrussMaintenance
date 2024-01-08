#include "EvolvingGraph.h"

#include <algorithm>
#include <chrono>

typedef long long __int64;

namespace sinz{

void DynamicGraph::Check(const char* fn) const {
  std::vector<std::pair<std::pair<int, int>, int>> answer;
  {
    FILE* ans_file = fopen(fn, "r");
    int n, m, l;
    fscanf(ans_file, "%d %d %d", &n, &m, &l);
    assert(0 < n && 0 < m && m <= l);
    int v1, v2, trs;
    for (int e = 0; e < m; ++e) {
      fscanf(ans_file, "%d %d %d", &v1, &v2, &trs);
      if (v1 > v2) std::swap(v1, v2);
      answer.push_back({{v1, v2}, trs + 2});
      assert(0 <= v1 && v1 < n);
      assert(0 <= v2 && v2 < n);
    }
    fclose(ans_file);
  }
  std::sort(answer.begin(), answer.end());
  for (size_t i = 1; i < answer.size(); ++i) {
    assert(answer[i] > answer[i - 1]);
  }
  std::vector<std::pair<std::pair<int, int>, int>> result;
  {
    for (int u = 0; static_cast<size_t>(u) < Edges.size(); ++u) {
      for (const auto info : Edges[u]) {
        if (info.id == IndexType(-1)) continue;
        int v = info.to;
        int trs = info.truss;
        if (v >= u) continue;
        assert(0 <= v && static_cast<size_t>(v) < Edges.size());
        result.push_back({{v, u}, trs});
      }
    }
    std::sort(result.begin(), result.end());
  }
  assert(answer.size() == result.size());
  for (size_t i = 0; i < answer.size(); ++i) {
    if (answer[i] != result[i]) {
      printf("%d %d %d\n", answer[i].first.first, answer[i].first.second, answer[i].second);
      printf("%d %d %d\n", result[i].first.first, result[i].first.second, result[i].second);
    }
  }
  assert(answer == result);
  printf("passed.\n");
}

void DynamicGraph::loadFromkTrussIndex(const char * fn) {
  cout<< "read truss-index"<<endl;
  if (fn && FileHandler::testOpen(fn)) {
//    cout << "reading " << string(fn) << endl;
    // FileHandler fd(fn, "b");
    vector<_WeightEdge> _E;
    FILE* file = fopen(fn, "r");
    assert(NULL != file);
    int n, m, l;
    fscanf(file, "%d %d %d", &n, &m, &l);
//    printf("%d %d %d\n", n, m, l);
    {
      std::vector<IndexType> edge_pos_in_bin(l);
      std::vector<int> k(l);
      _in_L.resize(l);
      _sup.resize(l);
      //getchar();
//      printf("%d\n", k[0] + k[1] + edge_pos_in_bin[0]);
    }
    assert(m <= l && 0 < n && 0 < m);
    for (int i = 0; i < m; ++i) {
      int u, v, w;
      fscanf(file, "%d %d %d", &u, &v, &w);
      _E.push_back({u, v, w + 2});
    }
    fclose(file);
    // IO::readVector(fd, _E);
    NodeIndex max_Node_id = 0;
    for (size_t i = 0; i < _E.size(); i++) {
      max_Node_id = max(max_Node_id, (NodeIndex)max(_E[i].from, _E[i].to));
    }
    _nbrNode = n;
    assert(max_Node_id < n);
    {
      /*
      _in_L.resize(l);
      _sup.resize(l);
      static std::vector<IndexType> tmp(l);
      static std::vector<int> truss(l);
      printf("%zd %zd\n", tmp.size(), truss.size());
      */
    }
    Edges.resize(_nbrNode);
    ordMap.resize(_nbrNode);
    EIDrev.resize(_nbrNode);
    for (size_t eid = 0; eid < _E.size(); eid++) {
      NodeIndex x = _E[eid].from, y = _E[eid].to;
      if (x > y)
        swap(x, y);
      int ts = _E[eid].truss;
      NodeIndex x_end = Edges[x].size();
      NodeIndex y_end = Edges[y].size();
      IndexType nbrEdges= Edgepos.size();
      Edges[x].push_back(_WeightNodeEdge(y, nbrEdges, ts ));
      Edges[y].push_back(_WeightNodeEdge(x, nbrEdges, ts ));
      EIDrev[x].push_back(y_end);
      EIDrev[y].push_back(x_end);
      Edgepos.push_back(make_pair(x, (NodeIndex)x_end));
    }
    for (size_t x = 0; x < Edges.size(); x++) {
      sort(Edges[x].begin(), Edges[x].end(),
           [] (const _WeightNodeEdge& a, const _WeightNodeEdge& b) {
             return a.to < b.to;
           });
      Edges[x].shrink_to_fit();
      ordMap[x].resize(Edges[x].size());
      for (size_t yi = 0; yi < Edges[x].size(); yi++) {
        ordMap[x][yi] = make_pair(Edges[x][yi].to, (NodeIndex)yi);
      }
      sort(ordMap[x].begin(), ordMap[x].end(), util::KeyValueCmp<NodeIndex, NodeIndex>);
    }
    for (size_t x = 0; x < Edges.size(); x++) {
      for (size_t yi = 0; yi < Edges[x].size(); yi++) {
        IndexType eid = Edges[x][yi].id;
        if (x == Edgepos[eid].first) {
          Edgepos[eid].second = yi;
        }
      }
    }
    for (size_t x = 0; x < Edges.size(); x++) {
      for (size_t yi = 0; yi < Edges[x].size(); yi++) {
        NodeIndex y = Edges[x][yi].to;
        if (x > y) continue;
        NodeIndex x_id_y = util::getValue(ordMap[y], (NodeIndex)(x));
        EIDrev[x][yi] = x_id_y;
        EIDrev[y][x_id_y] = yi;
      }
    }
    for (size_t x = 0; x < Edges.size(); x++) {
      EIDrev[x].shrink_to_fit();
    }
  }
}
void DynamicGraph::dumpTrussIntoTXT(const char* fn, int offset){
//  cerr << "dumpTrussIntoTXT " << offset<<endl;
  FileHandler fdtxt(fn, "w");
//  cout<<"Output new edge truss to: "<<fn<<endl;

  for (size_t i=0; i<Edges.size(); i++){
    sort(Edges[i].begin(), Edges[i].end(), [](const _WeightNodeEdge& a, const _WeightNodeEdge& b){
         return a.to < b.to;
         });
    for (size_t j_i=0; j_i<Edges[i].size(); j_i++){
      if (Edges[i][j_i].id == IndexType(-1))
        continue;
      if (i < Edges[i][j_i].to)
        fprintf(fdtxt(), "%d %d %d\n", i +  offset, Edges[i][j_i].to +  offset, Edges[i][j_i].truss );
    }
  }
//  fdtxt.close();
}
void DynamicGraph::dumpSortedEdgeIntoTXT(const char* fn, int offset){
  FileHandler fdtxt(fn, "w");
  for (size_t i=0; i<Edges.size(); i++){
    sort(Edges[i].begin(), Edges[i].end(), [](const _WeightNodeEdge& a, const _WeightNodeEdge& b){
         return a.to < b.to;
         });
    for (size_t j_i=0; j_i<Edges[i].size(); j_i++){
      if (Edges[i][j_i].id == IndexType(-1))
        continue;
      if (i < Edges[i][j_i].to)
        fprintf(fdtxt(), "%d %d\n", i + offset, Edges[i][j_i].to + offset);
    }
  }
  fdtxt.close();
}
void DynamicGraph::convertTrussBin2Txt(const char * fn,  int offset){
  output1("loadFromkTrussIndex");
  if (fn && FileHandler::testOpen(fn)){
    cerr <<fn<<endl;
    FileHandler fd(fn, "rb");
    vector<_WeightEdge> _E;
    IO::readVector(fd, _E);
    output1("read");
    NodeIndex max_Node_id = 0;
    for (size_t i=0; i<_E.size(); i++){
      max_Node_id = max(max_Node_id, (NodeIndex)max( _E[i].from, _E[i].to));
      //output3(_E[i].from, _E[i].to, _E[i].truss);
    }
    _nbrNode = max_Node_id + 1;
    output1(max_Node_id);
    Edges.resize(_nbrNode);
    for (size_t eid=0; eid<_E.size(); eid++)
    {
      NodeIndex x = _E[eid].from, y = _E[eid].to;
      if (x > y)
        swap(x, y);
      int ts = _E[eid].truss;
      Edges[x].push_back(_WeightNodeEdge(y, Edgepos.size(), ts ));
      Edges[y].push_back(_WeightNodeEdge(x, Edgepos.size(), ts ));
    }
    output1("re_size");
    for (size_t x=0; x<Edges.size(); x++){
      if (x % 100000 == 0)
        fprintf(stderr, "\r%d", x);

      sort(Edges[x].begin(), Edges[x].end(),
           [](const _WeightNodeEdge& a, const _WeightNodeEdge& b){
           return a.to < b.to ;
           }
          );

    }
    dumpTrussIntoTXT((string(fn) + "_txt").c_str(), offset);
  }
  output1("loadFromkTrussIndex Finish");
}

void DynamicGraph::BuildTCPGraphInside(){
  Timer timer;
  vector<_WeightEdge> tmpEdge;
  UnionSet T;
  int __profiling_nbrs_size = 0;
  __int64 __profiling_edge_size = 0;
  int __profiling_forest_size = 0;
  Timer __profiling_edge_time;
  Timer __profiling_forest_time;
  output1(Edges.size());
  for (int x = 0; x < Edges.size(); x++) {
    int nbrNeibr = Edges[x].size();
    __profiling_nbrs_size += nbrNeibr;
    tmpEdge.clear();

    __profiling_edge_time.Wake();
    _WeightNodeEdge *begin_x = Edges[x].data(), *end_x = begin_x + Edges[x].size();
    for (_WeightNodeEdge *y_it = begin_x; y_it < end_x; y_it++) {
      NodeIndex _x  = x, _y = y_it->to;
      if (ordMap[_x].size() > ordMap[_y].size())
        swap(_x, _y);
      pair<NodeIndex, NodeIndex> *_y_begin = ordMap[_y].data();
      pair<NodeIndex, NodeIndex> *_y_end = _y_begin + ordMap[_y].size();
      for (auto z_it = ordMap[_x].begin(); z_it < ordMap[_x].end(); z_it++) {
        if (_y_begin == _y_end)
          break;
        auto& e__xz= Edges[_x][z_it->second];
        int z_id_y = util::getValueInc(_y_begin, _y_end, e__xz.to);
        if (z_id_y < 0)
          continue;
        int w = min(min(Edges[_y][z_id_y].truss, e__xz.truss), y_it->truss);
        tmpEdge.push_back(
            _WeightEdge(_y == x ? z_id_y : z_it->second, y_it - begin_x,  w)
        );
      }
    }
    std::sort(tmpEdge.begin(), tmpEdge.end(), [](const _WeightEdge& a, const _WeightEdge& b){
              return a.truss > b.truss;
              });
    __profiling_edge_time.Sleep();
    __profiling_forest_time.Wake();
    T.reset(Edges[x].size() + 1);
    NodeIndex forest_x_max_node_id = 0;
    for (size_t i = 0; i<tmpEdge.size(); i++) {
      forest_x_max_node_id = max(forest_x_max_node_id,
                                 (NodeIndex)max(tmpEdge[i].from, tmpEdge[i].to));
      if (forest_x_max_node_id >= nbrNeibr - 1)
        break;
    }
    Forest[x].resize(forest_x_max_node_id + 1);
    int getEdgeCounter = 0;
    for (size_t i = 0; i<tmpEdge.size(); i++) {
      if (getEdgeCounter == nbrNeibr - 1)
        break;
      int& _x = tmpEdge[i].from, &_y = tmpEdge[i].to;
      if (T.FindSet(_x) != T.FindSet(_y)) {
        T.Union(_x, _y);
        Forest[x][_x].push_back(tmpEdge[i]);
        Forest[x][_y].push_back(_WeightEdge(_y, _x, tmpEdge[i].truss));
        getEdgeCounter++;
      }
    }
    for (size_t _x = 0; _x< Forest[x].size(); _x++)
      sort(Forest[x][_x].begin(), Forest[x][_x].end(), _WeightEdge::trussDec);
    __profiling_forest_time.Sleep();
    __profiling_forest_size += Forest[x].size();
  }
  timer.End("TCP build");
}

IndexType AdjGraph::insertEdge(NodeIndex x, NodeIndex y, int truss) {
  if (x > y)
    swap(x, y);
  IndexType e_xy = Edgepos.size();
  NodeIndex x_end = Edges[x].size();
  NodeIndex y_end = Edges[y].size();
  Edgepos.push_back(make_pair(x, (NodeIndex)x_end));
  util::InsertIntoVector(ordMap[x], make_pair(y, (NodeIndex)x_end));
  util::InsertIntoVector(ordMap[y], make_pair(x, (NodeIndex)y_end));
  Edges[x].push_back(_WeightNodeEdge(y, e_xy, truss ));
  Edges[y].push_back(_WeightNodeEdge(x, e_xy, truss ));
  EIDrev[x].push_back(y_end);
  EIDrev[y].push_back(x_end);
  return e_xy;
}
pair<int, int> DynamicGraph::estimateTruss(NodeIndex x, NodeIndex y) {
  int maxTruss = max(Edges[x].size(), Edges[y].size()) + 2;
//  cout <<"maxT=" <<maxTruss <<endl;
  vector<int> K(maxTruss + 1);
  if (Edges[x].size() > Edges[y].size())
    swap(x, y);
  for (int w_id = 0; w_id < Edges[x].size(); w_id++) {
    if (Edges[x][w_id].id == -1) continue;
    NodeIndex w(Edges[x][w_id].to);
    int w_id_y = util::getValue(ordMap[y], w);
    if (w_id_y < 0)
      continue;
    assert(Edges[y][w_id_y].id != -1);
    int min_truss = min(Edges[y][w_id_y].truss, Edges[x][w_id].truss);
    min_truss = min(min_truss, maxTruss);
    for (int j = 2; j <= min_truss; j++)
      K[j]++;
  }
  int K1 = -1, K2 = -1;
  for (int i = 2; i <= maxTruss; i++) {
    if (K[i] >= i - 2)
      K1 = i;
    if (K[i - 1] >= i - 2)
      K2 = i;
  }
  return make_pair(K1, K2);
}
void AdjGraph::deleteEdge(NodeIndex x, NodeIndex y){
  if (x > y)
    swap(x, y);
  auto e_xy_it = lower_bound(ordMap[x].begin(), ordMap[x].end(),
                             make_pair(y, (NodeIndex)(-1)),
                             util::KeyValueCmp<NodeIndex, NodeIndex>);
  auto e_yx_it = lower_bound(ordMap[y].begin(), ordMap[y].end(),
                             make_pair(x, (NodeIndex)(-1)),
                             util::KeyValueCmp<NodeIndex, NodeIndex>);
  Edges[x][e_xy_it->second].id = IndexType(-1);
  Edges[y][e_yx_it->second].id = IndexType(-1);
  ordMap[x].erase(e_xy_it);
  ordMap[y].erase(e_yx_it);
}
void DynamicGraph::UpdateTrussofDelete2(NodeIndex u, NodeIndex v,
                                        vector<IndexType>& out) {
  int offset = 1;
  out.clear();
  int v_id = util::getValue(ordMap[u], v) ;
  assert(v_id >= 0);
  if (v_id < 0)
    return ;

  int K_max = Edges[u][v_id].truss;
  out.push_back(Edges[u][v_id].id);

//  cout << "kmax= " <<K_max << endl;

  deleteEdge(u, v);
  static int update_id = 0;
  update_id += 2;
  vector<vector<IndexType> > L(K_max + 1);
  static vector<IndexType> edge_pos_in_bin;
  static sinz::graph::movableVector2d<IndexType> bin;
  if (_in_L.size() < Edgepos.size()) {
    _in_L.resize(Edgepos.size(), 0);
    _sup.resize(Edgepos.size(), 0);
    edge_pos_in_bin.resize(Edgepos.size(), (IndexType)-1);
   // cout<< "SIZE: " << Edgepos.size() <<endl;
  }
  else
  {
    _in_L.resize(_in_L.size(), 0);
    _sup.resize(_in_L.size(), 0);
    edge_pos_in_bin.resize(_in_L.size(), (IndexType)-1);
    //cout<< "SIZE: " << _in_L.size() <<endl;
  }




//  cout<< "SIZE: " << Edgepos.size() << " " << _in_L.size() <<endl;
  if (Edges[u].size() >  Edges[v].size())
    swap(u, v);
  {
    for (size_t w_id = 0; w_id < Edges[u].size(); w_id++) {
      auto& e_uw = Edges[u][w_id];
      if (e_uw.id == IndexType(-1))
        continue;
      NodeIndex w(Edges[u][w_id].to);
      int t_wu(Edges[u][w_id].truss);
      int w_id_v = util::getValue(ordMap[v], w);
      if (w_id_v < 0)
        continue;
      IndexType& e_wu = Edges[u][w_id].id;
      IndexType& e_wv = Edges[v][w_id_v].id;
      int t_wv(Edges[v][w_id_v].truss);
      int tmp_k = min(t_wu, t_wv);
      if (tmp_k <= K_max) {
        if (t_wu == tmp_k && _in_L[e_wu] != update_id) {
          L[tmp_k].push_back(e_wu);
          _in_L[e_wu] = update_id;
        }
        if (t_wv == tmp_k && _in_L[e_wv] != update_id) {
          L[tmp_k].push_back(e_wv);
          _in_L[e_wv] = update_id;
        }
      }
    }
  }
  vector<int> sup_q;



  for (int k = 3; k <= K_max; k++) {
    size_t ql = 0;
    sup_q.clear();
    int sup_ql = 0;
    
   // cout << k << ": L[k].size= " << L[k].size() <<endl;

    while (ql < L[k].size()) {
      IndexType e_xy = L[k][ql++];
      NodeIndex x = Edgepos[e_xy].first, y_id = Edgepos[e_xy].second;
      NodeIndex y = Edges[x][y_id].to;
      if (Edges[x].size() > Edges[y].size())
        swap(x, y);
      _sup[e_xy] = 0;
      for (size_t z_id = 0; z_id < Edges[x].size(); z_id++) {
        _WeightNodeEdge& e_xz = Edges[x][z_id];
        if (e_xz.id == IndexType(-1))
          continue;
        if (e_xz.truss < k)
          continue;
        int z_id_y  = util::getValue(ordMap[y], e_xz.to);
        if (z_id_y < 0)
          continue;
        _WeightNodeEdge& e_yz = Edges[y][z_id_y];
        if (e_yz.truss < k)
          continue;
        _sup[e_xy]++;
      }
      if (_sup[e_xy] < k - 2) {
        sup_q.push_back(e_xy);
       // cout<< "e_xy" << e_xy << endl;
      }

      //cout<< "e_xy" << edge_pos_in_bin.size() << endl;

      edge_pos_in_bin[e_xy] = update_id; // here check it the wether sup have been compute

      //cout<< "e_xy" << edge_pos_in_bin[e_xy] << endl;
      int last_sup_ql = sup_ql;
      while (sup_ql < sup_q.size()) {
        IndexType e_xy_id = sup_q[sup_ql++];
        NodeIndex x = Edgepos[e_xy_id].first, y_id = Edgepos[e_xy_id].second;
        NodeIndex y = Edges[x][y_id].to;
        Edges[x][y_id].truss--;
        Edges[y][EIDrev[x][y_id]].truss--;
        out.push_back(e_xy_id);
        edge_pos_in_bin[e_xy_id] = IndexType(-1);

        if (Edges[x].size() > Edges[y].size())
          swap(x, y);
        for (size_t z_id = 0; z_id < Edges[x].size(); z_id++) {
          auto& e_xz = Edges[x][z_id];
          if (e_xz.id == IndexType(-1))
            continue;
          IndexType e_xz_id(Edges[x][z_id].id);
          if (e_xz.truss < k)
            continue;
          int z_id_y  = util::getValue(ordMap[y], e_xz.to);
          if (z_id_y < 0)
            continue;
          auto& e_yz = Edges[y][z_id_y];
          if (e_yz.truss < k)
            continue;
          if ( e_xz.truss == k) {
            if (edge_pos_in_bin[e_xz.id] == update_id) {
              _sup[e_xz.id]--;
              if (_sup[e_xz.id] == k - 3)
                sup_q.push_back(e_xz.id);
            }
            if (_in_L[e_xz.id] != update_id) {
              L[k].push_back(e_xz.id);
              _in_L[e_xz.id] = update_id;
            }
          }
          if ( e_yz.truss == k) {
            if (edge_pos_in_bin[e_yz.id] == update_id) {
              _sup[e_yz.id]--;
              if (_sup[e_yz.id] == k - 3)
                sup_q.push_back(e_yz.id);
            }
            if (_in_L[e_yz.id] != update_id) {
              L[k].push_back(e_yz.id);
              _in_L[e_yz.id] = update_id;
            }
          }
        }
      }
    }
  }
//  cout<< "Deletion Completed." << endl;
}
void DynamicGraph::UpdateTrussofInsert2(NodeIndex u, NodeIndex v,
                                        vector<IndexType>& out) {
  out.clear();
  pair<int, int> Bound = estimateTruss(u, v);
  int K1(Bound.first), K2(Bound.second);
  int K_max = K2 - 1;


//  cout << "kmax= " << K_max <<endl;

  vector<vector<IndexType>> L(K_max + 1);
  IndexType e_uv = insertEdge(u, v, K1);
  static int update_id = 0;
  update_id += 2;
  static vector<IndexType> edge_pos_in_bin;
  static sinz::graph::movableVector2d<IndexType> bin;

  
  if (_in_L.size() < Edgepos.size()) {
    _in_L.resize(Edgepos.size(), 0);
    _sup.resize(Edgepos.size(), 0);
    edge_pos_in_bin.resize(Edgepos.size(), (IndexType)-1);
   // cout<< "SIZE: " << Edgepos.size() <<endl;
  }
  else
  {
    _in_L.resize(_in_L.size(), 0);
    _sup.resize(_in_L.size(), 0);
    edge_pos_in_bin.resize(_in_L.size(), (IndexType)-1);
    //cout<< "SIZE: " << _in_L.size() <<endl;
  }


  if (K1 == K_max) {
    L[K_max].push_back(e_uv);
    _in_L[e_uv] = update_id;
  } else {
    out.push_back(e_uv);
  }
  if (Edges[u].size() >  Edges[v].size())
    swap(u, v);
  {
    for (size_t w_id = 0; w_id < Edges[u].size(); w_id++) {
      if (Edges[u][w_id].id == -1) continue;
      NodeIndex w(Edges[u][w_id].to);
      int t_wu(Edges[u][w_id].truss);
      int w_id_v = util::getValue(ordMap[v], w);
      if (w_id_v < 0)
        continue;
      IndexType& e_wu = Edges[u][w_id].id;
      IndexType& e_wv = Edges[v][w_id_v].id;
      assert(e_wv != -1);
      int t_wv(Edges[v][w_id_v].truss);
      int tmp_k = min(t_wu, t_wv);
      if (tmp_k <= K_max) {
        if (t_wu == tmp_k && _in_L[e_wu] != update_id) {
          L[tmp_k].push_back(e_wu);
          _in_L[e_wu] = update_id;
        }
        if (t_wv == tmp_k && _in_L[e_wv] != update_id) {
          L[tmp_k].push_back(e_wv);
          _in_L[e_wv] = update_id;
        }
      }
    }
  }

//  cout << "kmax= " << K_max <<endl;

  static vector<IndexType> tmpEdge;
  static vector<IndexType> sup_q;
  for (int k = K_max; k >= 2; k--) {
    size_t ql = 0;
//     cout << "L[k].size= " << L[k].size() <<endl;
    while (ql < L[k].size()) {
      IndexType e_xy = L[k][ql++];
      NodeIndex x = Edgepos[e_xy].first, y_id = Edgepos[e_xy].second;
      assert(Edges[x][y_id].id != -1);
      NodeIndex y = Edges[x][y_id].to;
      if (Edges[x].size() >  Edges[y].size())
        swap(x, y);
      _sup[e_xy] = 0;
      tmpEdge.clear();
      for (size_t z_id=0; z_id < Edges[x].size(); z_id++) {
        if (Edges[x][z_id].id == -1) continue;
        _WeightNodeEdge& e_xz = Edges[x][z_id];
        if (e_xz.truss < k)
          continue;
        if (e_xz.truss == k && edge_pos_in_bin[e_xz.id] == update_id + 1)
          continue;
        int z_id_y  = util::getValue(ordMap[y], e_xz.to);
        if (z_id_y < 0)
          continue;
        _WeightNodeEdge& e_yz = Edges[y][z_id_y];
        assert(e_yz.id != -1);
        if (e_yz.truss < k)
          continue;
        if (e_yz.truss == k && edge_pos_in_bin[e_yz.id] == update_id + 1)
          continue;
        _sup[e_xy]++;
        if (e_xz.truss == k && _in_L[e_xz.id] != update_id) {
          tmpEdge.push_back(e_xz.id);
        }
        if (e_yz.truss == k && _in_L[e_yz.id] != update_id) {
          tmpEdge.push_back(e_yz.id);
        }
      }
      size_t sup_ql = 0;
      sup_q.clear();
      edge_pos_in_bin[e_xy] = update_id;
      if (_sup[e_xy] > k - 2) {
        for (auto it = tmpEdge.begin(); it < tmpEdge.end(); it++) {
          if (_in_L[*it] != update_id) {
            L[k].push_back(*it);
            _in_L[*it] = update_id;
          }
        }
      } else {
        sup_q.push_back(e_xy);
      }
      while (sup_ql < sup_q.size()) {
        IndexType e_xy_id = sup_q[sup_ql++];
        edge_pos_in_bin[e_xy_id] = update_id + 1;
        NodeIndex x = Edgepos[e_xy_id].first, y_id = Edgepos[e_xy_id].second;
        NodeIndex y = Edges[x][y_id].to;
        assert(Edges[x][y_id].id != -1);
        if (Edges[x].size() >  Edges[y].size())
          swap(x, y);

        for (size_t z_id=0; z_id < Edges[x].size(); z_id++){
          auto& e_xz = Edges[x][z_id];
          if (e_xz.id == -1) continue;
          IndexType e_xz_id(Edges[x][z_id].id);
          if (e_xz.truss == k && edge_pos_in_bin[e_xz.id] == update_id + 1)
            continue;
          if (e_xz.truss < k)
            continue;
          int z_id_y  = util::getValue(ordMap[y], e_xz.to);
          if (z_id_y < 0)
            continue;
          auto& e_yz = Edges[y][z_id_y];
          assert(e_yz.id != -1);
          if (e_yz.truss == k && edge_pos_in_bin[e_yz.id] == update_id + 1)
            continue;
          if (e_yz.truss < k)
            continue;

          if (e_xz.truss == k) {
            if (edge_pos_in_bin[e_xz.id] == update_id) {
              _sup[e_xz.id]--;
              if (_sup[e_xz.id] == k - 2)
                sup_q.push_back(e_xz.id);
            }
          }
          if ( e_yz.truss == k){
            if (edge_pos_in_bin[e_yz.id] == update_id){
              _sup[e_yz.id]--;
              if (_sup[e_yz.id] == k - 2)
                sup_q.push_back(e_yz.id);
            }
          }
        }
      }
    }
    for (int i = 0; i<L[k].size(); i++) {
      IndexType e_xy = L[k][i];
      if (edge_pos_in_bin[e_xy] == update_id) {
        NodeIndex x = Edgepos[e_xy].first, y_id = Edgepos[e_xy].second;
        NodeIndex y = Edges[x][y_id].to;
        assert(Edges[x][y_id].id != -1);
        Edges[x][y_id].truss++;
        Edges[y][EIDrev[x][y_id]].truss++;


//      cout << "Update Truss= " << Edges[x][y_id].truss <<endl;
        out.push_back(e_xy);
      }
    }
  }

}
void DynamicGraph::TrussUpdate(vector<pair<NodeIndex, NodeIndex>>& new_edges,
                               string fn, int silent_lvl) {
  vector<IndexType> ChangedEdges;
  Timer timer_ktruss, timer_tcp;
  size_t nbrTotalChangeEdge = 0;
  timer_ktruss.Start();
  const auto rmv_beg = std::chrono::steady_clock::now();
  for (auto it = new_edges.begin(); it != new_edges.end(); it++) {
//    cout<< "New Edge: "<<it->first << "," << it->second<<"\n";
    UpdateTrussofInsert2(it->first, it->second, ChangedEdges);
    nbrTotalChangeEdge += ChangedEdges.size();
    if (silent_lvl > 1) {
      for (int i = 0; i < ChangedEdges.size(); i++) {
        IndexType e_xy = ChangedEdges[i];
        NodeIndex x = Edgepos[e_xy].first;
        NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
        NodeIndex t = Edges[x][Edgepos[e_xy].second].truss;
      }
    }
  }
    const auto rmv_end = std::chrono::steady_clock::now();
//  cout << std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()
//       << " ms" << endl;
    cout<<"Total Time: "<< std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()<<"ms, Avg Time: "
        <<std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()/new_edges.size()
        <<"ms, "<< nbrTotalChangeEdge<<" edges have updated the trussness."<<endl;
//  cout<<"Total Time: "<< timer_ktruss.getTick()/1000<<"ms, Avg Time: "<<timer_ktruss.getTick()/1000/new_edges.size()
//      <<"ms, "<< nbrTotalChangeEdge<<" edges have updated truss."<<endl;
}
void DynamicGraph::TrussUpdateDelete(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl){
  int offset = 1;
  vector<IndexType> ChangedEdges;
  Timer timer_ktruss, timer_tcp;
  size_t nbrTotalChangeEdge= 0;
  timer_ktruss.Start();
  const auto rmv_beg = std::chrono::steady_clock::now();
  for (auto it = delete_edges.begin(); it != delete_edges.end(); it++) {
    UpdateTrussofDelete2(it->first, it->second, ChangedEdges);
    nbrTotalChangeEdge += ChangedEdges.size();
    if (silent_lvl > 1){
      for (int i = 0; i < ChangedEdges.size(); i++){
        IndexType e_xy = ChangedEdges[i];
        NodeIndex x = Edgepos[e_xy].first;
        NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
        NodeIndex t = Edges[x][Edgepos[e_xy].second].truss;
      }
    }
  }

    const auto rmv_end = std::chrono::steady_clock::now();
//  cout << std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()
//       << " ms" << endl;
    cout<<"Total Time: "<< std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()<<"ms, Avg Time: "
        <<std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()/delete_edges.size()
        <<"ms, "<< nbrTotalChangeEdge<<" edges have updated the trussness."<<endl;
//  cout<<timer_ktruss.getTick()<<"total changed"<<nbrTotalChangeEdge<<endl;
//  cout<<"total changed edges: "<<nbrTotalChangeEdge+delete_edges.size()<<endl;
}
void DynamicGraph::expandLvlK(NodeIndex x, NodeIndex z_id, int k,
                              vector<pair<NodeIndex, NodeIndex>>& out,
                              vector<int>& truss){
  assert(false);
  auto & Forest_x = Forest[x];
  out.clear();
  size_t ql = 0;
  out.push_back(make_pair(z_id, -1));
  truss.push_back(0);
  static bool flag = false;
  while (ql < out.size()) {
    auto f = out[ql++];
    NodeIndex cur = f.first;
    for (size_t i = 0; i < Forest_x[cur].size(); i++) {
      NodeIndex to = Forest_x[cur][i].to;
      if (to == f.second || Forest_x[cur][i].truss < k)
        continue;
      out.push_back(make_pair(to, cur));
      truss.push_back(Forest_x[cur][i].truss);
    }
  }
}

void DynamicGraph::expandLvlK_e(NodeIndex x, NodeIndex z_id, int k,
                                vector<pair<int, NodeIndex> >& out) {
  auto & Forest_x = Forest[x];
  out.clear();
  size_t ql = 0;
  if (z_id >= Forest[x].size())
    return ;
  out.push_back(make_pair(Edges[x][z_id].truss, z_id));
  static vector<int> pre;
  pre.clear();
  pre.push_back(-1);
  while (ql < out.size()){
    auto f = out[ql++];
    NodeIndex cur = f.second;
    for (size_t i = 0; i < Forest_x[cur].size(); i++){
      NodeIndex to = Forest_x[cur][i].to;
      if (to == pre[ql - 1] || Forest_x[cur][i].truss < k)
        continue;
      out.push_back(make_pair(Edges[x][to].truss, to));
      pre.push_back(cur);
    }
  }
}
void DynamicGraph::removeEdgefromForest(NodeIndex x, NodeIndex a,
                                        NodeIndex b_id_a) {
  Forest[x][a][b_id_a] = Forest[x][a][Forest[x][a].size() - 1];
  Forest[x][a].pop_back();
}
void DynamicGraph::insertEdgeIntoForest(NodeIndex x, NodeIndex a, NodeIndex b,
                                        int t, bool option_order_of_K_maintain) {
  if (max(a, b) >= Forest[x].size())
    Forest[x].resize(max(a, b) + 1);
  Forest[x][a].push_back(_WeightEdge(a, b, t));
  Forest[x][b].push_back(_WeightEdge(b, a, t));
  if (option_order_of_K_maintain) {
    sort(Forest[x][b].begin(), Forest[x][b].end(), _WeightEdge::trussDec);
    sort(Forest[x][a].begin(), Forest[x][a].end(), _WeightEdge::trussDec);
  }
}
};
