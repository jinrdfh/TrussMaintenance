#include "util.h"
#include "QueryTCPGraph.h"
#include <sstream>
string DataFileF(string x){
  return (string("D:/data/wttian/datasets/Graph/data/") + x).c_str();
}
string DataPrefixF(string x){
  return (string("D:/data/wttian/datasets/Graph/data/") + x + "/" + x).c_str();
}
void makeUnDirect(string dataname, vector<vector<NodeEdge> > & g_){
  int edgeID = 0;
  cout << "\n";
  for (int i=0; i< g_.size(); i++){
    if (i%10000==0)
      printf("\r\t\t\t\t\t\t\r%d %d",i, edgeID);
    sort(g_[i].begin(), g_[i].end(),[](const NodeEdge& a, const NodeEdge& b){
         return a.to < b.to;
         });
    size_t new_size = unique(g_[i].begin(), g_[i].end(),[](const NodeEdge& a, const NodeEdge& b){
                             return a.to == b.to;
                             }) - g_[i].begin();

    g_[i].resize(new_size);
    for (int j=0; j<g_[i].size(); j++)
      g_[i][j].id = edgeID++;
  }
  printf("\r\t\t\t\t\t\t\r%d %d",g_.size() -1 , edgeID);
  cout << "\n";
  for (int i=0; i< g_.size(); i++){
    if (i%10000==0)
      printf("\r\t\r%d",i);
    for (int j=0; j<g_[i].size(); j++){
      if (g_[i][j].to > i)
        g_[g_[i][j].to].push_back(NodeEdge(i, g_[i][j].id));
    }
  }

  cerr<<("loaded\n");
  cerr<<("uniqued\n");
  int _nbrEdge = 0;
  for (int i=0; i<g_.size(); i++)
    _nbrEdge +=g_[i].size();

  cerr<<"#node = "<<g_.size()<<" 2 * #edge = "<<_nbrEdge<<endl;

  IO::dumpVectorVector(FileHandler((DataPrefixF(dataname) + string(".bin")).c_str(), "wb"),g_);

  cerr << "dumped"<<endl;
  system("pause");

}
int ConvertJamesBTC(string dataname, int offset = 0){
  cerr << "ConvertJamesBTC" << endl;
  int a,b;
  int edgeID = 0;
  int readEdge = 0;
  map<string, int> id;

  vector<vector<NodeEdge> > g_;

  FileHandler fd((DataPrefixF(dataname) + string(".txt")).c_str(), "rb");

  u_int sum = 0;

  //cerr<<"read"<<++readEdge<<" "<<a<<" "<<b<<endl;
  char buff[65536 * 4 - 1];
  int org_size = 0;
  int l = 0;
  while(fscanf(fd(), "%d %d", &a, &a)!=EOF){
    if (l++ % 100000==0)
      cout <<l<< " ";
    int tmp_n = 0;
    //system("pause");
    //cout << a << endl;
    fgets(buff, 65536 * 4 - 1, fd());
    //cout<<buff<<endl;
    //system("pause");

    stringstream ss;
    ss << buff;
    while(ss>>b){
      org_size++;
      if (a > b)
        swap(a, b);
      a-=offset;b-=offset;

      while (g_.size()<=a || g_.size()<=b ){
        g_.push_back(vector<sinz::NodeEdge >());
      }
      if (a==b)
        continue;

      g_[a].push_back( NodeEdge(b, edgeID) );
    }


  }
  cerr << "org_size" << org_size <<endl;
  makeUnDirect(dataname, g_);
  return 0;
}


int ConvertJamesWebUK(string dataname, int offset = 0){
  cerr << "ConvertJamesWebUK" << endl;
  int a,b;
  int edgeID = 0;
  int readEdge = 0;
  map<string, int> id;

  vector<vector<NodeEdge> > g_;

  FileHandler fd((DataPrefixF(dataname) + string(".txt")).c_str(), "rb");
  u_int n = 0;
  u_int sum = 0;
  char buf[65536*8 - 1];
  //cerr<<"read"<<++readEdge<<" "<<a<<" "<<b<<endl;

  int org_size = 0;
  int l = 0;
  while(fscanf(fd(), "%d", &a)!=EOF){
    if (l++ % 100000 ==0)
      cout <<l<< " ";
    int tmp_n = 0;
    fscanf(fd(), "%d", &tmp_n);
    org_size += tmp_n ;
    //cout << a << " " << tmp_n << endl;
    //system("pause");
    for (int i=0; i< tmp_n; i++){
      fscanf(fd(), "%d", &b);
      if (a > b)
        swap(a, b);
      a-=offset;b-=offset;

      while (g_.size()<=a || g_.size()<=b ){
        g_.push_back(vector<sinz::NodeEdge >());
      }
      if (a==b)
        continue;

      g_[a].push_back( NodeEdge(b, edgeID) );
    }


  }
  cerr << "org_size" << org_size <<endl;
  makeUnDirect(dataname, g_);
  return 0;
}

