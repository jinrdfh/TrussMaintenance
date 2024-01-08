#include <chrono>
#include "global.h"
#include "util.h"
#include "EvolvingGraph.h"

using namespace sinz;
using namespace sinz::graph;
typedef long long __int64;
void crc(string dataname, int offset = 0, string queryid = ""){
  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_tcp_dy2.bin");
  DynamicGraph g;
  g.loadFromkTrussIndex(ktruss_file_name.c_str());
  Communities c;
  g.BuildTCPGraph(ontology_file_name.c_str());
  system("pause");
}
void crc_inc(string dataname, int offset = 0, string queryid =""){
  //to xin: this routine is used to convert truss file from binary format to txt format
  //  which can be compare by shell command
  string dataname_inc = dataname + "_inc";
  //writing file _query_k_max.log

  string inc_ktruss_file_name = (string(DataPrefix(dataname_inc)) + "_truss.bin");
  cerr << "crc_inc "<< inc_ktruss_file_name << endl;
  DynamicGraph g_inc;
  g_inc.convertTrussBin2Txt(inc_ktruss_file_name.c_str(), offset);

  system("pause");
}
void crc_del(string dataname, int offset = 0, string queryid =""){
  string dataname_inc = dataname + "_del";
  //writing file _query_k_max.log

  string inc_ktruss_file_name = (string(DataPrefix(dataname_inc)) + "_truss.bin");
  cerr << "crc_del "<< inc_ktruss_file_name << endl;
  DynamicGraph g_inc;
  g_inc.convertTrussBin2Txt(inc_ktruss_file_name.c_str(), offset);

  system("pause");
}
void truss_update(char *pcIndex,
                  char *pcInc,
                  char *pcSave,
                  int offset = 0,
                  int number_insert = 1000,
                  bool updatetcp =false,
                  bool dy_updatetcp =false) {
  assert(0 == offset && !updatetcp && !dy_updatetcp);
  //writing file _query_k_max.log
  //string prefix = "/data/ykzhang/Data/" + dataname;
//  string prefix = "../data/" + dataname;
//  string ktruss_file_name = prefix + "/truss-10000/" + dataname + "-before.truss";
//  string ontology_file_name = (string(DataPrefix(dataname)) + "_tcp_dy.bin");
//  string dataname_inc = prefix + "/truss-10000/" + dataname + ".inc";

  //string ktruss_file_name = dataname + "graph-before.truss";
  //string ontology_file_name = dataname + "_tcp_dy.bin";
  //string dataname_inc = dataname + "graph.inc";
//  cout << "ktruss file: " << ktruss_file_name << endl;
//  cout << "inc file: " << dataname_inc << endl;
  InsertGraph g;
  if (updatetcp == false) {
    g.loadFromkTrussIndex(pcIndex);
  } else {
    g.loadFromTCPIndex((string(DataPrefix("./")) + "_tcp.bin_aux").c_str());
  }
  Communities c;
  vector<pair<NodeIndex, NodeIndex>> new_edges;
  // if (FileHandler::testOpen(
  //         (string(DataPrefix(dataname_inc)) + ".update").c_str())){
    cerr << "loading new edges"<<endl;
    FileHandler fd(pcInc, "r");
    int n;
    fscanf(fd(), "%d", &n);
    // assert(10000 == n);
    int a, b;
    while (fscanf(fd(), "%d %d\n", &a, &b) !=EOF){
      new_edges.push_back(make_pair(a - offset, b - offset));
    }
    // assert(10000 == n && 10000 == new_edges.size());
    /*
  } else {
    int nbrNode = g.getNbrNode();
    unordered_set<long long> new_edge_map;
    cerr << "Generate new edges, #node" << nbrNode << endl;
    for (int i = 0; i < number_insert; i++) {
      fprintf(stderr, "\r\t\t\t\r%d", i);
      NodeIndex x ,  y ;
      long long hash_xy;
      do {
        x = Sample::rand30() % nbrNode;
        y = Sample::rand30() % nbrNode;
        if (x > y) swap(x, y);
        hash_xy = ((((long long)x)<<32) | y);
      } while ((x == y || g.hasEdge(x, y) || new_edge_map.count(hash_xy) > 0));
      new_edge_map.insert(hash_xy);
      new_edges.push_back(make_pair(x, y));
    }
    FileHandler fd((string(DataPrefix(dataname_inc)) + ".update").c_str(), "w");
    for (size_t i = 0; i < new_edges.size(); i++) {
      fprintf(fd(), "%d %d\n", new_edges[i].first + offset,
                               new_edges[i].second + offset);
    }
  }
  */
  if (!updatetcp) {
    const auto beg = std::chrono::steady_clock::now();
    cout << "inserting..." << endl;

    g.TrussUpdate(new_edges, "", 0);
    //cout << "Save..." << endl;
    //g.dumpTrussIntoTXT(pcSave, offset);
    //cout << "Save done" << endl;
//     cout<<"New edge truss has output to: "<<dataname + "_inc.txt"<<endl;
    // g.dumpSortedEdgeIntoTXT(
    //     (string(DataPrefix(dataname_inc)) + ".txt").c_str(), offset);


    const auto end = std::chrono::steady_clock::now();

//    cout << std::chrono::duration<double, std::milli>(end - beg).count()
//         << " ms" << endl;
  } else {
    assert(false);
    g.BuildTCPGraph((string(DataPrefix("./")) + "_tcp.bin").c_str());
    if (dy_updatetcp) {
      g.TCPUpdateofMultiInsertMain(new_edges,"", 0);
    } else {
      g.UpdateTCPofInsert(new_edges,"", 0);
    }
  }

  /* ====  Check Program ====
*/
//  cout<< prefix + "/truss-10000/"  + dataname + ".truss"<<endl;
//  g.Check((prefix + "/truss-10000/"  + dataname + ".truss").c_str());


  // system("pause");
}

