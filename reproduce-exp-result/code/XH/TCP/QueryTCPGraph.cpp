#include "QueryTCPGraph.h"
#include "global.h"
void QueryTCPGraph::BuildTCPGraph(const char* fn){

	//#define ongoing_2013_11_28_hash
	bool option_loading = 0;
	if (fn && FileHandler::testOpen(fn) && FileHandler::testOpen(fn))
		option_loading = true;
	Timer timer(stderr);
	dbg2(_edge.size(), _nbrEdge);
	if (FileHandler::testOpen((string(fn) + "_aux").c_str())){
		FileHandler fdsup((string(fn) + "_aux").c_str(), "rb");
		Edges.read(fdsup);
		for (int i=Edges.size() - 10; i<Edges.size();i++)
			output3(Edges[i].from, Edges[i].to, Edges[i].truss);
		IO::readVector(fdsup, geID);
		IO::readVector(fdsup, gdeIDrev);
		output3(Edges.size(), geID.size(), gdeIDrev.size());
	}
	else
	{
		Edges.resize(_edge.size() * 2);
		for (size_t eid=0; eid<_edge.size(); eid++){
			Edges[eid * 2] = _edge[eid];
			Edges[eid *2 + 1] = _WeightEdge(_edge[eid].to, _edge[eid].from, _edge[eid].truss);
		}
		dbg(Edges.size());
		
		Edges.sort([](const _WeightEdge& a, const _WeightEdge& b){
			//!!!......must change to stable sort!
			return a.from == b.from? a.truss > b.truss : a.from < b.from;
		}, [](const _WeightEdge& a){
			return a.from;
		});
		
		FileHandler fdsup;
		if (fn){
		
			fdsup.open((string(fn) + "_aux").c_str(), "wb");
			cerr << "dumping Edges" << Edges.size()<< "\n";
			Edges.dump(fdsup);
			
		}
		timer.End();
		//cerr<<"Create hash";system("pause");
#ifndef ongoing_2013_11_28_hash
		_indexOfNode = graph::Pair_Hash<int,int>(Edges);
#else

#endif
		timer.End();
		//cerr<<"Create hash finish";system("pause");

		timer.End();
#ifndef ongoing_2013_11_28_hash
		for (size_t eid = 0; eid < _edge.size(); eid++){
			_indexOfNode[_edge[eid].to][_edge[eid].from] = eid;
			_indexOfNode[_edge[eid].from][_edge[eid].to] = eid;
		}
#else
		for (size_t eid = 0; eid < _edge.size(); eid++){
			indexOfNode64[makehash(_edge[eid].to, _edge[eid].from)] = eid;
			indexOfNode64[makehash(_edge[eid].from,_edge[eid].to)] = eid;
		}
#endif
		//cerr<<"build hash finish";system("pause");
		timer.End();

		
		geID.reserve(Edges.size());
		gdeIDrev.reserve(Edges.size());

		for (size_t i=0; i< Edges.size(); i++){
#ifndef ongoing_2013_11_28_hash
			geID.push_back(_indexOfNode[Edges[i].from][Edges[i].to]); // here is _indexOfNode is eid
#else
			geID.push_back(indexOfNode64[makehash(Edges[i].from,Edges[i].to)]); // here is _indexOfNode is eid
#endif
		}
		timer.End();
		for (size_t gd_eid=0; gd_eid< Edges.size(); gd_eid++){
#ifndef ongoing_2013_11_28_hash
			_indexOfNode[Edges[gd_eid].from][Edges[gd_eid].to] = gd_eid; // here is _indexOfNode became gd_eid
#else
			indexOfNode64[makehash(Edges[gd_eid].from,Edges[gd_eid].to)] = gd_eid; // here is _indexOfNode became gd_eid
#endif
		}
		timer.End();
		for (auto gd_e_it=Edges.begin(); gd_e_it !=Edges.end(); gd_e_it++){
#ifndef ongoing_2013_11_28_hash
			gdeIDrev.push_back( _indexOfNode[gd_e_it->to][gd_e_it->from] );
#else
			gdeIDrev.push_back( indexOfNode64[makehash(gd_e_it->to,gd_e_it->from)] );
#endif
		}
		timer.End();
		if (fn){

			cerr << "dumping geID" <<  geID.size()<< "\n";
			IO::dumpVector(fdsup, geID);
			cerr << "dumping gdeIDrev\n" <<  gdeIDrev.size()<< "\n";
			IO::dumpVector(fdsup, gdeIDrev);		
			fdsup.close();
		}
	}
	cerr <<" aux builded"<<endl;
	Forest.resize(_nbrNode);
	if (true && fn && FileHandler::testOpen(fn)){

		FileHandler fd(fn, "rb");
		cerr <<"< Loading TCP Graph>"<<endl;
		for (int v = 0; v < Edges.size1d(); v++)
			Forest[v].read(fd);
	}else {
		cerr << "BuildTCPGraphInside"<< endl;
		BuildTCPGraphInside();
		if (fn){
			FileHandler fd(fn, "wb");
			cout <<"Dumping TCP"<<endl;
			for (int v = 0; v < Forest.size(); v++){
				dbg(v);
				Forest[v].dump(fd);
				dbg(v);
			}
		}
	}

	gVis.resize(_edge.size(), 0);
	gdVis.resize(Edges.size(), 0);
	//_edge.swap(vector<_WeightEdge>());
#ifndef ongoing_2013_11_28_hash
	_indexOfNode.deallocate();
#else
	indexOfNode64.swap(unordered_map<__int64,int>());
#endif
	cerr << "build TCP finished\n";
}

