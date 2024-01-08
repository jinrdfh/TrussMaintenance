#ifndef H_UTIL
#define H_UTIL
//#define DEBUG_MODE
#define debug_2013_11_14

#include <cstring>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <ctime>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

#if defined(_WIN32)
#include <Windows.h>
#include <direct.h>
#endif
#if (defined _MSC_VER)
#include <Windows.h>
#endif

typedef size_t IndexType;
typedef long long __int64;

//typedef size_t IndexType;
#define stdcerr (std::cerr)
#define iterall_(a) a.begin(), a.end()
#define vector2(T) vector<vector<T> >
#define makehash(a,b) (((__int64)a<<32) | b )
#define for_(v,s, t) for (size_t v = s; v < t ; v++)
#define for_iter_(iter, container) for (auto iter = (container).begin(); iter != (container).end() ; iter++)
#define for_iiter_(iter, container) for (auto iter = (container)->begin(); iter != (container)->end() ; iter++)

#define max_v(v, v0, container) v0; for_iter_(iter, container) v = max(v, *iter);
#define vec_int vector<int>
#define sz_(v) int(v.size())
#define in_(c, v) ((c).find(v) != (c).end())
using namespace std;

#define output1(v) cerr<<#v<<" = "<<v<<endl;
#define output2(v1,v2) cerr<<#v1<<","<<#v2<<" = "<<v1<<","<<v2<<endl;
#define output3(v1,v2,v3) cerr<<#v1<<","<<#v2<<","<<#v3<<" = "<<v1<<","<<v2<<","<<v3<<endl;
#define output4(v1,v2,v3,v4) cerr<<#v1<<","<<#v2<<","<<#v3<<" = "<<v1<<","<<v2<<","<<v3<<","<<v4<<endl;
#ifdef DEBUG_MODE
#define dbg(v) cerr<<#v<<" = "<<v<<endl;
#define dbg2(v1,v2) cerr<<#v1<<","<<#v2<<" = "<<v1<<","<<v2<<endl;
#define dbg3(v1,v2,v3) cerr<<#v1<<","<<#v2<<","<<#v3<<" = "<<v1<<","<<v2<<","<<v3<<endl;
#define dbgL(t) t
#else
#define dbg(v)
#define dbg2(v1,v2)
#define dbg3(v1,v2,v3)
#define dbgL(t)
#endif

#if (defined _MSC_VER) || (defined __INTEL_COMPILER)
#define STRONG_INLINE __forceinline
#else
#define STRONG_INLINE inline
#endif

