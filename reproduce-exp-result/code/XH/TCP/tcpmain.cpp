#include "global.h"
#include "util.h"
#include "QueryTCPGraph.h"
#include "datapreprocess.h"

enum OFFSET{
  ZERO,ONE
};

using namespace sinz;
using namespace sinz::graph;

void correctness(string dataname = "sample3", int offset = ZERO, string queryid = ""){
  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefix(dataname)) + queryid + ".query");

  QueryTCPGraph g;
  if (!FileHandler::testOpen(ktruss_file_name.c_str())){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str());
  Communities c;
  g.BuildTCPGraph(ontology_file_name.c_str());
  vector<vector<pair<int,int> >> Query;
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);
  freopen((string(DataPrefix(dataname)) + queryid + "_query_tcp.txt").c_str(), "w", stdout);
  output1(Query.size());
  int community_size = 0;
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
      g.TCPQuery(node_id, k, c);
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

  system("pause");
}
void answer_query(string dataname = "sample3", int offset = ZERO, string queryid = ""){
#define DataFileCurPath(x) (x).c_str()
#define DataPrefixCurPath(x) (x + "/" + x).c_str()
  //writing file _query_k_max.log
  FileHandler single_query_time_file((string(DataPrefixCurPath(dataname)) + queryid + "_tcp_stime_res.txt").c_str(),"w");

  string ktruss_file_name = (string(DataPrefixCurPath(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefixCurPath(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefixCurPath(dataname)) + queryid + ".query");

  QueryTCPGraph g;
  if (!FileHandler::testOpen(ktruss_file_name.c_str())){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefixCurPath(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str());
  Communities c;
  g.BuildTCPGraph(ontology_file_name.c_str());
  vector<vector<pair<int,int> >> Query;
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);

  output1(Query.size());
  int community_size = 0;
  char output_name_buf[1023];
#if defined(_WIN32)
  _mkdir((dataname + "_result").c_str());
#else
  mkdir(strPath.c_str(), 0777); // notice that 777 is different than 0777
#endif
  int nbr_total_querys = 0;
  for (int patitionID=0; patitionID < Query.size(); patitionID++)
    nbr_total_querys += Query[patitionID].size();
  fprintf(single_query_time_file(), "%d\n", nbr_total_querys);
  for (int patitionID = 0; patitionID <Query.size(); patitionID++){
    output1(Query[patitionID].size());
    Timer timer;
    //cout << "Sizeof parition: "<<end - begin <<" ["<<V_Size[begin].second << ", " <<V_Size[end - 1].second<<"]"<<endl;
    community_size = 0;
    for_iter_ ( query, Query[patitionID]){
      int query_id_in_group = query - Query[patitionID].begin();
      if ((query - Query[patitionID].begin()) % 10 ==0)
        cerr<<"#";
      int node_id(query->first), k(query->second);
      sprintf(output_name_buf, "%s/%s_%d_k%d.txt",
              (dataname + "_result").c_str(), (dataname).c_str(),
              node_id, k);
      freopen(output_name_buf, "w", stdout);
      //cout << node_id << " " << k << endl;
      g.TCPQuery(node_id, k, c);
      fprintf(single_query_time_file(), "%d %d %.2lf\n", node_id, k , timer.getTick());
      //cout << output_name_buf <<endl;
      c.DisplayCommunityForAnswerQuery([&](const int& eid){
                                       return make_pair(g.getEdge(eid).first, g.getEdge(eid).second);
                                       },[&](const int& nid){
                                       return nid + offset;
                                       }, node_id + offset);
      //community_size += c.size();
    }
    // cout << "community_size = "<< community_size/Query[patitionID].size()<<endl;
    cerr<<"\n";
    //timer.End();
  }

  //system("pause");
}
void QueryTime(const char* dataname = "WikiTalk", int offset = 0, string query_id ="", int timeout = 40 * 1){
  cout << "QueryTime " << endl;

  //writing file _query_k_max.log
  string ktruss_file_name = (string(DataPrefix(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefix(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefix(dataname)) + query_id + ".query");

  QueryTCPGraph g;
  if (!FileHandler::testOpen(ktruss_file_name.c_str())){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefix(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str());
  Communities c;
  g.BuildTCPGraph(ontology_file_name.c_str());
  vector<vector<pair<int,int> >> Query;
  if (FileHandler::testOpen(query_file_name.c_str()) == false) {
    cout << "Cannot open file" << query_file_name.c_str() << endl;
    return ;
  }
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);

  freopen((string(DataPrefix(dataname)) + "_tcp" + query_id + ".querytime").c_str(), "w", stdout);
  output1(Query.size());
  size_t community_size = 0;
  for (int patitionID = 0; patitionID <Query.size(); patitionID++){
    output1(Query[patitionID].size());
    Timer timer;
    //cout << "Sizeof parition: "<<end - begin <<" ["<<V_Size[begin].second << ", " <<V_Size[end - 1].second<<"]"<<endl;
    int community_size = 0;
    int timeoutFlag = -1;
    for_iter_ ( query, Query[patitionID]){
      if ((query - Query[patitionID].begin()) % 10 ==0)
        cerr<<"#";
      int node_id(query->first), k(query->second);
      cout << node_id << ":" << k << " ";
      g.TCPQuery(node_id, k, c);
      community_size += c.size();
      if (timer.getTick() >= 1000 * 60 * timeout)
      {
        timeoutFlag = query - Query[patitionID].begin();
        break;
      }
    }
    cout<<"\n";
    cout << "community_size = "<< floor((community_size + 0.0)/Query[patitionID].size())<<endl;
    cerr<<"\n";
    if (timeoutFlag >=0)
      cout << "size: "<<Query[patitionID].size()<<" time: "<< (timer.getTick() * Query[patitionID].size())/timeoutFlag<< " Timeout at query #"<<timeoutFlag<<" realtime:"<<timer.getTick() <<"\n";
    else
      cout << "size: "<<Query[patitionID].size()<<" time: "<< timer.getTick()<< "\n";
  }

  //system("pause");
}
void QueryEach(const char* dataname = "WikiTalk", int offset = 0, string query_id ="", int timeout = 40 * 1){
  cout << "QueryEach " << endl;
#define DataPrefixCurPath(x) (string(x) + "/" + string(x)).c_str()

  FileHandler single_query_time_file((string(DataPrefixCurPath(dataname)) + query_id + "_tcp_stime.txt").c_str(),"w");
  FileHandler group_query_time_file((string(DataPrefixCurPath(dataname)) + query_id + "_tcp_gtime.txt").c_str(),"w");
  string ktruss_file_name = (string(DataPrefixCurPath(dataname)) + "_truss.bin");
  string ontology_file_name = (string(DataPrefixCurPath(dataname)) + "_tcp.bin");
  string query_file_name = (string(DataPrefixCurPath(dataname)) + query_id + ".query");

  QueryTCPGraph g;
  if (!FileHandler::testOpen(ktruss_file_name.c_str())){
    kTrussGraph tg;
    tg.loadDataAdaptive(DataPrefixCurPath(dataname), ".txt" , offset);
    tg.kTruss(ktruss_file_name.c_str());
  }
  g.getTrussGraph(ktruss_file_name.c_str());
  Communities c;
  g.BuildTCPGraph(ontology_file_name.c_str());
  vector<vector<pair<int,int> >> Query;
  if (FileHandler::testOpen(query_file_name.c_str()) == false) {
    cout << "Cannot open file" << query_file_name.c_str() << endl;
    return ;
  }
  sinz::QueryLoader(FileHandler(query_file_name.c_str(), "rb"), Query);

  freopen((string(DataPrefix(dataname)) + "_tcp" + query_id + ".querytime").c_str(), "w", stdout);
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
      g.TCPQuery(node_id, k, c);
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
  if (program_id == "answer"){
    if (argn < 3){
      answer_query("sample3", 1); return 0;
    }
    else {
      string query_id = "";
      if (argn > 4){
        query_id = argv[4];
      }
      cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<query_id<<endl;
      answer_query(argv[2], atoi(argv[3]), query_id);
    }
    return 0;
  }
  if (program_id == "queryeach"){
    if (argn < 3){
      answer_query("sample3", 1); return 0;
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
  if (program_id == "buildtcp"){
    global::smartOutput smart_output_handle(("./" + string(argv[2]) + "/buildtime.txt").c_str(), global::stdinfo);

    if (argn < 3){
      QueryTime("sample3", 1);
    }
    else {
      string query_id = "";
      if (argn > 4){
        query_id = argv[4];
      }
      cerr<<argv[2]<<" offset "<<atoi(argv[3])<<" "<<query_id<<endl;
      QueryEach(argv[2], atoi(argv[3]), query_id);
    }
    global::stdinfo << "kTruss time(ms): "<<global::time_ktruss + global::time_tr_list <<"\n";
    global::stdinfo << "TCP time(ms): "<<global::time_ktruss + global::time_tr_list + global::time_tcp <<"\n";
  }

  return 0;
}
