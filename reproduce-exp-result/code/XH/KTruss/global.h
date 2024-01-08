#ifndef H_GLOBAL
#define H_GLOBAL
//#define MACHINE_IS08
#ifdef MACHINE_IS08
#define DataFile(x) (string("D:/wttian/Research/data2/") + x).c_str()
#define DataPrefix(x) (string("D:/wttian/Research/data2/") + x + "/" + x).c_str()
#else
#define DataFile(x) (string("D:/data/wttian/datasets/Graph/data/") + x).c_str()
#define DataPrefix(x) (string("D:/data/wttian/datasets/Graph/data/") + x + "/" + x).c_str()
#endif

#include <map>
#include <iostream>
#include <fstream>
#include <string>
namespace sinz{
namespace global{

using namespace std;

extern std::map<string,string> flag;
extern std::ofstream stdinfo;
extern double time_tr_list, time_ktruss, time_tcp;

void showflags();
class smartOutput {
  std::ofstream* _stdinfo;
 public:
  smartOutput(const char * filename, ofstream& stdinfo);
  ~smartOutput();
};
}
}

#endif