void QueryTCPGraph::BuildTCPGraphInside(){

	Timer info_TCP;
	Timer timer;
	vector<_WeightEdge> tmpEdge;
	UnionSet T;
	int __profiling_nbrs_size = 0;
	int __profiling_edge_size = 0;
	int __profiling_forest_size = 0;
	Timer __profiling_edge_time;
	Timer __profiling_forest_time;
	for (int x=0; x<Edges.size1d(); x++){
		int nbrNeibr = Edges.size2d(x);
		__profiling_nbrs_size += nbrNeibr;
		if (x%10000==0)
			fprintf(stderr,"\r\t\t\t\rBuilding Node:%d Edges:%d Tr:%d Fo:%d %lf %lf %lf", x,__profiling_nbrs_size,
			__profiling_edge_size, __profiling_forest_size,
			__profiling_edge_time.Sum("silent"), __profiling_forest_time.Sum("silent"), timer.getTick());
		tmpEdge.clear();

		//dbg2(x, Edges.size1d());
		__profiling_edge_time.Wake();
		_WeightEdge *begin_x = Edges.beginPt(x);
		for (_WeightEdge *y_it = begin_x, *y_end = Edges.beginPt(x + 1); y_it < y_end; y_it++ ){
			const int& y = y_it->to;
			//dbg3(y, Edges.size2d(x), Edges.size2d(y));

			if (Edges.size2d(x) > Edges.size2d(y)){
				//dbg(y);
				_WeightEdge *begin_y = Edges.beginPt(y);

				for (_WeightEdge *z_it = Edges.beginPt(y), *z_end = Edges.beginPt(y + 1); z_it < z_end; z_it++ ){
					const int& z = z_it->to;
#ifndef ongoing_2013_11_28_hash
					int xz_id  = _indexOfNode.getValue(x,z);
#else
					__int64 hash_xz = makehash(x,z);
					int xz_id  = indexOfNode64.find(hash_xz) == indexOfNode64.end() ? -1 : indexOfNode64[hash_xz];
#endif
					//dbg3(xz_id, x, z);
					if (xz_id < 0 )
						continue;
					//dbg(xz_id);
					int w = min(min(y_it->truss, z_it->truss), Edges[xz_id].truss);
					tmpEdge.push_back(_WeightEdge(y_it - begin_x, xz_id - (begin_x - Edges.data()), w));
				}
			}
			else {
				//dbg(y);
				//dbg2(Edges.size1d(), y + 1);
				for (_WeightEdge *z_it = Edges.beginPt(x), *z_end = Edges.beginPt(x + 1); z_it < z_end; z_it++ ){
					const int& z = z_it->to;
#ifndef ongoing_2013_11_28_hash
					int yz_id  = _indexOfNode.getValue(y,z);
#else
					__int64 hash_yz = makehash(y,z);
					int yz_id  = indexOfNode64.find(hash_yz) == indexOfNode64.end() ? -1 : indexOfNode64[hash_yz];
#endif
					//dbg(yz_id);
					if (yz_id < 0 )
						continue;
					int w = min(min(y_it->truss, z_it->truss), Edges[yz_id].truss);
					tmpEdge.push_back(_WeightEdge(y_it - begin_x, z_it - begin_x, w));
				}
			}
		}

		//dbg("sort");
		__profiling_edge_size+=tmpEdge.size();

		sort(tmpEdge.begin(), tmpEdge.end(), [](const _WeightEdge& a, const _WeightEdge& b){
			return a.truss > b.truss;	
		});
		__profiling_edge_time.Sleep();
		//dbg("Forest");

		__profiling_forest_time.Wake();
		T.reset(Edges.size2d(x) + 1);
		Forest[x].reserve(min(tmpEdge.size() * 2 + 1, (Edges.size2d(x))*2 + 1));
		int getEdgeCounter = 0;
		for (size_t i=0; i<tmpEdge.size(); i++){
			if (getEdgeCounter == nbrNeibr - 1)
				break;
			int& _x = tmpEdge[i].from, &_y = tmpEdge[i].to;

			if (T.FindSet(_x) != T.FindSet(_y)){

				dbg2(_x, _y);
				T.Union(_x, _y);
				Forest[x].push_back(tmpEdge[i]);
				Forest[x].push_back(_WeightEdge(_y,_x, tmpEdge[i].truss));
				getEdgeCounter++;
			}
		}
		//dbg3(x, Forest[x].size1d(), Forest[x].size2d());
		if (Forest[x].size2d()>0){
			Forest[x].sort( [](const _WeightEdge& a, const _WeightEdge& b){
				return a.from ==  b.from ? a.truss > b.truss : a.from < b.from;	
			}, [](const _WeightEdge& e){
				return e.from;	
			});
		}
		//Forest[x].shrink_to_fit();
		__profiling_forest_time.Sleep();

		__profiling_forest_size+=Forest[x].size();

	}
	timer.End("TCP build");
	global::stdinfo << "Build TCP : "<<info_TCP.getTick() << " ms\n";
	global::time_tcp = info_TCP.getTick();
	//change _edge
}
void QueryTCPGraph::getTrussGraph(const char* truss_file_name){
	cerr << " QueryTCPGraph getTrussGraph"<<endl;
	int m = 0;
	if (!FileHandler::testOpen(truss_file_name)) return ;

	FileHandler fd(truss_file_name, "rb");
	IO::ReadBlock(fd, &m, sizeof(m));
	_edge.resize(m);
	//output3(sizeof(size_t), sizeof(_edge[0]) * m, sizeof(sizeof(_edge[0]) * m));
	IO::ReadBlock(fd, _edge.data(), sizeof(_edge[0]) * m);
	int maxTrust(0);
	_nbrNode = 0;
	for (int i=0; i<m; i++){
		_nbrNode = max(_nbrNode, max(_edge[i].from, _edge[i].to));
		maxTrust = max(maxTrust, _edge[i].truss);
		if (i> m -10)
		{
			fprintf(stderr, "%d %d %d\n", _edge[i].from, _edge[i].to, _edge[i].truss);fflush(stderr);
		}
	}

	_nbrNode += 1;
	_nbrEdge = m;
	fprintf(stderr, "#nbrNode = %d, #nbrEdge = %d, maxTrust = %d\n", _nbrNode, _nbrEdge, maxTrust);

}		