void truss_update_delete(char *pcIndex,
                  char *pcDec,
                  char *pcSave, int offset = 0, int number_delete = 1000, bool updatetcp =false,  bool dy_updatetcp =false ){
  assert(0 == offset);
  assert(true); // set the file paths
  //writing file _query_k_max.log
  /*string prefix = "/data/ykzhang/Data/" + dataname;*/

//  string prefix = "../data/" + dataname;
//  string ktruss_file_name = prefix + "/truss-10000-del/" + dataname + "-before.truss";
//  string ontology_file_name = (string(DataPrefix(dataname)) + "_tcp_dy.bin");
//  string dataname_inc = prefix + "/truss-10000-del/" + dataname + ".inc";
//
//
//  /*string ktruss_file_name = prefix + "/" + dataname + ".truss";
//  string ontology_file_name = (string(DataPrefix(dataname)) + "_tcp_dy.bin");
//  string dataname_inc = prefix + "/truss-10000/" + dataname + ".inc";
//  */
//  cout << "ktruss file: " << ktruss_file_name << endl;
//  cout << "inc file: " << dataname_inc << endl;

  //string ktruss_file_name = dataname + "graph-before.truss";
  //string ontology_file_name = dataname + "_tcp_dy.bin";
  //string dataname_inc = dataname + "graph.del";
//  cout << "ktruss file: " << ktruss_file_name << endl;
//  cout << "inc file: " << dataname_inc << endl;

  DeleteGraph g;
  if (updatetcp == false)
    g.loadFromkTrussIndex(pcIndex);
  else
    g.loadFromTCPIndex((string(DataPrefix("./")) + "_tcp.bin_aux").c_str());
  Communities c;
  //g.BuildTCPGraph(ontology_file_name.c_str());
  //g.TrussUpdate();
  vector<pair<NodeIndex, NodeIndex> > delete_edges;
  // cout << "try to open " << string(DataPrefix(dataname_inc)) + ".delete"<<endl;
  if (FileHandler::testOpen(pcDec)){
//    cout << "loading delete edges from " << dataname_inc << endl;
    FileHandler fd(pcDec, "r");
    int m;
    fscanf(fd(), "%d", &m);
    //assert(10000 == m);
//    cout<< "m=" << m << endl;
    int a, b;
    while (fscanf(fd(), "%d %d\n", &a, &b) !=EOF){
      delete_edges.push_back(make_pair(a - offset,b - offset));
    }
    //assert(10000 == delete_edges.size());
    cout <<"deletion INPUT end" <<endl;
  }
  else {
    assert(false);
    int nbrNode = g.getNbrNode();
    unordered_set<__int64> new_edge_map;
    cerr << "Generate delete edges, #node"<<nbrNode<<endl;
    for (int i = 0; i < number_delete; i++) {
      NodeIndex x ,  y_id, y;
      __int64 hash_xy;
      do {
        x = Sample::rand30() % nbrNode;
        if (g.getNbrEdgeofNode(x) == 0 )
          continue;
        y_id = Sample::rand30() % g.getNbrEdgeofNode(x);
        y = g.getEdgeofNode(x, y_id);
        if (x > y)
          swap(x, y);
        hash_xy =((((__int64)x)<<32)| y);
      } while (new_edge_map.count(hash_xy) > 0);
      new_edge_map.insert(hash_xy);
      delete_edges.push_back(make_pair(x, y));
    }
    /*
    FileHandler fd((string(DataPrefix(dataname_inc)) + ".delete").c_str(), "w");
    for (int i=0; i<delete_edges.size(); i++){
      fprintf(fd(), "%d %d\n", delete_edges[i].first + offset, delete_edges[i].second + offset);
    }
    */
  }
  const auto rmv_beg = std::chrono::steady_clock::now();
  if (!updatetcp){
    g.TrussUpdateDelete(delete_edges,"", 0);

     //g.dumpTrussIntoTXT(pcSave, offset);
    // g.dumpSortedEdgeIntoTXT((string(DataPrefix(dataname_inc)) + ".txt").c_str(), offset);
  }
  else {
    assert(false);

    g.BuildTCPGraph((string(DataPrefix("./")) + "_tcp.bin").c_str());
    if (dy_updatetcp)
      g.TCPUpdateofMultiDeleteMain(delete_edges,"", 0);
    else
      g.UpdateTCPofDelete(delete_edges,"", 0);
  }
  const auto rmv_end = std::chrono::steady_clock::now();
//  cout << std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()
//       << " ms" << endl;
//
//  cout << "testing." << endl;
//  g.Check((prefix + "/truss-10000-del/" + dataname + ".truss").c_str());
//  cout << "test passed." << endl;
//
  // system("pause");
}
// ****************************************************************************
// for testing on the TWITTER dataset
void run() {
  /*string prefix = "/data/ykzhang/Data/facebook/";
  string ktruss_file_name = prefix + "facebook.truss";
  string dataname_inc = prefix + "truss-10000/" + "facebook.inc";
  */
  string prefix = "../data/";
  string ktruss_file_name = prefix + "graph.truss";
  string dataname_inc = prefix + "graph.inc";


  cout << "ktruss file: " << ktruss_file_name << endl;
  cout << "inc file: " << dataname_inc << endl;
  DynamicGraph g;
  g.loadFromkTrussIndex(ktruss_file_name.c_str());
  //
  vector<pair<NodeIndex, NodeIndex>> inc_edges;
  if (FileHandler::testOpen(dataname_inc.c_str())){
    cout << "loading new edges from " << dataname_inc << endl;
    FileHandler fd(dataname_inc.c_str(), "r");
    int m;
    fscanf(fd(), "%d", &m);
    printf("%d\n", m);
    printf("# of inc edges: %d\n", m);
    int a, b;
    while (fscanf(fd(), "%d %d\n", &a, &b) != EOF) {
      inc_edges.push_back(make_pair(a, b));
    }
    assert(m == inc_edges.size());
  }
  const auto rmv_beg = std::chrono::steady_clock::now();
  g.TrussUpdateDelete(inc_edges, "", 0);
  const auto rmv_end = std::chrono::steady_clock::now();
  cout << "removal takes "
       << std::chrono::duration<double, milli>(rmv_end - rmv_beg).count()
       << " ms" << endl;
  const auto ins_beg = std::chrono::steady_clock::now();
  g.TrussUpdate(inc_edges, "", 0);
  const auto ins_end = std::chrono::steady_clock::now();
  cout << "insertion takes "
       << std::chrono::duration<double, milli>(ins_end - ins_beg).count()
       << " ms" << endl;
  cout << "testing." << endl;

  g.Check((prefix + "graph.truss").c_str());
  cout << "test passed." << endl;
}
// ****************************************************************************
int main(int argn, char ** argv){
  string program_id;
  unordered_map<string, int> parameters;

  if (argn < 2)
  {
    program_id = "delete_tcp";
  }
  else{
    program_id = argv[1];
  }

    char *pcIndex = NULL;
    char *pcUpt = NULL;
    char *pcSave = NULL;
//======   这部分走通了
  if (program_id == "-i"){
    int nbrsample = 10, K_sel = 0, number_insert;
    int seed = time(0);
    int offset = 0;
    if (argn > 2) pcIndex = argv[2];
    if (argn > 3) pcUpt = argv[3];
    if (argn > 4) pcSave = argv[4];
    if (argn > 5) offset = atoi(argv[5]);
    if (argn > 6) number_insert = atoi(argv[6]);
    srand(seed);
    truss_update(pcIndex, pcUpt, pcSave, offset, number_insert);
  }
//=====   这部分走通了





#if 0
  if (program_id == "update_tcp"){
    int nbrsample = 10, K_sel = 0, number_insert;
    int seed = time(0);
    string dataname = "sample2";
    int offset = 1;
    if (argn > 2) dataname = argv[2];
    if (argn > 3) offset = atoi(argv[3]);
    if (argn > 4) number_insert = atoi(argv[4]);
    srand(seed);
    truss_update(dataname, offset, number_insert, true);
  }
  if (program_id == "update_tcp_dy"){
    int nbrsample = 10, K_sel = 0, number_insert = 10;
    int seed = time(0);
    string dataname = "sampleUT";
    int offset = 1;
    if (argn > 2) dataname = argv[2];
    if (argn > 3) offset = atoi(argv[3]);
    if (argn > 4) number_insert = atoi(argv[4]);
    srand(seed);
    truss_update(dataname, offset, number_insert, true, true);
  }
#endif
  if (program_id == "-d"){
    int nbrsample = 10, K_sel = 0, number_delete = 10;
    int seed = time(0);
    string dataname = "sampleUT";
    int offset = 0;
    if (argn > 2) pcIndex = argv[2];
    if (argn > 3) pcUpt = argv[3];
    if (argn > 4) pcSave = argv[4];
    if (argn > 5) offset = atoi(argv[5]);
    if (argn > 6) number_delete = atoi(argv[6]);
    srand(seed);
    truss_update_delete(pcIndex, pcUpt, pcSave, offset, number_delete);
  }
#if 0
  if (program_id == "delete_tcp"){
    int nbrsample = 10, K_sel = 0, number_delete = 10;
    int seed = time(0);
    string dataname = "sampleUT";
    int offset = 1;
    if (argn > 2) dataname = argv[2];
    if (argn > 3) offset = atoi(argv[3]);
    if (argn > 4) number_delete = atoi(argv[4]);
    srand(seed);
    truss_update_delete(dataname, offset, number_delete, true);
  }
  if (program_id == "delete_tcp_dy"){
    int nbrsample = 10, K_sel = 0, number_delete = 10;
    int seed = time(0);
    string dataname = "sampleUT";
    int offset = 1;
    if (argn > 2) dataname = argv[2];
    if (argn > 3) offset = atoi(argv[3]);
    if (argn > 4) number_delete = atoi(argv[4]);
    srand(seed);
    truss_update_delete(dataname, offset, number_delete, true, true);
  }
  if (program_id == "crc"){
    //int nbrsample = 10, K_sel = 0;
    //int seed = time(0);
    //srand(seed);
    crc(argv[2], atoi(argv[3]));
    //truss_update(argv[2], atoi(argv[3]), "");
  }
  if (program_id == "crc_inc"){
    //int nbrsample = 10, K_sel = 0;
    //int seed = time(0);
    //srand(seed);
    crc_inc(argv[2], atoi(argv[3]));
    //truss_update(argv[2], atoi(argv[3]), "");
  }
  if (program_id == "crc_del"){
    //int nbrsample = 10, K_sel = 0;
    //int seed = time(0);
    //srand(seed);
    crc_del(argv[2], atoi(argv[3]));
    //truss_update(argv[2], atoi(argv[3]), "");
  }
#endif
  return 0;
}
