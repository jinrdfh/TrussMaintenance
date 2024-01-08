//#include "global.h"
#include "util.h"
#include "QueryGraph.h"

enum OFFSET{
  ZERO,ONE
};

using namespace sinz;
using namespace sinz::graph;
void load_map(const char* file, vector<string>& name){
  FileHandler fd(file, "r");
  assert (fd()==NULL);
  char __name[255];
  int i;
  while (fscanf(fd(), "%d ", &i) !=EOF){
    fgets(__name, 255, fd());
    if (i >= name.size())
      name.resize(i + 1 );
    __name[strlen(__name) - 1] = '\0';
    name[i] = __name;
    //cout<<name[i]<<endl;
  }
}

void run(string dataname = "sample3", int offset = ZERO){

  //string dataname = "sample3"; int offset = OFFSET::ONE;
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_ontology.bin");
  freopen((string(DataPrefix(dataname)) + ".log2").c_str(), "w", stdout);
  int flag = 1;
  QueryGraph g;
  if (flag == 1)
  {

    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
    g.getTrussGraph(ktruss_file_name.c_str());
  }
  Communities c;
  Communities c2;
  g.BuildOntologyGraph(ontology_file_name.c_str());

  auto TestQuery = [&g, &c](int node_id, int k) {
    auto& _g = g;
    g.OIQuery(node_id ,k, c);
    c.DisplayCommunityAndSum([&](const int& eid){
                             return make_pair(_g.getEdge(eid).first, _g.getEdge(eid).second);
                             },[&](const int& nid){
                             return nid + 1;
                             });
  };
  auto TestQuery2 = [&g, &c2](int node_id, int k){
    auto& _g = g;
    g.SimpleQuery(node_id ,k, c2);
    c2.DisplayCommunityAndSum([&](const int& eid){
                              return make_pair(_g.getEdge(eid).first, _g.getEdge(eid).second);
                              },[&](const int& nid){
                              return nid + 1;
                              });
  };
  freopen((string(DataPrefix(dataname)) + "_query_index.txt").c_str(), "w", stdout);
  for (int node_id = 0 ; node_id <= 10; node_id ++){
    for (int k = 2; k < 10; k+=2) {
      g.OIQuery(node_id, k, c);
      cout << node_id << " " << k << endl;
      c.DisplayCommunityAndOverlapping([&](const int& eid){
                                       return make_pair(g.getEdge(eid).first, g.getEdge(eid).second);
                                       },[&](const int& nid){
                                       return nid + 1;
                                       });
    }
  }


  /*
     TestQuery(0 ,3);
     TestQuery(0 ,4);
     TestQuery(0 ,5);
     TestQuery(4, 3);
     TestQuery(4, 4);
     TestQuery(2, 4);
     */
  system("pause");
}
void correctness(string dataname = "sample3", int offset = ZERO, string queryid=""){
  //string dataname = "sample3"; int offset = OFFSET::ONE;
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");

  string query_file_name = (string(DataPrefix(dataname)) + queryid + ".query");

  int flag = 1;
  QueryGraph g;
  if (flag == 1)
  {

    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);

    tg.kTruss(ktruss_file_name.c_str());
    g.getTrussGraph(ktruss_file_name.c_str());
  }


  auto& adjV = g.edgeTrusses._v;
  vector<vector<pair<int,int> >> Query;
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);
  output1(Query.size());
  Communities c;
  int community_size = 0;
  freopen((string(DataPrefix(dataname)) + queryid + "_query_sp.txt").c_str(), "w", stdout);
  for (int patitionID = 0; patitionID <Query.size(); patitionID++){
    output1(Query[patitionID].size());
    Timer timer;
    //cout << "Sizeof parition: "<<end - begin <<" ["<<V_Size[begin].second << ", " <<V_Size[end - 1].second<<"]"<<endl;
    community_size = 0;
    for_iter_ ( query, Query[patitionID]){
      if ((query - Query[patitionID].begin()) % 10 ==0)
        cerr<<"#";
      int node_id(query->first), k(query->second);
      cout << node_id << " " << k << endl;
      g.SimpleQuery(node_id, k, c);
      c.DisplayCommunityAndOverlapping([&](const int& eid){
                                       return make_pair(g.getEdge(eid).first, g.getEdge(eid).second);
                                       },[&](const int& nid){
                                       return nid + 1;
                                       });
      //community_size += c.size();
    }
    // cout << "community_size = "<< community_size/Query[patitionID].size()<<endl;
    cerr<<"\n";
    //timer.End();
  }


}
void buildIndex(string dataname = "sample3", int offset = ZERO){
  assert(false);//2014_2_15, this function can only used when triangle listing is finished
  // and the _aux file exsits, which contains the triangles count


  //string dataname = "sample3"; int offset = OFFSET::ONE;
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");



  int flag = 1;
  QueryGraph g;
  if (flag == 1)
  {

    kTrussGraph tg;
    tg.loadDataFromBin(FileHandler((string(DataPrefix(dataname)) + ".bin").c_str(), "rb"), false);
    tg.kTrussNoMapTruss(ktruss_file_name.c_str());
    //g.getTrussGraph(ktruss_file_name.c_str());
  }

  //g.BuildOntologyGraph(ontology_file_name.c_str());


}
void QueryTime(const char* dataname = "WikiTalk", int K_ = 5, string query_id = "", int timeout = 40 * 1){
  //writing file _query_k_max.log
  int offset = OFFSET::ZERO;
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_ontology.bin");
  string query_file_name = (string(DataPrefix(dataname)) + query_id + ".query");

  graph::QueryGraph g;
  if (!FileHandler::testOpen(ktruss_file_name.c_str())){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str());


  Communities c;
  //g.BuildOntologyGraph(ontology_file_name.c_str());

  vector<vector<pair<int,int> >> Query;
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);
  output1(Query.size());
  freopen((string(DataPrefix(dataname)) + "_sp" + query_id + ".querytime").c_str(), "w", stdout);
  for (int patitionID = 0; patitionID <Query.size(); patitionID++){
    output1(Query[patitionID].size());
    Timer timer;
    //cout << "Sizeof parition: "<<end - begin <<" ["<<V_Size[begin].second << ", " <<V_Size[end - 1].second<<"]"<<endl;
    int community_size = 0;
    int timeoutFlag = -1;
    for_iter_ ( query, Query[patitionID]){
      fprintf(stderr, "\r\t\t\t\t\r%d %lf", query-Query[patitionID].begin(), timer.getTick());
      fflush(stderr);
      int node_id(query->first), k(query->second);
      cout << node_id << ":" << k << " ";
      g.SimpleQuery(node_id, k, c);
      community_size += c.size();
      if (timer.getTick() >= timeout * 1000 * 60)
      {
        timeoutFlag = query - Query[patitionID].begin();
        break;
      }
    }
    cout<<"\n";
    cout << "community_size = "<< floor((community_size +0.0 )/Query[patitionID].size())<<endl;
    cerr<<"\n";
    if (timeoutFlag >=0)
      cout << "size: "<<Query[patitionID].size()<<" time: "<< (timer.getTick() * Query[patitionID].size())/timeoutFlag<< " Timeout at query #"<<timeoutFlag<<" realtime:"<<timer.getTick() <<"\n";
    else
      cout << "size: "<<Query[patitionID].size()<<" time: "<< timer.getTick()<< "\n";
  }
  //system("pause");
}
void QueryEach(const char* dataname = "WikiTalk", int offset = 0, string query_id ="", int timeout = 40 * 1){
  //this function is added during sigmod14 resubmit
  cout << "QueryEach " << endl;
#define DataPrefixCurPath(x) (string(x) + "/" + string(x)).c_str()
  //writing file _query_k_max.log
  FileHandler single_query_time_file((string(DataPrefixCurPath(dataname)) + query_id + "_sp_stime.txt").c_str(),"w");
  FileHandler group_query_time_file((string(DataPrefixCurPath(dataname)) + query_id + "_sp_gtime.txt").c_str(),"w");
  string ktruss_file_name = (string(DataPrefixCurPath(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefixCurPath(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefixCurPath(dataname)) + query_id + ".query");

  QueryGraph g;
  if (!FileHandler::testOpen(ktruss_file_name.c_str())){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str());
  Communities c;
  //	g.BuildTCPGraph(ontology_file_name.c_str());
  vector<vector<pair<int,int> >> Query;
  if (FileHandler::testOpen(query_file_name.c_str()) == false) {
    cout << "Cannot open file" << query_file_name.c_str() << endl;
    return ;
  }
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);

  //freopen((string(DataPrefix(dataname)) + "_tcp" + query_id + ".querytime").c_str(), "w", stdout);
  output1(Query.size());
  size_t community_size = 0;
  int nbr_total_querys = 0;
  for (int patitionID=0; patitionID < Query.size(); patitionID++)
    nbr_total_querys += Query[patitionID].size();
  fprintf(single_query_time_file(), "%d\n", nbr_total_querys);
  for (int patitionID = 0; patitionID <Query.size(); patitionID++){
    output1(Query[patitionID].size());
    Timer timer;
    Timer group_timer;
    //cout << "Sizeof parition: "<<end - begin <<" ["<<V_Size[begin].second << ", " <<V_Size[end - 1].second<<"]"<<endl;
    int community_size = 0;
    int timeoutFlag = -1;
    for_iter_ ( query, Query[patitionID]){
      if ((query - Query[patitionID].begin()) % 10 ==0)
        cerr<<"#";
      int node_id(query->first), k(query->second);
      timer.Start();
      g.SimpleQuery(node_id, k, c);
      fprintf(single_query_time_file(), "%d %d %.2lf\n", node_id, k , timer.getTick());
      community_size += c.size();
      if (timer.getTick() >= 1000 * 60 * timeout)
      {
        timeoutFlag = query - Query[patitionID].begin();
        break;
      }
    }
    //cout<<"\n";
    //cout << "community_size = "<< floor((community_size + 0.0)/Query[patitionID].size())<<endl;
    cerr<<"\n";
    if (timeoutFlag >=0)
      fprintf(group_query_time_file(), "size: %d, avg time(ms): %.2lf. Timeout at query %d by ellapsed Time %.2lf\n", Query[patitionID].size(), (group_timer.getTick())/timeoutFlag);
    else
      fprintf(group_query_time_file(), "size: %d, avg time(ms): %.2lf\n", Query[patitionID].size(), group_timer.getTick()/Query[patitionID].size());

  }


  //system("pause");
}

void BuildQuery(const char* dataname = "WikiTalk", int offset = 0){
  //writing file _query_k_max.log
  //global::smartOutput smart_output_handle(("./" + string(dataname) + "/info.txt").c_str(), global::stdinfo);

  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_ontology.bin");
  string query_file_name = (string(DataPrefix(dataname)) + ".query");
  //freopen((string(DataPrefix(dataname)) + "_query.log").c_str(), "w", stdout);
  graph::QueryGraph g;
  if (FileHandler::testOpen(ktruss_file_name.c_str()) == false){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str(), false);
  //srand(0); org simple query does not reset the seed to 0
  Communities c;
  //g.BuildOntologyGraph(ontology_file_name.c_str());

  auto& adjV = g.edgeTrusses._v;
  vector<pair<int, int> > V_Size(adjV.size());
  vector<int> spliter;
  Sample::Stratify(adjV, V_Size, spliter);

  int number_of_sample = 100;
  cout << "number_of_sample = "<< number_of_sample << endl;
  cerr << " number_of_sample = " << number_of_sample << "\n";
  vector<vector<pair<int,int> >> Query(4);
  for (int patitionID = 0; patitionID < spliter.size() - 1; patitionID++){
    int begin = spliter[patitionID], end = spliter[patitionID + 1];
    for (int rr =0 ; rr < number_of_sample; rr++) {
      int nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      while (g.edgeTrusses._v[nid].size() ==0){
        nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      }
      int k = g.edgeTrusses._v[nid][0].truss;
      if (k > 3)
        k = Sample::rand30()% (g.edgeTrusses._v[nid][0].truss + 1 - 4) + 4;
      Query[2 - patitionID].push_back(make_pair(nid, k));
    }

  }
  for (int i=0; i<number_of_sample; i++){
    int nid = (int)(Sample::rand30() % V_Size.size());
    Query[3].push_back(make_pair(nid,g.edgeTrusses._v[nid][0].truss ));
  }
  cerr << " finished generating query "<< "\n";
  sinz::QueryDumper(FileHandler(query_file_name.c_str(), "w"), Query);
  return ;

}
void BuildQueryCurPath(const char* dataname = "WikiTalk", int offset = 0){
  cout << " BuildQueryCurPath "<<endl;
  //writing file _query_k_max.log
  //global::smartOutput smart_output_handle(("./" + string(dataname) + "/info.txt").c_str(), global::stdinfo);

#define DataPrefixCurPath(x) (string(x) + "/" + string(x)).c_str()
  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefixCurPath(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefixCurPath(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefixCurPath(dataname)) + ".query");

  //freopen((string(DataPrefix(dataname)) + "_query.log").c_str(), "w", stdout);
  graph::QueryGraph g;
  if (FileHandler::testOpen(ktruss_file_name.c_str()) == false){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str(), false);
  //srand(0); org simple query does not reset the seed to 0
  Communities c;
  //g.BuildOntologyGraph(ontology_file_name.c_str());

  auto& adjV = g.edgeTrusses._v;
  vector<pair<int, int> > V_Size(adjV.size());
  vector<int> spliter;
  Sample::Stratify(adjV, V_Size, spliter);

  int number_of_sample = 100;
  cout << "number_of_sample = "<< number_of_sample << endl;
  cerr << " number_of_sample = " << number_of_sample << "\n";
  vector<vector<pair<int,int> >> Query(4);
  for (int patitionID = 0; patitionID < spliter.size() - 1; patitionID++){
    int begin = spliter[patitionID], end = spliter[patitionID + 1];
    for (int rr =0 ; rr < number_of_sample; rr++) {
      int nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      while (g.edgeTrusses._v[nid].size() ==0){
        nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      }
      int k = g.edgeTrusses._v[nid][0].truss;
      if (k > 3)
        k = Sample::rand30()% (g.edgeTrusses._v[nid][0].truss + 1 - 4) + 4;
      Query[2 - patitionID].push_back(make_pair(nid, k));
    }

  }
  for (int i=0; i<number_of_sample; i++){
    int nid = (int)(Sample::rand30() % V_Size.size());
    Query[3].push_back(make_pair(nid,g.edgeTrusses._v[nid][0].truss ));
  }
  cerr << " finished generating query "<< "\n";
  sinz::QueryDumper(FileHandler(query_file_name.c_str(), "w"), Query);
  return ;

}
void BuildQueryFixKCurPath(const char* dataname = "WikiTalk", int offset =0, int K_ = 7){
  //writing file _query_k_max.log
#define DataPrefixCurPath(x) (string(x) + "/" + string(x)).c_str()
  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefixCurPath(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefixCurPath(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefixCurPath(dataname)) + "fixk.query");
  //freopen((string(DataPrefix(dataname)) + "_query.log").c_str(), "w", stdout);
  graph::QueryGraph g;
  if (FileHandler::testOpen(ktruss_file_name.c_str()) == false){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str(), false);
  //srand(0); org simple query does not reset the seed to 0
  Communities c;
  //g.BuildOntologyGraph(ontology_file_name.c_str());

  auto& adjV = g.edgeTrusses._v;
  vector<pair<int, int> > V_Size(adjV.size());
  vector<int> spliter;
  int nbrPartitions = 10;
  Sample::Stratify(adjV, V_Size, spliter, nbrPartitions);

  int number_of_sample = 100;
  cout << "number_of_sample = "<< number_of_sample << endl;
  cerr << " number_of_sample = " << number_of_sample << "\n";

  vector<vector<pair<int,int> >> Query(nbrPartitions);
  for (int patitionID = 0; patitionID < spliter.size() - 1; patitionID++){
    int begin = spliter[patitionID], end = spliter[patitionID + 1];
    for (int rr =0 ; rr < number_of_sample; rr++) {
      int nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      while (g.edgeTrusses._v[nid].size() ==0){
        nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      }
      int k = g.edgeTrusses._v[nid][0].truss;
      if (k > 3)
        k = Sample::rand30()% (g.edgeTrusses._v[nid][0].truss + 1 - 4) + 4;
      Query[patitionID].push_back(make_pair(nid, k));
    }
  }
  int nbrQueryBin = 2;
  //vector<vector<vector<pair<int,int> > >> QueryS(nbrQueryBin);
  vector<vector<pair<int,int> > > Query1(Query.size()/nbrQueryBin), Query2(Query.size()/nbrQueryBin);
  //<FixedK>
  int FixK = K_;
  for (int i=0; i<Query.size(); i++){
    for (int j=0; j<Query[i].size(); j++){
      Query[i][j].second = FixK;
      if (i%2==0)
        Query1[i/2].push_back(Query[i][j]);
      else
        Query2[i/2].push_back(Query[i][j]);
    }
  }
  //</FixedK>
  cerr << " finished generating query "<< "\n";

  sinz::QueryDumper(FileHandler((string(DataPrefixCurPath(dataname)) + "fixk1.query").c_str(), "w"), Query1);
  sinz::QueryDumper(FileHandler((string(DataPrefixCurPath(dataname)) + "fixk2.query").c_str(), "w"), Query2);
  return ;

}
void BuildQueryFixK(const char* dataname = "WikiTalk", int offset =0, int K_ = 7){
  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_ontology.bin");
  string query_file_name = (string(DataPrefix(dataname)) + "fixk.query");
  //freopen((string(DataPrefix(dataname)) + "_query.log").c_str(), "w", stdout);
  graph::QueryGraph g;
  if (FileHandler::testOpen(ktruss_file_name.c_str()) == false){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str(), false);
  //srand(0); org simple query does not reset the seed to 0
  Communities c;
  //g.BuildOntologyGraph(ontology_file_name.c_str());

  auto& adjV = g.edgeTrusses._v;
  vector<pair<int, int> > V_Size(adjV.size());
  vector<int> spliter;
  int nbrPartitions = 10;
  Sample::Stratify(adjV, V_Size, spliter, nbrPartitions);

  int number_of_sample = 100;
  cout << "number_of_sample = "<< number_of_sample << endl;
  cerr << " number_of_sample = " << number_of_sample << "\n";

  vector<vector<pair<int,int> >> Query(nbrPartitions);
  for (int patitionID = 0; patitionID < spliter.size() - 1; patitionID++){
    int begin = spliter[patitionID], end = spliter[patitionID + 1];
    for (int rr =0 ; rr < number_of_sample; rr++) {
      int nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      while (g.edgeTrusses._v[nid].size() ==0){
        nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      }
      int k = g.edgeTrusses._v[nid][0].truss;
      if (k > 3)
        k = Sample::rand30()% (g.edgeTrusses._v[nid][0].truss + 1 - 4) + 4;
      Query[patitionID].push_back(make_pair(nid, k));
    }
  }
  int nbrQueryBin = 2;
  //vector<vector<vector<pair<int,int> > >> QueryS(nbrQueryBin);
  vector<vector<pair<int,int> > > Query1(Query.size()/nbrQueryBin), Query2(Query.size()/nbrQueryBin);
  //<FixedK>
  int FixK = K_;
  for (int i=0; i<Query.size(); i++){
    for (int j=0; j<Query[i].size(); j++){
      Query[i][j].second = FixK;
      if (i%2==0)
        Query1[i/2].push_back(Query[i][j]);
      else
        Query2[i/2].push_back(Query[i][j]);
    }
  }
  //</FixedK>
  cerr << " finished generating query "<< "\n";

  sinz::QueryDumper(FileHandler((string(DataPrefix(dataname)) + "fixk1.query").c_str(), "w"), Query1);
  sinz::QueryDumper(FileHandler((string(DataPrefix(dataname)) + "fixk2.query").c_str(), "w"), Query2);
  return ;

}
void BuildQueryVaryK(const char* dataname = "WikiTalk", int offset =0, int number_of_sample = 10, int K_Selector = 0){
  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_ontology.bin");
  string query_file_name = (string(DataPrefix(dataname)) + "varyk.query");
  //freopen((string(DataPrefix(dataname)) + "_query.log").c_str(), "w", stdout);
  graph::QueryGraph g;
  if (FileHandler::testOpen(ktruss_file_name.c_str()) == false){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str(), false);
  //srand(0); org simple query does not reset the seed to 0
  Communities c;
  //g.BuildOntologyGraph(ontology_file_name.c_str());

  auto& adjV = g.edgeTrusses._v;
  vector<pair<int, int> > V_Size(adjV.size());
  vector<int> spliter;
  int nbrPartitions = 10;
  Sample::Stratify(adjV, V_Size, spliter, nbrPartitions);

  number_of_sample = 100;
  cout << "number_of_sample = "<< number_of_sample << endl;
  cerr << " number_of_sample = " << number_of_sample << "\n";
  const int nbrK = 5;
  int tmp_Ks0[nbrK] = {5,7,9,11,13};
  int tmp_Ks1[nbrK] = {4,5,6,7,8};
  int tmp_Ks2[nbrK] = {5,7,10,15,20};
  int tmp_Ks3[3] = {9, 11, 13};

  vector<int> KS[4];
  KS[0] = vector<int>(tmp_Ks0, tmp_Ks0 + nbrK);
  KS[1] = vector<int>(tmp_Ks1, tmp_Ks1 + nbrK);
  KS[2] = vector<int>(tmp_Ks2, tmp_Ks2 + nbrK);
  KS[3] = vector<int>(tmp_Ks3, tmp_Ks3 + 3);

  //<VaryK>

  //</VaryK>
  vector<vector<pair<int,int> >> Query1;
  vector<vector<pair<int,int> >> Query2;
  Query1.resize(KS[K_Selector].size());
  Query2.resize(KS[K_Selector].size());
  for (int patitionID = 0; patitionID < spliter.size() - 1; patitionID++){
    int begin = spliter[patitionID], end = spliter[patitionID + 1];
    for (int rr =0 ; rr < number_of_sample; rr++) {
      int nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      while (g.edgeTrusses._v[nid].size() ==0){
        nid = V_Size[Sample::rand30()%(end - begin ) + begin].first;
      }
      int k = g.edgeTrusses._v[nid][0].truss;
      if (k > 3)
        k = Sample::rand30()% (g.edgeTrusses._v[nid][0].truss + 1 - 4) + 4;

      //<VaryK>
      if ((patitionID < 2 && rr== 33) || (patitionID ==2 && rr== 34))
        break;
      if (patitionID >=3 && ((patitionID < 9 && rr== 14) || (patitionID ==9 && rr== 16)))
        break;
      //<VaryK>
      if (patitionID < 3)
        Query1[0].push_back(make_pair(nid, KS[K_Selector][0]));
      if (patitionID >= 3)
        Query2[0].push_back(make_pair(nid, KS[K_Selector][0]));
      //</VaryK>
    }
  }
  //<VaryK>
  for (int i=1; i<KS[K_Selector].size(); i++){
    for (int j=0; j<Query1[0].size(); j++){
      Query1[i].push_back(make_pair(Query1[0][j].first, KS[K_Selector][i]));
    }
    for (int j=0; j<Query2[0].size(); j++){
      Query2[i].push_back(make_pair(Query2[0][j].first, KS[K_Selector][i]));
    }
  }
  //</VaryK>
  cerr << " finished generating query "<< "\n";
  sinz::QueryDumper(FileHandler((string(DataPrefix(dataname)) + "varyk1.query").c_str(), "w"), Query1);
  sinz::QueryDumper(FileHandler((string(DataPrefix(dataname)) + "varyk2.query").c_str(), "w"), Query2);
  return ;

}
int main(int argn, char ** argv){

  string program_id;
  unordered_map<string, int> parameters;
  //#define opt_crc_time
#define opt_query_time
  //#define opt_build_query

  if (argn < 2)
  {
    program_id = "query";
  }
  else{
    program_id = argv[1];
  }
  cerr <<program_id << endl;
  if (program_id == "crc"){
    if (argn < 3){
      correctness("sample3", 1); return 0;
    }
    else {
      string query_id = "";
      if (argn > 4){
        query_id = argv[4];
      }
      cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<query_id<<endl;
      correctness(argv[2], atoi(argv[3]), query_id);
    }
    return 0;
  }
  if (program_id == "queryeach"){
    if (argn < 3){
      QueryEach("sample3", 1); return 0;
    }
    else {
      string query_id = "";
      if (argn > 4){
        query_id = argv[4];
      }
      cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<query_id<<endl;
      QueryEach(argv[2], atoi(argv[3]), query_id);
    }
    return 0;
  }

  if (program_id == "query"){
    if (argn < 3){
      QueryTime("sample3", 1);
    }
    else {
      string query_id = "";
      if (argn > 4){
        query_id = argv[4];
      }
      cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<query_id<<endl;
      QueryTime(argv[2], atoi(argv[3]), query_id);
    }
  }

  if (program_id == "newbuildqueryfixk"){
    int K = 6;
    int seed = time(0);
    if (argn > 4) K = atoi(argv[4]);
    if (argn > 5) seed = atoi(argv[5]);
    srand(seed);
    cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<K<<" "<<seed<<endl;
    BuildQueryFixKCurPath(argv[2], atoi(argv[3]), K);
  }

  if (program_id == "buildquery_fix_k"){
    int K = 6;
    int seed = time(0);
    if (argn > 4) K = atoi(argv[4]);
    if (argn > 5) seed = atoi(argv[5]);
    srand(seed);
    cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<K<<" "<<seed<<endl;
    BuildQueryFixK(argv[2], atoi(argv[3]), K);
  }

  if (program_id == "buildquery_vary_k"){
    int nbrsample = 10, K_sel = 0;
    int seed = time(0);
    if (argn > 4) nbrsample = atoi(argv[4]);
    if (argn >5) K_sel = atoi(argv[5]);
    if (argn > 6) seed = atoi(argv[6]);
    srand(seed);
    cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<K_sel<<endl;
    BuildQueryVaryK(argv[2], atoi(argv[3]), nbrsample, K_sel);
  }
  if (program_id == "buildIndex"){
    int nbrsample = 10, K_sel = 0;
    if (argn > 4) nbrsample = atoi(argv[4]);
    if (argn >5) K_sel = atoi(argv[5]);
    cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<K_sel<<endl;
    buildIndex(argv[2], atoi(argv[3]));
  }
  return 0;
}
