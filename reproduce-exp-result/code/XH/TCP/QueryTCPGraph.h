#include "QueryGraph.h"
#ifndef H_QUERYTCPGRAPH
#define H_QUERYTCPGRAPH
typedef long long __int64;
class QueryTCPGraph:public sinz::graph::QueryGraph{
	void BuildTCPGraphInside();
	vector<vector2d<_WeightEdge> >  Forest;
	vector<int> gVis;
	vector<int> gdVis;
	vector<int> geID;
	vector<int> gdeIDrev;
	sinz::queue<int> q;
	BFSqueue<int> TCPq;
	unordered_map<__int64, int> indexOfNode64;

public:
	vector2d<_WeightEdge> Edges;

	QueryTCPGraph(){

	}
	QueryTCPGraph(kTrussGraph& kG, const char* prefix, const char *suffix, int offset){
		kG.loadDataAdaptive(prefix, suffix , offset);
		kG.kTruss();
	}
	void BuildTCPGraph(const char* file = 0);
	void getTrussGraph(const char* truss_file_name);
	void TCPQuery(int v0, int k, graph::Communities& c);
};
#endif
