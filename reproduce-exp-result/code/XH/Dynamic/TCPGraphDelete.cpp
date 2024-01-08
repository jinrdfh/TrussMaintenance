#include"EvolvingGraph.h"

namespace sinz{
	void DeleteGraph::resortTruss(vector<IndexType>& ChangedEdges){
		static vector<int> resorted_flag(Edges.size(), 0);
		static int resorted_true = 0;
		resorted_true ++;
		if (ordTruss.size() == 0 ){
			ordTruss.resize(Edges.size());
			for (size_t  i= 0; i < Edges.size(); i++){
				ordTruss.reserve(Edges[i].size());
				for (size_t j = 0; j<Edges[i].size();j++)
					ordTruss[i].push_back(make_pair(Edges[i][j].truss, Edges[i][j].to));
				ordTruss.shrink_to_fit();
				sort(ordTruss[i].begin(), ordTruss[i].end());
			}
			return ;
		}
		for (int i=0; i<ChangedEdges.size(); i++){
			NodeIndex x = Edgepos[ChangedEdges[i]].first;
			auto& e_xy = getEdge(ChangedEdges[i]);
			NodeIndex y = e_xy.to;
			while ( x != NodeIndex(-1)){
				if (resorted_flag[x] != resorted_true){
					for (int k = 0; k<ordTruss[x].size(); k++){
						if (Edges[x][ordTruss[x][k].second].truss != ordTruss[x][k].first)
						{
							ordTruss[x][k].first = Edges[x][ordTruss[x][k].second].truss ;
							break;
						}
					}
					sort(ordTruss[x].begin(), ordTruss[x].end());
					resorted_flag[x] = resorted_true;
				}
				x = y ;
				y = NodeIndex(-1);
			}
		}
	}
	void DeleteGraph::TCPUpdateofMultiDeleteMain(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl){
		//vector<IndexType> ChangedEdges; 
		Timer timer;
		size_t nbrTotalChangeEdge= 0;
		
		vector<pair<NodeIndex, NodeIndex> > triangles;
		vector<IndexType> all_updated_edges;
		vector<int> old_truss(Edgepos.size());
		for (IndexType e_xy=0; e_xy<Edgepos.size(); e_xy++)
		{
			NodeIndex x = Edgepos[e_xy].first;
			NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
			old_truss[e_xy] = Edges[x][Edgepos[e_xy].second].truss;
		}
		timer.Start();
		size_t removeQuerySize = 0;
		size_t connectQuerySize= 0;
		int info_nbr_Edge_update = 0;
		for (auto it_delete = delete_edges.begin(); it_delete != delete_edges.end(); it_delete++){
			if ((it_delete - delete_edges.begin() ) % 10==0)
				cerr << "\r\t\t"<< it_delete - delete_edges.begin();	
			UpdateTrussofDelete2(it_delete->first, it_delete->second, all_updated_edges);
			nbrTotalChangeEdge += all_updated_edges.size();
			//for (int i=0; i<ChangedEdges.size(); i++)
			//	all_updated_edges.push_back(ChangedEdges[i]);
			sort(all_updated_edges.begin(), all_updated_edges.end());
			all_updated_edges.resize(unique(all_updated_edges.begin(), all_updated_edges.end()) - all_updated_edges.begin());
			//vector<pair<NodeIndex, IndexType> > forests_new_edges;
			vector<pair<NodeIndex, pair<NodeIndex, NodeIndex > > > connectQuery;
			vector<pair<NodeIndex, pair<NodeIndex, NodeIndex > > > updateQuery;
			vector< pair<NodeIndex, NodeIndex > > removeQuery;
			unordered_set<NodeIndex> rebuild_nodes;
			for (int i=0; i<all_updated_edges.size(); i++){

				NodeIndex x = Edgepos[all_updated_edges[i]].first;
				auto& e_xy = getEdge(all_updated_edges[i]);
				NodeIndex y = e_xy.to;
				//output3(x, y, Edges.size());
				TriangleListingXY(x, y, triangles);
				for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
				{
					//output2(e_xy.id, Edgepos.size());
				
					auto& tr = triangles[tr_id];
					NodeIndex w = Edges[x][tr.first].to;
					//output2(Edges[x].size(), tr.first);
					//output2(Edges[y].size(), tr.second);
					int min_w_new =min(min(Edges[x][tr.first].truss, Edges[y][tr.second].truss), e_xy.truss);
					int min_w_old =min(min( old_truss[Edges[x][tr.first].id], 
											old_truss[Edges[y][tr.second].id]), 
											IndexType(-1) == e_xy.id ? 2147483647: old_truss[e_xy.id]);
						if (e_xy.id == IndexType(-1)){
							removeQuery.push_back(make_pair(x, y));
							removeQuery.push_back(make_pair(y, x));
							connectQuery.push_back(make_pair(w, make_pair(x, y)));
							continue;
						}
						if ((min_w_new != min_w_old)){
							IndexType tmp ;
							//output3(w, x, y);
							updateQuery.push_back(make_pair(w, make_pair(x, y)));
		
							updateQuery.push_back(make_pair(x, make_pair(w, y)));
		
							updateQuery.push_back(make_pair(y, make_pair(x, w)));

						}				
				}
			}
			bool option_case_1_rebuild = true;
			for (auto it = removeQuery.begin(); it < removeQuery.end(); it++){
				NodeIndex x = it->first, y = it->second;
				if (rebuild_nodes.count(x) == 0){
					if (option_case_1_rebuild){
						BuildTCPofNode(x);
						rebuild_nodes.insert(x);
					}
					else 
						removeNodeFromForestX(x, y);
					
					removeQuerySize++;
				}
			}
			
			//timer.End("remove query");
			
			for (auto it = connectQuery.begin(); it < connectQuery.end(); it++){
				NodeIndex w = it->first, x = it->second.first, y = it->second.second;
				if (rebuild_nodes.count(w) >= 0){
					IndexType tmp = findEdgeInForeset(w, x, y, true);
					if (tmp == IndexType(-1))
						continue;
					removeEdgeFromForest(w, x, y);
					//rebuild_nodes.insert(w);
					connectQuerySize++;
				}
			}
			
			//timer.End("connectQuery");
			//printf("%d Node update TCP, %lf\n", rebuild_nodes.size(), timer.getTick());
		
			
			for (auto q_it = updateQuery.begin(); q_it < updateQuery.end(); q_it++){
				if (rebuild_nodes.count(q_it->first) > 0)
					continue;
				if (updateDeleteEdgeFromForest(q_it->first, q_it->second.first,  q_it->second.second))
					info_nbr_Edge_update++;
			}
		}
		output1(removeQuerySize)
		output1(connectQuerySize);
		output1(info_nbr_Edge_update);
		timer.End("update truss time:");
		printf("%d nbrTotalChangeEdge\n", nbrTotalChangeEdge);
		timer.Start();


		//timer.End("find candidate tcp");
		//timer.Start();

		//printf("%d Node update TCP, %d Edge's TCP update\n", rebuild_nodes.size(), info_nbr_Edge_update);
		//timer.End("tcp time:");		
		
		/*
		cerr << "updating Truss\n";
		vector<IndexType> ChangedEdges; 
		Timer timer;
		size_t nbrTotalChangeEdge= 0;
		unordered_set<NodeIndex> rebuild_nodes;
		vector<pair<NodeIndex, NodeIndex> > triangles;
		
		//vector<IndexType> all_updated_edges;
		vector<int> old_truss(Edgepos.size());
		for (IndexType e_xy=0; e_xy<Edgepos.size(); e_xy++)
		{
			NodeIndex x = Edgepos[e_xy].first;
			NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
			old_truss[e_xy] = Edges[x][Edgepos[e_xy].second].truss;
		}
		resortTruss(ChangedEdges);
		timer.Wake();
		int nbrNodeUpdated = 0;
		for (auto it = delete_edges.begin(); it != delete_edges.end(); it++){
			if ((it - delete_edges.begin())%10==0)
				fprintf(stderr, "\r\t\t\t\tTruss Update and TCP Update %d", it - delete_edges.begin());
			//output2(it->first, it->second);
			UpdateTrussofDelete(it->first, it->second, ChangedEdges);
			timer.Sleep();
			resortTruss(ChangedEdges); // out of timer ....
			timer.Wake();
			nbrTotalChangeEdge += ChangedEdges.size();
			//cout << nbrTotalChangeEdge << endl;
			for (int i=0; i<ChangedEdges.size(); i++){

				NodeIndex x = Edgepos[ChangedEdges[i]].first;
				auto& e_xy = getEdge(ChangedEdges[i]);
				NodeIndex y = e_xy.to;
				//output3(x, y, Edges.size());
				TriangleListingXY(x, y, triangles);
				for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
				{
					//output2(e_xy.id, Edgepos.size());
				
					auto& tr = triangles[tr_id];
					NodeIndex w = Edges[x][tr.first].to;
					//output2(Edges[x].size(), tr.first);
					//output2(Edges[y].size(), tr.second);
					int min_w_new =min(min(Edges[x][tr.first].truss, Edges[y][tr.second].truss), e_xy.truss);
					int min_w_old =min(min( old_truss[Edges[x][tr.first].id], 
											old_truss[Edges[y][tr.second].id]), 
											IndexType(-1) == e_xy.id ? 2147483647: old_truss[e_xy.id]);
					if (e_xy.id == IndexType(-1)){
						removeNodeFromForestX(x, y);
						removeNodeFromForestX(y, x);
						IndexType tmp ;
						tmp = findEdgeInForeset(w, x, y);
						if (tmp != IndexType(-1)){
							//output1("==");
							//output3(w, x, y);
							removeEdgeFromForest(w, x, tmp);
							nbrNodeUpdated++;
						}
						continue;
					}
					if ((min_w_new != min_w_old)){
						IndexType tmp ;
						//output3(w, x, y);
						tmp = findEdgeInForeset(w, x, y);
						if (tmp != IndexType(-1)){
							updateDeleteEdgeFromForest(w, x, tmp, min_w_new );
							nbrNodeUpdated++;
						}
						//output3(x, w, y);
						if (util::getValue(ordMap[w], y) != NodeIndex(-1)){
							tmp = findEdgeInForeset(x, w, y);
							if (tmp != IndexType(-1)){
								updateDeleteEdgeFromForest(x, w, tmp, min_w_new);
								nbrNodeUpdated++;
							}
						}
						//output3(y, x, w);
						if (util::getValue(ordMap[x], w) != NodeIndex(-1)){
							tmp = findEdgeInForeset(y, x, w);
							if (tmp != IndexType(-1)){
								updateDeleteEdgeFromForest(y, x, tmp, min_w_new);
								nbrNodeUpdated++;
							}
						}
					}
				}
			//	cout << nbrNodeUpdated<< endl;
			}
		}
		timer.Sum("batch TCP delete");
		*/
	}
	void DeleteGraph::removeEdgeFromForest(NodeIndex x, NodeIndex y, NodeIndex z, int w, bool not_exist ){
		/*output1(x);
		for (int i=0; i<Forest[x].size(); i++){
			cout << i << ": ";
			for (int j=0; j<Forest[x][i].size(); j++){
				cout << Forest[x][i][j].to<<" ";
			}
			cout <<endl;
		}
		*/
		//cout <<"removeEdgeFromForest"<<x<< " "<< y<< endl;
		auto& Forest_x = Forest[x];
		NodeIndex y_id = util::getValue(ordMap[x], y);
		NodeIndex z_id = util::getValue(ordMap[x], z);
		NodeIndex y_id_f_y = this->findEdgeInForeset(x,z_id,y_id, false);
		NodeIndex z_id_f_y = this->findEdgeInForeset(x,y_id,z_id, false);
		static vector<pair<int, NodeIndex> > edges_z, edges_y; //(id, pre)
		int truss_of_this_edge = Forest[x][y_id][z_id_f_y].truss;
		if ( not_exist == false ){
			Forest[x][y_id][z_id_f_y] = Forest[x][y_id][Forest[x][y_id].size() - 1];
			Forest[x][z_id][y_id_f_y] = Forest[x][z_id][Forest[x][z_id].size() - 1];
			Forest[x][y_id].pop_back();
			Forest[x][z_id].pop_back();
		}
		edges_z.clear();
		edges_y.clear();

	
		this->expandLvlK_e(x, z_id, 2, edges_z);
		this->expandLvlK_e(x, y_id, 2, edges_y);
		
		int max_weight = truss_of_this_edge;
		NodeIndex best_zp_id = -1, best_yp_id =-1;
		findBridgePrune(x, edges_z, edges_y, best_zp_id, best_yp_id, max_weight, true);
		if (max_weight < 0)
			return ;
	
		insertEdgeIntoForest( x,  best_zp_id, best_yp_id ,  max_weight);
	
	}
	_WeightEdge DeleteGraph::connectTwoComponent(NodeIndex x, NodeIndex y_id, NodeIndex z_id, int w){
		auto& Forest_x = Forest[x];
		NodeIndex z = Edges[x][z_id].to;

		static vector<pair<NodeIndex, NodeIndex> > edges_z, edges_y; //(id, pre)
		edges_z.clear();
		edges_y.clear();
		static vector<int> truss_z, truss_y;
		truss_z.clear();
		truss_y.clear();
	
		this->expandLvlK(x, z_id, w + 1, edges_z, truss_z);
		this->expandLvlK(x, y_id, w + 1, edges_y, truss_y);
	
		int max_weight = -1;
		NodeIndex best_zp_id, best_yp_id;
		findBridge(x, edges_z, edges_y, best_zp_id, best_yp_id, max_weight);
		if (max_weight < 0)
			return _WeightEdge(-1, -1, -1);
	
		insertEdgeIntoForest( x,  best_zp_id, best_yp_id ,  max_weight);
		return _WeightEdge(best_zp_id, best_yp_id, max_weight);
	}
	bool DeleteGraph::updateDeleteEdgeFromForest(NodeIndex x, NodeIndex y, NodeIndex z){
		//cout << "updateDeleteEdgeFromForest" <<endl;
		/*
		if (x == 1623)
			displayForest(x);
			*/
		auto& Forest_x = Forest[x];
		NodeIndex y_id = util::getValue(ordMap[x], y);
		NodeIndex z_id = util::getValue(ordMap[x], z);
		if (util::getValue(ordMap[y], z) == NodeIndex(-1) )
			return false;
		if (y_id == NodeIndex(-1) || z_id == NodeIndex(-1))
			return false;
		NodeIndex z_id_f_y = this->findEdgeInForeset(x, y_id, z_id, false);
		if (z_id_f_y  == NodeIndex(-1))
			return false;
		NodeIndex y_id_f_z = this->findEdgeInForeset(x, z_id ,y_id, false);
		if (y_id_f_z  == NodeIndex(-1))
			return false;
		//output2(z_id, y_id_f_z);
		//output2(y_id, z_id_f_y);
		int w = Forest[x][y_id][z_id_f_y].truss;
		Forest[x][y_id][z_id_f_y].truss --;
		Forest[x][z_id][y_id_f_z].truss --;
		
		static vector<pair<int, NodeIndex> > edges_z, edges_y; //(id, pre)
		edges_z.clear();
		edges_y.clear();
		/*
		output2(x, z_id);
		output2(x, y_id);
		*/
		this->expandLvlK_e(x, z_id, w, edges_z);
		this->expandLvlK_e(x, y_id, w, edges_y);
		//output3("fi",x, z_id);
		int max_weight = -1;
		NodeIndex best_zp_id = -1, best_yp_id = -1;
		findBridgePrune(x, edges_z, edges_y, best_zp_id, best_yp_id, max_weight, false);

		if (max_weight < w)
			return true;
		removeEdgefromForest(x, y_id, z_id_f_y);
		removeEdgefromForest(x, z_id, y_id_f_z);
		insertEdgeIntoForest( x,  best_zp_id, best_yp_id ,  max_weight);
		/*
		if (x == 1623){
			displayForest(x);
			output2(best_zp_id, best_yp_id);
			
			system("pause");
		}
		*/
		return true;
	}

