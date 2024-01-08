#include "EvolvingGraph.h"

namespace sinz{
	void DynamicGraph::loadFromTCPIndex(const char * fn, bool option_require_sort){

 		cerr << "loadFromTCPIndex "<<endl;
		if (fn && FileHandler::testOpen(fn)){
			FileHandler fdsup(fn, "rb");
			vector2d<_WeightEdge> _E;
			_E.read(fdsup);
			fdsup.close();
			_nbrNode = _E.size1d();
			Edges.resize(_nbrNode);
			ordMap.resize(_nbrNode);
			EIDrev.resize(_nbrNode);
			for (size_t x=0; x<_E.size1d(); x++)
			{        
				for (_WeightEdge* pt = _E.beginPt(x); pt < _E.beginPt(x + 1); pt++ ){
					int ts = pt->truss;
					NodeIndex y = pt->to;
					NodeIndex x_end = Edges[x].size();
					IndexType nbrEdges= Edgepos.size();
					Edges[x].push_back(_WeightNodeEdge(y, nbrEdges, ts ));
					EIDrev[x].push_back(0);
					if (x < y)
						Edgepos.push_back(make_pair((NodeIndex)x, (NodeIndex)x_end));
				}
			}

			output1("re_size");
			for (size_t x=0; x<Edges.size(); x++){
				if (x % 100000 == 0)
					fprintf(stderr, "\r%d", x);
				/*cannot sort*/
				Edges[x].shrink_to_fit();
				ordMap[x].resize(Edges[x].size());
				for (size_t yi = 0; yi< Edges[x].size(); yi++)
					ordMap[x][yi] = make_pair(Edges[x][yi].to, (NodeIndex)yi); 
				sort(ordMap[x].begin(), ordMap[x].end(), util::KeyValueCmp<NodeIndex, NodeIndex>);

			}
			
			for (size_t x=0; x<Edges.size(); x++){
				for (size_t yi = 0; yi< Edges[x].size(); yi++){
					IndexType eid = Edges[x][yi].id;
					NodeIndex y = Edges[x][yi].to;
					if (x == Edgepos[eid].first)
						Edgepos[eid].second = yi;
				}
			}

			for (size_t x=0; x<Edges.size(); x++){
				for (size_t yi = 0; yi< Edges[x].size(); yi++){
					
					NodeIndex y = Edges[x][yi].to;
					//output3(x, yi, y);
					if (x > y)
						continue;
					IndexType eid = Edges[x][yi].id;
				
					NodeIndex x_id_y = util::getValue(ordMap[y], (NodeIndex)(x));
					EIDrev[x][yi] = x_id_y;
					EIDrev[y][x_id_y] = yi;
					//output3(y, x_id_y, Edges[y].size());
					Edges[y][x_id_y].id = eid;  // correct the edgeID
					//output3(y, x_id_y, Edges[y].size());
				}
			}
			/*
			output1("ggg");
			for (size_t x = 0; x<Edges.size(); x++)
				EIDrev[x].shrink_to_fit();
			cout << "\n";
			for (int i=0; i< Edges.size(); i++){
				for (int j=0; j<Edges[i].size(); j++){
					cout << Edges[i][j].to<< ":"<<Edges[i][j].truss<< " ";
				}
				cout << "\n";
				for (int j=0; j<Edges[i].size(); j++){
					cout << ordMap[i][j].first<< ":" << ordMap[i][j].second<< " ";
				}
				cout << "\n";
			}
			*/
		}

	}
	void DynamicGraph::BuildTCPGraph(const char* fn){

		//#define ongoing_2013_11_28_hash
		bool option_loading = 0;
		if (fn && FileHandler::testOpen(fn) && FileHandler::testOpen(fn))
			option_loading = true;
		Timer timer(stderr);
		dbg2(_edge.size(), _nbrEdge);

		Forest.resize(Edges.size());
		if (true && fn && FileHandler::testOpen(fn)){

			FileHandler fd(fn, "rb");
			cerr <<"< Loading TCP Graph>"<<endl;
			for (int v = 0; v < Edges.size(); v++)
				Vector2dM<_WeightEdge, IndexType>::load_from_vector2d(Forest[v], fd); 
		}else {
			cerr << "BuildTCPGraphInside"<< endl;
			BuildTCPGraphInside();
			if (fn){
				FileHandler fd(fn, "wb");
				cout <<"Dumping TCP"<<endl;
				for (int v = 0; v < Forest.size(); v++){
					dbg(v);
					Vector2dM<_WeightEdge, IndexType>::dump_into_vector2d(Forest[v], fd);
					dbg(v);
				}
			}
		}
		return ;
		for (int i=0; i< Forest.size(); i++){
			util::showVV(Forest[i], [](const _WeightEdge& a){
				cerr << a.to << " " << a.truss;
			});
		}
		cerr << "build TCP finished\n";
	}
	

