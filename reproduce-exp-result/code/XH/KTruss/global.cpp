#include "global.h"
#include <map>
#include <iostream>
#include <fstream>
#include <string>

namespace sinz {
namespace global {

using namespace std;

std::map<string,string> flag;
std::ofstream stdinfo;
double time_tr_list, time_ktruss, time_tcp;

void showflags(){
  for (auto it = flag.begin(); it != flag.end(); it++)
    std::cout << it->first << " " << it->second << std::endl;
}
smartOutput::smartOutput(const char * filename, ofstream& stdinfo) {
  _stdinfo = &stdinfo;
  _stdinfo->open(filename, std::ofstream::out);
}
smartOutput::~smartOutput() {
  if (_stdinfo) {
    std::cerr << "closed info file\n";
    _stdinfo->close();
  }
}
}
}