void QueryTCPGraph::TCPQuery(int v0, int k, graph::Communities& c){
	static int qid = 0; qid++;
	c.reset();
	dbg3("query", v0, k);
	//if (Edges.size2d(v0)<0 || Edges.beginPt(v0)[0].truss < 4) //cmt: for btc test allowing k = 3
	if (Edges.size2d(v0)<=0)
		return ;

	//vector<int> curC;
	//dbg(Edges.size2d(v0));
	auto edges0 = Edges.data();
	auto v0_begin = Edges.beginPt(v0), v0_end =  Edges.beginPt(v0 + 1);

	for (auto u_it = v0_begin; u_it < v0_end ; u_it++){
		//dbg2(Edges.size1d(), v0);

		if (u_it->truss < k)
			break; //here assume Edges is sorted by truss, actually is not necessary.
		int gd_e_vu = u_it - edges0;
		//dbg2(gd_e_vu, gdVis.size());
		if (gdVis[gd_e_vu] >= qid)
			continue;
		//dbg2(gd_e_vu, gdVis.size());
		//curC.clear();
		//cerr << " push "<<Edges[gd_e_vu].from + 1<< " " <<  Edges[gd_e_vu].to + 1<< endl;
		q.reset();
		q.push_back(gd_e_vu);

		while (!q.empty()){
			int gd_e_xy = q.pop();
			int x = Edges[gd_e_xy].from;
			//dbg2(Edges.size1d(), x);
			int Td_x_y_out = gd_e_xy - Edges.beginInd(x);
			if (gdVis[gd_e_xy] >= qid)
				continue;
			TCPq.clear(Edges.size2d(x));
			TCPq.push(Td_x_y_out);
			while (!TCPq.empty()){
				int Td_x_y = TCPq.pop();
				int  gd_e_xy = Edges.beginInd(x) + Td_x_y;

				int g_e_xy = geID[gd_e_xy];
		
				if (gVis[g_e_xy] != qid){
					c.push_back( g_e_xy );
					gVis[g_e_xy] = qid;
				}
				//dbg2(gd_e_xy, gdVis.size());
				gdVis [ gd_e_xy ] = qid; 
				int gd_e_zx = gdeIDrev[gd_e_xy];
				if (gdVis[gd_e_zx] != qid ){
					q.push_back(gd_e_zx);

				}
				
				auto Td_x_y_begin = Forest[x].beginPt(Td_x_y), Td_x_y_end = Forest[x].beginPt(Td_x_y + 1);
				for (auto Td_e_yz_it = Td_x_y_begin; Td_e_yz_it < Td_x_y_end ; Td_e_yz_it++){
			
					if (Td_e_yz_it->truss < k)
						break;
					TCPq.push(Td_e_yz_it->to);

				}
			}
			//system("pause");
		}
		if ( c.size2d() > c.beginInd(c.size1d()))
			c.makeEnd();
		//cerr<<"\n";system("pause");
	}
	//dbg("finished");
}