	IndexType DynamicGraph::findEdgeInForeset(NodeIndex x, NodeIndex y, NodeIndex z, bool isconvert){
			NodeIndex y_id = isconvert ? util::getValue(ordMap[x], y) : y;
			NodeIndex z_id = isconvert ? util::getValue(ordMap[x], z) : z;
			if (y_id >= Forest[x].size() )
				return IndexType(-1);
			if (z_id >= Forest[x].size() )
				return IndexType(-1);
			for (size_t i=0; i<Forest[x][y_id].size(); i++)
				if (Forest[x][y_id][i].to == z_id)
					return (IndexType)i;
			
			return IndexType(-1);
	}

	

	void DynamicGraph::flushTruss(vector<int>& old_truss, vector<IndexType>& ChangedEdges){
		if (old_truss.size() == 0){
			//init truss
			old_truss.resize(Edgepos.size());
			for (IndexType e_xy=0; e_xy<Edgepos.size(); e_xy++)
			{
				NodeIndex x = Edgepos[e_xy].first;
				NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
				old_truss[e_xy] = Edges[x][Edgepos[e_xy].second].truss;
			}	
		}
		else {
			//update truss of ChangedEdges
			for (IndexType e_xy_it=0; e_xy_it<ChangedEdges.size(); e_xy_it++){
				NodeIndex e_xy = ChangedEdges[e_xy_it];
				NodeIndex x = Edgepos[e_xy].first;
				NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
				old_truss[e_xy] = Edges[x][Edgepos[e_xy].second].truss;
			}			
		}
	}
	void DynamicGraph::buildForestFromEdges(NodeIndex x, vector<_WeightEdge>& tmpEdge, NodeIndex max_node_id){
		//this function rebuild the whole tree.
		// insertEdgesIntoExistingForest rebuild partial tree
		int nbrNeibr = min(Edges[x].size(), ordMap[x].size());
		static UnionSet T;
		sort(tmpEdge.end(), tmpEdge.end(), _WeightEdge::trussDec);
		T.reset(Edges[x].size() + 1);
		Forest[x].clear();
		Forest[x].resize(max_node_id + 1);
		int getEdgeCounter = 0;
		for (size_t i = 0; i<tmpEdge.size(); i++){
			if (getEdgeCounter == nbrNeibr - 1)
				break;
			int _x = tmpEdge[i].from, _y = tmpEdge[i].to;
			//output3(_x, _y, Edges[x].size());
			if (T.FindSet(_x) != T.FindSet(_y)){
				T.Union(_x, _y);
				//output2(_x, _y);
				Forest[x][_x].push_back(tmpEdge[i]);
				Forest[x][_y].push_back(_WeightEdge(_y, _x, tmpEdge[i].truss));
				getEdgeCounter++;
			}
		}
		 
		for (size_t _x = 0; _x< Forest[x].size(); _x++)
			sort(Forest[x][_x].begin(), Forest[x][_x].end(), _WeightEdge::trussDec);

	}
	void DynamicGraph::TriangleListingXY(NodeIndex x, NodeIndex y, vector<pair<NodeIndex, NodeIndex> >& out){
		out.clear();
		NodeIndex _x = x, _y = y;
		if (ordMap[_x].size() > ordMap[_y].size())
			swap(_x, _y);
		pair<NodeIndex, NodeIndex> *_y_begin = ordMap[_y].data();
		pair<NodeIndex, NodeIndex> *_y_end = _y_begin + ordMap[_y].size();

		for (auto z_it = ordMap[_x].begin(); z_it < ordMap[_x].end(); z_it++ ){
			if (_y_begin == _y_end)
				break;
			auto& e__xz= Edges[_x][z_it->second];
			int z_id_y = util::getValueInc(_y_begin, _y_end, e__xz.to);
			//int z_id_y = util::getValue(ordMap[_y], e__xz.to);
			if (z_id_y < 0)
				continue;
			out.push_back(((_x == x) ? (make_pair( z_it->second, (NodeIndex)z_id_y )) : (make_pair((NodeIndex)z_id_y , z_it->second ))));					
		}

	}
	