namespace sinz{


class FileHandler{
  FILE* fd_ ;
 public:
  static bool testOpen(const char* fn){
    FILE* f = fopen(fn, "rb");
    if (f == NULL){
      return false;
    }
    fclose(f);
    return true;
  }
  FileHandler(){}
  FileHandler(const char* fn, const char * mode, bool flush = false){
    open(fn, mode, flush);
  }
  STRONG_INLINE FILE* operator()() {
    return fd_;
  }
  void open(const char* fn, const char * mode, bool flush = false){
    fd_ = fopen(fn, mode);
    if (fd_ ==NULL){
      cerr<<"Cannot Open File: "<< fn<<endl;
    }
    if (flush)
      setbuf(fd_,0);
    assert(fd_ != NULL);
    //dbg("open file");
  }
  ~FileHandler(){
    if (fd_){
      fclose(fd_);
      //dbg("closed file");
    }
    //dbg("~FileHandler");
  }
  void flush(){
    fflush(fd_);
  }
  void close(){
    if (fd_){
      fclose(fd_);
      //dbg("closed file");
    }
  }

};
namespace IO{
void WriteBlock(FileHandler& fd, void* buf, size_t size );
void ReadBlock(FileHandler& fd, void* buf, size_t size);
void WriteBlockN(FileHandler& fd, void* buf, size_t size, size_t blocks );
void ReadBlockN(FileHandler& fd, void* buf, size_t size, size_t blocks );
template<class T>
void dumpVectorVector(FileHandler& fd, vector<vector<T> >& v){
  int n = v.size();
  WriteBlock(fd, &n, sizeof(n));
  if (n==0)
    return ;
  vector<int> s(n);
  for (int i=0; i<n; i++)
    s[i] = v[i].size();
  WriteBlock(fd, s.data(), sizeof(s[0]) * s.size());
  for (int i=0; i<v.size(); i++){
    if (s[i]==0)
      continue;
    WriteBlock(fd, v[i].data(), sizeof(T) * s[i]);
  }
}
template<class T>
void readVectorVector(FileHandler& fd, vector<vector<T> >& v){
  int n = 0 ;
  ReadBlock(fd, &n, sizeof(n));
  //cerr << n <<endl;
  if (n==0)
    return;
  v.resize(n);
  //cerr << n;
  vector<int> s(n);
  ReadBlock(fd, s.data(), sizeof(s[0]) * s.size());
  for (int i=0; i<v.size(); i++){
    if (s[i]==0)
      continue;
    v[i].resize(s[i]);
    ReadBlock(fd, v[i].data(), sizeof(T) * s[i]);
  }
}

template<class T>
void dumpVector(FileHandler&& fd, vector<T >& v){
  dumpVector(fd, v);
}
template<class T>
void dumpVector(FileHandler& fd, vector<T >& v){
  int n= v.size();
  WriteBlock(fd, &n, sizeof(n));
  if (n==0)
    return ;
  WriteBlockN(fd, v.data(), sizeof(T), v.size());
}
template<class T>
void readVector(FileHandler&& fd, vector<T >& v){
  readVector(fd, v);
}
template<class T>
void readVector(FileHandler& fd, vector<T >& v){
  int n = 0;
  ReadBlock(fd, &n, sizeof(n));
  //output1(n);
  if (n==0)
    return ;
  v.resize(n);
  ReadBlockN(fd, v.data(), sizeof(T),  v.size());
}

template<class T>
void dump(FileHandler& fd, T& n){
  WriteBlock(fd, &n, sizeof(n));
}
template<class T>
void read(FileHandler& fd, T& n){
  ReadBlock(fd, &n, sizeof(n));
}
};
template<class T, class Size_Type = size_t>
class vector2d:public std::vector<T>{
 protected:
  vector<Size_Type> pt;
 public:
  void advance(int x, Size_Type step = 1){
    pt[x]+=step;
  }
  vector2d(){
    this->reserve(1);
    pt.push_back(0);
  }
  vector2d(int n){

    if (n>0)
      this->resize(n);
    pt.push_back(0);
  }
  inline void makeEnd(Size_Type end = 0){
    if (end > 0)
      pt.push_back(end);
    else {
      //dbg(size());
      pt.push_back(this->size());
    }
  }
  inline T* beginPt(int i){
    //dbg3(i, pt[i], data());
    return i >= pt.size() ? this->data() + this->size() : pt[i] + this->data();
  }
  inline Size_Type beginInd(int i){
    return i >= pt.size() ? this->size() : pt[i];
  }
  inline Size_Type size1d(){
    return pt.size() - 1;
  }
  inline Size_Type size2d(){
    return this->size();
  }
  inline Size_Type size2d(int x){

    return x >= pt.size() ? 0 : pt[x + 1] - pt[x];
  }
  void reset(){
    this->clear();
    pt.clear();
    pt.push_back(0);
  }
  template<class Pred>
void sort(Pred _Pr ){
  sort(_Pr, [](const T& a, const T& b){return a!=b;});
}
template<class T3>
void fromSizeVec(vector<T3>& s){
  pt.clear();
  if (s.size()>0)
    pt.reserve(s.size());
  size_t sum = 0;
  for (int i = 0; i<s.size(); i++){
    pt.push_back(sum);
    sum += s[i];
  }
  pt.push_back(sum);
}
template<class Pred, class Pred2>
void sort(Pred _Pr, Pred2 _Pr2){
  T* begin = this->data(), *end = this->data() + this->size();
  std::sort(begin, end, _Pr);
  pt.clear();
  size_t lt = -1;
  for (T * p= begin; p<end; p++)
  {
    if (p==begin || (_Pr2(*p) != _Pr2(*(p-1)))){
      for (size_t j = lt + 1; j <= _Pr2(*p); j++)
        pt.push_back(p - this->data());
      lt = _Pr2(*p);
    }
  }
  pt.push_back(this->size());
}
template<class Pred>
int sum(Pred _Pr){
  int s= 0;
  T* begin = this->data(), *end = this->data() + this->size();
  for (T * p= begin; p<end; p++)
    s+=_Pr(*p);
  return s;
}
void dump(FileHandler& fd){
  IO::dumpVector(fd, *this);
  IO::dumpVector(fd, pt);
}
void read(FileHandler& fd){
  sinz::IO::readVector(fd, *this);
  IO::readVector(fd, pt);
}
};

template<class KeyType, class ValueType>
class AdaptiveMap{
  unordered_map<KeyType, ValueType>* _ht;
  map<KeyType, ValueType>* _mp;
  vector<KeyType, ValueType>* _v;
 public:
  AdaptiveMap(){}
  ~AdaptiveMap(){

  }
};

template<class T>
class Queue{
  //queue that to avoid pop all elements.
  std::vector<T> v_;
  size_t front, end;
 public:
  Queue(int n){
    front = end = 0;
    v_.resize(n);
  }
  void clear(int n = 0){
    if (v_.size()<n)
      v_.resize(n);
    front = end = 0;
  }
  STRONG_INLINE T pop(){
    return v_[front++];
  }
  STRONG_INLINE void push(const T& a){
    v_[end++];
  }
  STRONG_INLINE bool empty(){
    return end==front;
  }
  STRONG_INLINE size_t size(){
    return end;
  }
};

template<class T>
class queue:public vector<T>{
		//queue that to avoid pop all elements.
		size_t ql, _Myfirst, _Mylast;;
	public:
		queue(){
			reset();
		}
		void reset(){
			ql = 0;
			_Myfirst = 0;
			_Mylast = 0;
			this->clear();
		}
		