	void DeleteGraph::removeNodeFromForestX(NodeIndex x, NodeIndex y){
		//output3("removeNodeFromForestX", x, y);
		static size_t profiling_nbs = 0;
		static size_t profiling_edges_x_size = 0;
		static size_t profiling_round = 0;
		static vector<_WeightEdge> tmpEdge;
		static UnionSet T;
		static vector<int> inTree;
		static int inTree_true = 0;
		inTree_true++;
		if (inTree.size() < Edges[x].size())
			inTree.resize( Edges[x].size());
		int nbrNeibr = ordMap[x].size();
		int nbrAdj = Edges[x].size();
		tmpEdge.clear();
		
		//_WeightNodeEdge *begin_x = Edges[x].data(),  *end_x = begin_x + Edges[x].size();
		//int  = util::getValue(ordMap[x], y);
		int y_id = -1;
		for (size_t i = 0; i<Edges[x].size(); i++){
			if (Edges[x][i].to == y) {
				y_id = i;
				break;
			}
		}
		vector<pair<int, NodeIndex> > nbs;
		//output2(x, y_id);
		expandLvlK_e(x , y_id,  3, nbs);
				
		for (auto y_it = nbs.begin(); y_it < nbs.end(); y_it++ ){
			Forest[x][y_it->second].clear();
			inTree[y_it->second] = inTree_true;
		}
		T.reset(Edges[x].size() + 1 );
		int getEdgeCounter = 0;
		for (size_t f_t_id = 0; f_t_id < Forest[x].size(); f_t_id++){
			auto& ft = Forest[x][f_t_id];
			for (int j=0; j< ft.size(); j++){
				if (inTree[ft[j].to] == inTree_true){
					removeEdgefromForest(x, f_t_id, j);
					j--;
				}
				else {
					if (f_t_id < ft[j].to){
						T.Union(f_t_id, ft[j].to);
						getEdgeCounter++;
					}
				}
			}
		}
		//output2("removed", Forest[x].size());
		for (auto y_it = nbs.begin() + 1; y_it < nbs.end(); y_it++ ){
			//should ignore y_it.
			NodeIndex y_id = y_it->second;
			/*
			if (y_id > Edges[x].size()){
				output2(x, y_id);
				system("pause");
			}
			*/
			NodeIndex _x  = x, _y = Edges[x][y_it->second].to;
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
				if (x ==_y && z_id_y >= Forest[_y].size() ){
					output2(z_id_y, Forest[_y].size());
					output1(Edges[_y].size());
					output1(Edges[x].size());
					system("pause");
				}
				int w = min(min(Edges[_y][z_id_y].truss, e__xz.truss), Edges[x][y_id].truss);
				tmpEdge.push_back(_WeightEdge(_y == x ? z_id_y : z_it->second, y_id,  w));
			}

		}
		