	void DynamicGraph::BuildTCPofNode(NodeIndex x){

		static vector<_WeightEdge> tmpEdge;
		static UnionSet T;

		int nbrNeibr = Edges[x].size();
		tmpEdge.clear();
		Forest[x].clear();
		_WeightNodeEdge *begin_x = Edges[x].data(),  *end_x = begin_x + Edges[x].size();

		for (auto y_it = ordMap[x].begin(); y_it < ordMap[x].end(); y_it++ ){
			NodeIndex y_id = y_it->second;
			NodeIndex _x  = x, _y = y_it->first;
			if (ordMap[_x].size() > ordMap[_y].size())
				swap(_x, _y);
			pair<NodeIndex, NodeIndex> *_y_begin = ordMap[_y].data();
			pair<NodeIndex, NodeIndex> *_y_end = _y_begin + ordMap[_y].size();

			for (auto z_it = ordMap[_x].begin(); z_it < ordMap[_x].end(); z_it++ ){
				if (_y_begin == _y_end)
					break;
				auto& e__xz= Edges[_x][z_it->second];
				int z_id_y = util::getValueInc(_y_begin, _y_end, e__xz.to);
				if (z_id_y < 0)
					continue;
				int w = min(min(Edges[_y][z_id_y].truss, e__xz.truss), Edges[x][y_id].truss);
				tmpEdge.push_back(_WeightEdge(_y == x ? z_id_y : z_it->second, y_id,  w));					
			}

		}

		std::sort(tmpEdge.begin(), tmpEdge.end(), [](const _WeightEdge& a, const _WeightEdge& b){
			return a.truss > b.truss;	
		});

		T.reset(Edges[x].size() + 1);
		NodeIndex forest_x_max_node_id = 0;
		for (size_t i = 0; i<tmpEdge.size(); i++){
			forest_x_max_node_id = max(forest_x_max_node_id, (NodeIndex)max(tmpEdge[i].from, tmpEdge[i].to));
			if (forest_x_max_node_id >= nbrNeibr - 1)
				break;
		}
		Forest[x].resize(forest_x_max_node_id + 1);
		int getEdgeCounter = 0;
		for (size_t i = 0; i<tmpEdge.size(); i++){
			if (getEdgeCounter == nbrNeibr - 1)
				break;
			int& _x = tmpEdge[i].from, &_y = tmpEdge[i].to;

			if (T.FindSet(_x) != T.FindSet(_y)){
				T.Union(_x, _y);
				//output2(_x, _y);
				Forest[x][_x].push_back(tmpEdge[i]);
				Forest[x][_y].push_back(_WeightEdge(_y, _x, tmpEdge[i].truss));
				getEdgeCounter++;
			}
		}

		for (size_t _x = 0; _x< Forest[x].size(); _x++)
			sort(Forest[x][_x].begin(), Forest[x][_x].end(), _WeightEdge::trussDec);

	}
	void DynamicGraph::unzipBelowLvlK(vector<vector<_WeightEdge> >& forest_x, int K, vector<_WeightEdge>& out){
		for (NodeIndex _x = 0; _x < forest_x.size(); _x++){
			if ( forest_x[_x].size() ==0 )
				continue;
			//output1(  forest_x[_x].size());
			NodeIndex j ;
			for (int j=0; j<forest_x[_x].size(); j++){
				NodeIndex _y = forest_x[_x][j].to;
				if (_x < _y){
					out.push_back(forest_x[_x][j]);
				}
				if (forest_x[_x][j].truss <= K){
					forest_x[_x][j] = forest_x[_x][forest_x[_x].size() -1];
					forest_x[_x].pop_back();
					j--;
				}
			}
		}
	}
	
	void DynamicGraph::BuildUnionFromForest(vector<vector<_WeightEdge> >& forest_x, UnionSet& T){
		
		for (NodeIndex _x = 0; _x < forest_x.size(); _x++){
			for (NodeIndex j = 0; j<forest_x[_x].size(); j++){
				NodeIndex _y = forest_x[_x][j].to;
				if (_x < _y){
					T.Union(_x, _y);
				}
			}
		}
	}
	void DynamicGraph::insertEdgesIntoExistingForest(NodeIndex x, vector<_WeightEdge>& tmpEdge){
		cout << " insertEdgesIntoExistingForest " << endl;
		int nbrNeibr = ordMap[x].size(); 
		int nbrAdj = Edges[x].size(); 
		static UnionSet T;
		sort(tmpEdge.end(), tmpEdge.end(), _WeightEdge::trussDec);
		T.reset(nbrAdj + 1); // id may larger than ordMap[x].size();
		//Forest[x].clear();	must unzip forest outsize.
		// Forest[x].resize(max_node_id + 1);
		int getEdgeCounter = 0;
		for (NodeIndex a = 0; a< Forest[x].size(); a++){
			for (NodeIndex b_i=0; b_i < Forest[x][a].size(); b_i++){
				NodeIndex b = Forest[x][a][b_i].to;
				if (b > a)
					continue;
				T.Union(a, b);
				getEdgeCounter++;
			}
		}
	
		for (size_t i = 0; i<tmpEdge.size(); i++){
			if (getEdgeCounter == nbrNeibr - 1)
				break;
			int _x = tmpEdge[i].from, _y = tmpEdge[i].to;
			if (max(_x, _y) >= Forest[x].size())
				Forest[x].resize(max(_x, _y) + 1);
			if (T.FindSet(_x) != T.FindSet(_y)){
				T.Union(_x, _y);
				//output2(_x, _y);
				Forest[x][_x].push_back(tmpEdge[i]);
				Forest[x][_y].push_back(_WeightEdge(_y, _x, tmpEdge[i].truss));
				getEdgeCounter++;
			}
		}
		 /*
		for (size_t _x = 0; _x< Forest[x].size(); _x++)
			sort(Forest[x][_x].begin(), Forest[x][_x].end(), _WeightEdge::trussDec);
		*/
	}
	void DynamicGraph::displayForest(NodeIndex x){
		output1(x);
		for (int i=0; i<Forest[x].size(); i++){
			cout << i << ": ";
			for (int j=0; j<Forest[x][i].size(); j++){
				cout << Forest[x][i][j].to<<" ";
			}
			cout <<endl;
		}
	}
};