		/*STRONG_INLINE T front(){
			return _Myfirst[ql];
		}*/

		STRONG_INLINE T pop(){
			//return this->_Myfirst[ql++];
			return this->at(ql++);
		}
		STRONG_INLINE bool empty(){
			return this->_Myfirst + ql == this->size();
		}
		STRONG_INLINE size_t sizeInQueue(){
			return this->size() - (this->_Myfirst + ql);
		}
	};

template<class T, class T2, bool rev_order = false>
class KeyValue{
 public:
  T fi;
  T2 se;
  KeyValue(){}
  KeyValue(T _a, T2 _b):fi(_a),se(_b){}
  bool operator<(const KeyValue& rhs) const {
    return rev_order ? fi>rhs.fi : fi<rhs.fi;
  }
};
template<class T>
void dumpIter(FileHandler& fd, T& v, int offset = 0){

  for_ (i, 0, v.size()){
    fprintf(fd(), "%d %d\n", i + offset, v[i] );
  }
}

#ifdef _MSC_VER
class Timer{
  clock_t start;
  double sum;
  double PCFreq;
  __int64 CounterStart;
  FILE * myout;
  void StartCounter()
  {
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
      cout << "QueryPerformanceFrequency failed!\n";

    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
  }
  double GetCounter()
  {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
  }

 public:
  Timer(){
    StartCounter();
    myout = stdout;
  }
  Timer(FILE * myout_){
    Start();
    myout = myout_;
  }
  void Start(){
    StartCounter();
  }
  void End(string str = ""){
    fprintf(myout, "%s Ellapsed Time: %.4lf ms\n", str.c_str(), GetCounter());
  }
  double getTick(){
    return GetCounter();
  }
  void Wake(){
    Start();
  }
  void Sleep(){
    sum +=  getTick();
  }
  double Sum(string str = ""){
    if (str!="silent")
      fprintf(myout, "%s Ellapsed Time: %.4lf ms\n", str.c_str(), (sum + .0));
    return sum;
  }
  double End(int time){
    double ticks = getTick();
    fprintf(myout, "Avg Time: %.4lf,  #time = %d\n", (ticks + .0)/time, time);
    return ticks;
  }
};

#else

class Timer{
  clock_t start;
  clock_t sum;
  double PCFreq;
  __int64 CounterStart;
  FILE * myout;

 public:
  Timer(){
    Start();

  }
  void Start(){
    start = clock();
  }
  Timer(FILE * myout_){
    Start();
    myout = myout_;
  }
  void End(string str = ""){
    fprintf(stdout, "%s Ellapsed Time: %.4lf\n", str.c_str(), (clock() - start + .0)/CLOCKS_PER_SEC);
  }
  double getTick(){
    return (clock() - start);
  }
  void Wake(){
    Start();
  }
  void Sleep(){
    sum +=  getTick();
  }
  double  Sum(string str = ""){
    if (str !="silent")
      fprintf(stdout, "%s Ellapsed Time: %.4lf\n", str.c_str(), (sum + .0)/CLOCKS_PER_SEC);
    return (sum + .0)/CLOCKS_PER_SEC;
  }
  void End(int time){
    fprintf(stdout, "Avg Time: %.4lf,  #time = %d\n", (getTick() + .0)/time, time);
  }
};

#endif
class UnionSet{
 public:
  unordered_map<int, int> _id;
  vector<int> _father;
  vector<int> _rank;
  inline int getf(int x) {
    int p = x, t;
    while (_father[p] != p)
      p = _father[p];
    while (x != p) {
      t = _father[x];
      _father[x] = p;
      x = t;
    }
    return x;
  }