int loadFromHuBinOld(string dataname, int offset = 0){
  int a,b;
  int edgeID = 0;
  int readEdge = 0;
  map<string, int> id;

  vector<vector<NodeEdge> > g_;
  vector<int> _int_buf(100000);
  vector<pair<int, int> > _pair_buf(100000);
  vector<int> deg;
  cerr << string(DataPrefixF(dataname) + string(".deg")) << endl;
  FileHandler fdeg(string(DataPrefixF(dataname) + string(".deg")).c_str(), "rb");

  u_int nbrBlockRead;
  while (nbrBlockRead = fread(_pair_buf.data() , sizeof(pair<int, int>), _pair_buf.size(), fdeg())){
    size_t _n = deg.size();
    int max_id = 0;
    for (int i=0; i<nbrBlockRead; i++)
      max_id = max(_pair_buf[i].first, max_id);
    deg.resize( max_id + 1);
    fprintf(stderr, "\r\t\t\t\t\r%d", deg.size());
    for (int i=0; i<nbrBlockRead; i++){
      deg[_pair_buf[i].first] = _pair_buf[i].second;
    }
  }
  cerr << "\nfinished"<<endl;
  __int64 max_deg = 0, sum_deg = 0;
  for (size_t i=0; i < deg.size(); i++){
    max_deg = max( max_deg , deg[i]);
    sum_deg += deg[i];
  }
  cerr << max_deg<<endl;
  _int_buf.resize(max_deg);
  cerr << " org_ nbrNode = "<< deg.size() << " org_ nbrEdge = "<< sum_deg<<  endl;
  FileHandler fadj(string(DataPrefixF(dataname) + string(".adj")).c_str(), "rb");
  u_int sum_edge = 0;
  for (size_t i=0; i < deg.size(); i++){
    if (i%100000==0)
      fprintf(stderr, "\r\t\t\t\t\rProcessed %d", i);
    fread(_int_buf.data() , sizeof(int) * deg[i], 1 , fadj());
    sort(_int_buf.begin(), _int_buf.begin() + deg[i]);
    int new_deg_size =  unique(_int_buf.begin(), _int_buf.begin() + deg[i]) - _int_buf.begin();
    if (new_deg_size != deg[i])
      cerr <<deg[i] << " " << new_deg_size << "\n";
    for (int j = 0; j< new_deg_size; j++) {
      int a = i;
      b = _int_buf[j];
      if (a > b)
        swap(a, b);
      a-=offset;b-=offset;

      while (g_.size()<=a || g_.size()<=b ){
        g_.push_back(vector<sinz::NodeEdge >());
      }
      if (a==b)
        continue;

      g_[a].push_back( NodeEdge(b, edgeID) );

    }

  }
  makeUnDirect(dataname, g_);
  return 0;
}
int loadFromHuBin(string dataname, int offset = 0){
  typedef pair<int, int> pairII;
  int a,b;
  int edgeID = 0;
  int readEdge = 0;
  map<string, int> id;

  vector<vector<NodeEdge> > g_;
  vector<int> _int_buf(100000);
  vector<pair<int, int> > _pair_buf(100000);
  vector<int> deg;
  cerr << string(DataPrefixF(dataname) + string(".deg")) << endl;
  FileHandler fdeg(string(DataPrefixF(dataname) + string(".deg")).c_str(), "rb");

  u_int nbrBlockRead;
  while (nbrBlockRead = fread(_pair_buf.data() , sizeof(pair<int, int>), _pair_buf.size(), fdeg())){
    size_t _n = deg.size();
    int max_id = 0;
    for (int i=0; i<nbrBlockRead; i++)
      max_id = max(_pair_buf[i].first, max_id);
    deg.resize( max_id + 1);
    fprintf(stderr, "\r\t\t\t\t\r%d", deg.size());
    for (int i=0; i<nbrBlockRead; i++){
      deg[_pair_buf[i].first] = _pair_buf[i].second;
    }
  }
  cerr << "\nfinished"<<endl;
  __int64 max_deg = 0, sum_deg = 0;
  for (size_t i=0; i < deg.size(); i++){
    max_deg = max( max_deg , deg[i]);
    sum_deg += deg[i];
  }
  cerr << max_deg<<endl;
  _int_buf.resize(max_deg + 1);
  cerr << " org_ nbrNode = "<< deg.size() << " org_ nbrEdge = "<< sum_deg<<  endl;
  vector<pair<int,int> > _edges(sum_deg);
  FileHandler fadj(string(DataPrefixF(dataname) + string(".adj")).c_str(), "rb");
  size_t sum_edge = 0;
  for (size_t i=0; i < deg.size(); i++){
    if (i%100000==0)
      fprintf(stderr, "\r\t\t\t\t\rProcessed %d", i);
    int a = i;
    fread(_int_buf.data() , sizeof(int) * deg[i], 1 , fadj());
    for (int j=0; j< deg[i]; j++){
      b = _int_buf[j];
      _edges[sum_edge + j] = make_pair(a, b);
    }
    sum_edge += deg[i];
  }
  sort(_edges.begin(), _edges.end(), [](const pairII& a, const pairII& b){
       return a.first == b.first ?  a.second  < b.second :  a.first < b.first;
       });
  auto new_size = unique(_edges.begin(), _edges.end(), [](const pairII& a, const pairII& b){
                         return a.first == b.first && a.second == b.second;
                         }) - _edges.begin();
  for (int i=0; i<100;i++)
    cout<<_edges[i].first << " " << _edges[i].second<< endl;
  output2(_edges.size(), new_size);
  //makeUnDirect(dataname, g_);
  system("pause");
  return 0;
}