		std::sort(tmpEdge.begin(), tmpEdge.end(), [](const _WeightEdge& a, const _WeightEdge& b){
			return a.truss > b.truss;	
		});

		
		NodeIndex forest_x_max_node_id = 0;
		for (size_t i = 0; i<tmpEdge.size(); i++){
			forest_x_max_node_id = max(forest_x_max_node_id, (NodeIndex)max(tmpEdge[i].from, tmpEdge[i].to));
			if (forest_x_max_node_id >= nbrAdj - 1)
				break;
		}
		//Forest[x].resize(forest_x_max_node_id + 1);
		
		for (size_t i = 0; i<tmpEdge.size(); i++){
			if (getEdgeCounter == nbrNeibr - 1)
				break;
			int& _x = tmpEdge[i].from, &_y = tmpEdge[i].to;

			if (T.FindSet(_x) != T.FindSet(_y)){
				T.Union(_x, _y);
				if (max(_x, _y) >= Forest[x].size() ){
					output3(_x, _y, Forest[x].size());
					output3(_x, x, Edges[x].size());
					system("pause");
				}
				Forest[x][_x].push_back(tmpEdge[i]);
				Forest[x][_y].push_back(_WeightEdge(_y, _x, tmpEdge[i].truss));
				getEdgeCounter++;
			}
		}
		profiling_nbs += nbs.size();
		profiling_edges_x_size += Edges[x].size();
		if (++profiling_round % 100 == 0){
			//output3(getEdgeCounter, nbrNeibr , tmpEdge.size());
			output2(profiling_nbs, profiling_edges_x_size);
		}
		//for (size_t _x = 0; _x< Forest[x].size(); _x++)
		//	sort(Forest[x][_x].begin(), Forest[x][_x].end(), _WeightEdge::trussDec);

	}
	void DeleteGraph::findBridge(NodeIndex x, 
		vector<pair<NodeIndex, NodeIndex> > & edges_z, 
		vector<pair<NodeIndex, NodeIndex>> & edges_y, 
		NodeIndex& best_zp_id, NodeIndex& best_yp_id, int& max_weight )
	{
		int optimal_weight = -1;
		if (max_weight >= 0)
			optimal_weight = max_weight;
		bool swap_flag = false;
		if (edges_z.size() > edges_y.size())
		{
			swap_flag = true;
			edges_z.swap(edges_y);
		}
#define ongoing_triagnle_listing
#ifdef ongoing_triagnle_listing
		
		static vector<int> inY;
		static int updateid = 0;
		static vector<pair<NodeIndex,NodeIndex > >triangles;
		updateid++;
		if (inY.size() < Edges[x].size())
			inY.resize(Edges[x].size());
		for (int i=0; i<edges_y.size(); i++)
			inY[edges_y[i].first] = updateid;
		//output1(inY.size());
		for (auto zp_it = edges_z.begin() ; zp_it < edges_z.end(); zp_it++){
			NodeIndex z_id = zp_it->first ;
			NodeIndex zp = Edges[x][z_id].to;
			TriangleListingXY(x, zp, triangles);
			//output1(triangles.size());
			for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
			{
				auto& tr = triangles[tr_id];
				NodeIndex w = Edges[x][tr.first].to;
				if (inY[tr.first] != updateid)
					continue;
				int tw =min(min(Edges[x][tr.first].truss, Edges[zp][tr.second].truss), 
					Edges[x][z_id].truss);
				if (tw > max_weight){
					best_zp_id = z_id;
					best_yp_id = tr.first;
					max_weight = tw;
	
				}
				if (tw == optimal_weight)
					break;
			}
		}
#else
		for (auto zp_it = edges_z.begin() ; zp_it < edges_z.end(); zp_it++){
			// TriangleListingXY(x, Edges[x][zp_it->first].to, triangles);
			NodeIndex zp = Edges[x][zp_it->first].to;
			for (auto yp_it = edges_y.begin() ; yp_it < edges_y.end(); yp_it++){
				NodeIndex yp_id = yp_it->first;
				int yp_id_z = util::getValue(ordMap[zp], Edges[x][yp_id].to);
				if (yp_id_z < 0 )
					continue;
				int tw = min(Edges[zp][yp_id_z].truss, 
					min(Edges[x][zp_it->first].truss, Edges[x][yp_it->first].truss));

				if (tw > max_weight){
					best_zp_id = zp_it->first;
					best_yp_id = yp_it->first;
					max_weight = tw;
	
				}
				if (tw == optimal_weight)
					break;
			}
		}

		
#endif
		if (swap_flag){
			swap(best_zp_id, best_yp_id);
		}
	}

	void DeleteGraph::findBridgePrune(NodeIndex x, 
		vector<pair<int, NodeIndex> > & edges_z, 
		vector<pair<int, NodeIndex>> & edges_y, 
		NodeIndex& best_zp_id, NodeIndex& best_yp_id, int& max_weight, bool case_2 )
	{
		//cout << "findBridgePrune"<<endl;
		//assert(false);
		int optimal_weight = -1;
		if (max_weight >= 0)
			optimal_weight = max_weight;
		if (case_2)
			max_weight = -1;
		bool swap_flag = false;
		if (edges_z.size() > edges_y.size())
		{
			swap_flag = true;
			edges_z.swap(edges_y);
		}
		if (case_2 == true){
			sort(edges_z.begin(), edges_z.end(), util::KeyValueCmp<int, NodeIndex>);
		}
		else {
			sort(edges_z.begin(), edges_z.end(), util::KeyValueCmpGreater<int, NodeIndex>);
		}
//	sort(edges_y.begin(), edges_y.end(), util::KeyValueCmp<int, NodeIndex>());
		static vector<int> inY;
		static int updateid = 0;
		static vector<pair<NodeIndex,NodeIndex > >triangles;
		updateid++;
		if (inY.size() < Edges[x].size())
			inY.resize(Edges[x].size());
		for (int i=0; i<edges_y.size(); i++)
			inY[edges_y[i].second] = updateid;
		//output1(inY.size());
		/*
		if (x == 1623){
			for (int i=0; i<edges_y.size(); i++)
				cout << edges_y[i].second << " ";
			cout << endl;
		}
		*/
		for (auto zp_it = edges_z.begin() ; zp_it < edges_z.end(); zp_it++){

			NodeIndex z_id = zp_it->second ;
			if (case_2 == true && zp_it->first == max_weight)
				continue;
			if (case_2 == false &&  zp_it->first < max_weight)
				break;
			NodeIndex zp = Edges[x][z_id].to;
			TriangleListingXY(x, zp, triangles);
			/*
			if (x==1623)
				output3(z_id, x, zp);
			*/
			//output1(triangles.size());
			for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
			{
				auto& tr = triangles[tr_id];
				NodeIndex w = Edges[x][tr.first].to;
				if (inY[tr.first] != updateid)
					continue;
	
				int tw =min(min(Edges[x][tr.first].truss, Edges[zp][tr.second].truss), 
					Edges[x][z_id].truss);
				//if (x==1623)output3(z_id, tr.first, inY[tr.first]);
				if (tw > max_weight){
					best_zp_id = z_id;
					best_yp_id = tr.first;
					max_weight = tw;
	
				}
				if (tw == optimal_weight)
					break;
			}
		}

		if (swap_flag){
			swap(best_zp_id, best_yp_id);
		}
	}
	void DeleteGraph::UpdateTCPofDelete(vector<pair<NodeIndex, NodeIndex> >& delete_edges, string fn, int silent_lvl){
		//batch update
		//displayForest(5);
		//cerr << "updating Truss\n";
		vector<IndexType> ChangedEdges; 
		Timer timer;
		size_t nbrTotalChangeEdge= 0;
		unordered_set<NodeIndex> rebuild_nodes;
		vector<pair<NodeIndex, NodeIndex> > triangles;
		vector<IndexType> all_updated_edges;
		vector<int> old_truss(Edgepos.size());
		for (IndexType e_xy=0; e_xy<Edgepos.size(); e_xy++)
		{
			NodeIndex x = Edgepos[e_xy].first;
			NodeIndex y = Edges[x][Edgepos[e_xy].second].to;
			old_truss[e_xy] = Edges[x][Edgepos[e_xy].second].truss;
		}
		timer.Start();
		for (auto it = delete_edges.begin(); it != delete_edges.end(); it++){
			if ((it - delete_edges.begin() ) % 10==0)
				cerr << "\r\t\t"<< it - delete_edges.begin();	
			UpdateTrussofDelete2(it->first, it->second, ChangedEdges);
			nbrTotalChangeEdge += ChangedEdges.size();
			for (int i=0; i<ChangedEdges.size(); i++)
				all_updated_edges.push_back(ChangedEdges[i]);
		}
		timer.End("update truss time:");
		printf("%d nbrTotalChangeEdge\n", nbrTotalChangeEdge);
		timer.Start();

		sort(all_updated_edges.begin(), all_updated_edges.end());
		all_updated_edges.resize(unique(all_updated_edges.begin(), all_updated_edges.end()) - all_updated_edges.begin());
		//vector<pair<NodeIndex, IndexType> > forests_new_edges;
		vector<pair<NodeIndex, pair<NodeIndex, NodeIndex > > > connectQuery;
		vector<pair<NodeIndex, pair<NodeIndex, NodeIndex > > > updateQuery;
		vector< pair<NodeIndex, NodeIndex > > removeQuery;
		for (int i=0; i<all_updated_edges.size(); i++){

			NodeIndex x = Edgepos[all_updated_edges[i]].first;
			auto& e_xy = getEdge(all_updated_edges[i]);
			NodeIndex y = e_xy.to;
			//output3(x, y, Edges.size());
			TriangleListingXY(x, y, triangles);
			for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
			{
				//output2(e_xy.id, Edgepos.size());
				
				auto& tr = triangles[tr_id];
				NodeIndex w = Edges[x][tr.first].to;
				//output2(Edges[x].size(), tr.first);
				//output2(Edges[y].size(), tr.second);
				int min_w_new =min(min(Edges[x][tr.first].truss, Edges[y][tr.second].truss), e_xy.truss);
				int min_w_old =min(min( old_truss[Edges[x][tr.first].id], 
										old_truss[Edges[y][tr.second].id]), 
										IndexType(-1) == e_xy.id ? 2147483647: old_truss[e_xy.id]);
					if (e_xy.id == IndexType(-1)){
						removeQuery.push_back(make_pair(x, y));
						removeQuery.push_back(make_pair(y, x));
						connectQuery.push_back(make_pair(w, make_pair(x, y)));
						continue;
					}
					if ((min_w_new != min_w_old)){
						IndexType tmp ;
						//output3(w, x, y);
						updateQuery.push_back(make_pair(w, make_pair(x, y)));
		
						updateQuery.push_back(make_pair(x, make_pair(w, y)));
		
						updateQuery.push_back(make_pair(y, make_pair(x, w)));

					}				
			}
		}
		timer.End("find candidate tcp");
		timer.Start();
		size_t removeQuerySize = 0;
		bool option_case_1_rebuild = false;
		output1(option_case_1_rebuild);
		for (auto it = removeQuery.begin(); it < removeQuery.end(); it++){
			NodeIndex x = it->first, y = it->second;
			if (rebuild_nodes.count(x) == 0){
				if (option_case_1_rebuild){
					BuildTCPofNode(x);
					rebuild_nodes.insert(x);
				}
				else 
					removeNodeFromForestX(x, y);

				removeQuerySize++;
			}
		}
		output1(removeQuerySize)
		timer.End("remove query");
		size_t connectQuerySize= 0;
		for (auto it = connectQuery.begin(); it < connectQuery.end(); it++){
			NodeIndex w = it->first, x = it->second.first, y = it->second.second;
			if (rebuild_nodes.count(w) >= 0){
				IndexType tmp = findEdgeInForeset(w, x, y, true);
				if (tmp == IndexType(-1))
					continue;
				removeEdgeFromForest(w, x, y);
				//rebuild_nodes.insert(w);
				connectQuerySize++;
			}
		}
		output1(connectQuerySize);
		timer.End("connectQuery");
		printf("%d Node adaptive rebuild TCP, %lf\n", rebuild_nodes.size(), timer.getTick());
		
		int info_nbr_Edge_update = 0;
		for (auto q_it = updateQuery.begin(); q_it < updateQuery.end(); q_it++){
			if (rebuild_nodes.count(q_it->first) > 0)
				continue;
			if (updateDeleteEdgeFromForest(q_it->first, q_it->second.first,  q_it->second.second))
				info_nbr_Edge_update++;
		}
		printf("%d Node adaptive rebuild TCP, %d Edge's TCP update\n", rebuild_nodes.size(), info_nbr_Edge_update);
		timer.End("tcp time:");
	}
};
	