 public:
  UnionSet(){}
  void reset(int _size){
    if (_size > _father.size()){
      _father.resize(_size);
      _rank.resize(_size);
    }
    for (int i=0;i<_size; i++)
      _father[i]=i;
    memset(_rank.data(), 0 , sizeof(int) * _size);
  }
  UnionSet(int _size){
    reset(_size);
  }
  STRONG_INLINE int FindSet(int x){
    int& fa = _father[x];
    return _father[fa] == fa ? fa : fa = getf(x);
  }
  inline void Union(int x, int y) {
    if ((x = getf(x)) == (y = getf(y))) return;
    if (_rank[x] > _rank[y])
      _father[y] = x;
    else {
      _father[x] = y;
      if (_rank[x] == _rank[y])
        _rank[y]++;
    }
  }

  template<class T, class _Pr>
void setMap(vector<T>& v, _Pr Pred){
  for (auto it = v.begin() ; it != v.end(); it++){
    _id[Pred(it)] = static_cast<int>(_id.size());
  }
}

void Display(){
  Display([](const int x){return x;});
}

template<class _Pr>
void Display(_Pr Pred){
  for (int i=0; i< _father.size(); i++){
    cout<<Pred(FindSet(i))<<" ";
  }
  cout<<endl;
}
};
template<class ValueType, class IndexType>
class Vector2dM:public vector<vector<ValueType> >{
 public:
  Vector2dM(){
  }
  inline void push(IndexType& x, ValueType& v){
    while (x >= this->size())
      push_back(vector<ValueType>());

  }
  static void load_from_vector2d(vector<vector<ValueType> >& v, FileHandler& fd){
    vector2d<ValueType> v2d;
    v2d.read(fd);
    v.resize(v2d.size1d());
    for (size_t i=0; i<v2d.size1d(); i++){
      v[i].resize(v2d.size2d(i));
      copy(v2d.beginPt(i), v2d.beginPt(i + 1), v[i].begin());
    }
  }
  static void dump_into_vector2d(vector<vector<ValueType> >& v, FileHandler& fd){
    vector2d<ValueType> v2d;
    vector<size_t> S(v.size());
    size_t s =0 ;
    for (size_t i=0; i<v.size(); i++){
      s += v[i].size();
      S[i] = v[i].size();
    }
    //output1(v.size());
    v2d.resize(s);
    s = 0;
    for (int i=0; i<v.size(); i++){
      copy(v[i].begin(), v[i].end(), v2d.begin() + s);
      s += v[i].size();
    }
    v2d.fromSizeVec(S);
    v2d.dump(fd);
  }
};

template<class T, class T_SIZE = int>
class Array{
  /*
     2013-11-8
     1. is this designed for simple type such as int, double, bool?
     2. 2D vector is more important than 1D. vector is already implemented to be very efficient.
     Becasue 2D vector *may* allocated in different locations, we prefer vectors are allocated in consective space.

*/
  T* _data;
  T_SIZE _size;
 public:
  Array(){
    _size = 0;
  }
  Array(int s_):_size(s_){
    resize(_size);
  }

  void resize_v(int s_, T& value){
    resize(s_);
    T* end = _data + _size;
    for (T* _p = _data; _p != end ; _p++)
      *_p = value;
  }
  void resize(int s_, T& value){
    resize(s_);
    memset(_data, value, sizeof(T) * _size);
  }
  void resize(int s_){
    T* _data_new = (T*) realloc(_data, sizeof(T)*_size);
    _data = _data_new;
    _size = s_;
  }
  inline T& operator[](T_SIZE x){
    return _data[x];
  }
  inline T_SIZE size(){
    return _size;
  }

  ~Array(){
    if (_data)
      free(_data);
  }
};
template<class T>
class BFSqueue{
 public:
  vector<T> q;
  vector<int> vis;
  int _qid;
  int ql, qr;
 public:
  BFSqueue(){
    _qid = 0;
    reset();
  }
  BFSqueue(int _s){
    BFSqueue();
    q.resize(_s);
    vis.resize(_s, 0);
    reset();
  }
  void clear (int _s){
    if (q.size() < _s){
      BFSqueue();
      q.resize(_s);
      vis.resize(_s, 0);
    }
    reset();
  }
  STRONG_INLINE void push(T t){

    if (vis[t] != _qid) {
      q[qr++] = t;
      vis[t] = _qid;
    }
  }
  STRONG_INLINE T& pop(){
    return q[ql++];
  }
  STRONG_INLINE bool empty(){
    return ql==qr;
  }
  void reset(){
    _qid++;
    ql = qr =0;
  }
};


template<class T>
class SeqVector2D{
  vector<T> _v;
  vector<T> _B;

 public:
  SeqVector2D(){
    reset();
  }
  inline void makeEnd(){
    _B.push_back(_v.size());
  }
  void reset(){
    _v.clear();
    _B.clear();
    _B.push_back(0);
  }
  inline void push_back(T& a){
    _v.push_back(a);
  }
  inline void push_back(T&& a){
    _v.push_back(a);
  }
  inline size_t size(){
    return _B.size() - 1;
  }
  inline size_t sizesize(){
    return _v.size();
  }
  inline T* begin(int i){
    return _v.data() + _B[i];
  }
  inline T* end(int i){
    return _v.data() + _B[i+1];
  }
  inline T* begin(){
    return _v.data();
  }
  inline T* end(){
    return _v.data() + _v.size();
  }
};
namespace Sample{
template<class T, class T2>
void Stratify(T& adjV, vector<pair<int, T2 > >& V_Size, vector<int>& spliter){
  int cnt = 0;
  for (int i=0; i<adjV.size(); i++){
    V_Size[i] = make_pair(i, (int)adjV[i].size());
    cnt += V_Size[i].second;
  }
  sort(V_Size.begin(), V_Size.end(), [](const pair<int, int> & a, const pair<int, int> & b){return a.second > b.second;});
  spliter.resize(4);
  spliter[0] = 0;
  spliter[1] = (int)ceil(V_Size.size() * 0.05);
  spliter[2] = (int)ceil(V_Size.size() * 0.4);
  spliter[3] = V_Size.size();
}
template<class T, class T2>
void Stratify(T& adjV, vector<pair<int, T2 > >& V_Size, vector<int>& spliter, int nbrPartition){
  int cnt = 0;
  for (int i=0; i<adjV.size(); i++){
    V_Size[i] = make_pair(i, (int)adjV[i].size());
    cnt += V_Size[i].second;
  }
  sort(V_Size.begin(), V_Size.end(), [](const pair<int, int> & a, const pair<int, int> & b){return a.second > b.second;});
  spliter.resize(nbrPartition + 1);
  for (int i=0; i<nbrPartition; i++)
    spliter[i] = (int)ceil(V_Size.size() * (i + .0)/nbrPartition);
  spliter[nbrPartition] = V_Size.size();
}
/*
   template<class T, class T2>
void Stratify(vector2d<T> Edges, vector<pair<int, T2 > >& V_Size, vector<int>& spliter){
int cnt = 0;
for (int i=0; i<adjV.size(); i++){
V_Size[i] = make_pair(i, (int)adjV[i].size());
cnt += V_Size[i].second;
}
sort(V_Size.begin(), V_Size.end(), [](const pair<int, int> & a, const pair<int, int> & b){return a.second > b.second;});
spliter.resize(4);
spliter[0] = 0;
spliter[1] = (int)ceil(V_Size.size() * 0.05);
spliter[2] = (int)ceil(V_Size.size() * 0.4);
spliter[3] = V_Size.size();
}
*/
unsigned int rand30();
//__int64 rand64();
STRONG_INLINE __int64 int64(int x,int y);
}
};
namespace sinz{

namespace util{


template<typename T, typename T2>
STRONG_INLINE bool cmp_pair_by_second_less(const pair<T, T2>& a, const pair<T, T2>& b) {
  return (a.second < b.second);
}
template<typename T, typename T2>
STRONG_INLINE bool cmp_pair_by_second_greater(const pair<T, T2>& a, const pair<T, T2>& b) {
  return (a.second > b.second);
}

STRONG_INLINE int incGet(unordered_map<int, int>& d, int k){
  return (d.find(k)==d.end())? d[k] = d.size() : d[k];
}

namespace config{
/*
   class Value{
   std::string _s;
   double _d_s;
   int _i_s;
   public:
   Value(){

   }
   Value(string s){
   _s = s;
   }
   void Int(){
   sscanf(_s.c_str(), "%d", &_i_s);
   }
   void Double(){
   sscanf(_s.c_str(), "%lf", &_d_s);
   }
   };
   map<std::string, Value> read_parameter(int argn, char** argv){
   map<std::string, Value> p;
   for (int i = 0; i < argn; i++){

   }
   }
   */
}
}

};

#endif
