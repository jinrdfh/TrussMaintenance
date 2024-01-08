#include "EvolvingGraph.h"

namespace sinz{
	void InsertGraph::UpdateTCPofInsertOneByOne(
		vector< IndexType >& all_updated_edges, 
		vector<int> & old_truss)
	{
		static vector<pair<NodeIndex, NodeIndex> >triangles;
		static vector<pair<NodeIndex, IndexType> > forests_new_edges;
		static vector<pair<NodeIndex, IndexType> > new_edge_induced_edges;
		triangles.clear();
		forests_new_edges.clear();
		new_edge_induced_edges.clear();
		//begin[unique]
		sort(all_updated_edges.begin(), all_updated_edges.end()); 
		all_updated_edges.resize(unique(all_updated_edges.begin(), all_updated_edges.end()) - all_updated_edges.begin());
		//end[]
		old_truss.resize(Edgepos.size(), 2147483647); //resize the edgesize
		
		for (int i=0; i<all_updated_edges.size(); i++){
			IndexType e_xy_id = all_updated_edges[i];
			NodeIndex x = Edgepos[e_xy_id].first;
			NodeIndex y = Edges[x][Edgepos[e_xy_id].second].to;
			auto& e_xy = Edges[Edgepos[e_xy_id].first][Edgepos[e_xy_id].second];
			TriangleListingXY(x, y, triangles);

			//output3(x, y, triangles.size());
			for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
			{
				auto& tr = triangles[tr_id];
				NodeIndex w = Edges[x][tr.first].to;
				//output3(x , y, w);
				int min_w_new =min(min(Edges[x][tr.first].truss, Edges[y][tr.second].truss), e_xy.truss);
				int min_w_old =min(min( old_truss[Edges[x][tr.first].id], 
										old_truss[Edges[y][tr.second].id]), 
										old_truss[e_xy.id]);
				if (old_truss[e_xy.id] == 2147483647){
					new_edge_induced_edges.push_back(make_pair(x, Edges[y][tr.second].id));
					new_edge_induced_edges.push_back(make_pair(y, Edges[x][tr.first].id));
					forests_new_edges.push_back(make_pair(w, e_xy.id));
				}			
				if ((min_w_new != min_w_old) || (old_truss[e_xy.id] == 2147483647)){
					forests_new_edges.push_back(make_pair(w, e_xy.id));
					forests_new_edges.push_back(make_pair(x, Edges[y][tr.second].id));
					forests_new_edges.push_back(make_pair(y, Edges[x][tr.first].id));
				}
			}
		}
		pair<NodeIndex, IndexType> *from = new_edge_induced_edges.data();
		pair<NodeIndex, IndexType> *end = new_edge_induced_edges.data() + new_edge_induced_edges.size();
		int info_nbr_Node = 0;
		//cout << "Edges: "<< end - from << endl;
		
		for (pair<NodeIndex, IndexType>* pt = from + 1; pt < end + 1; pt++){
			if (pt == end || pt->first != (pt-1)->first){
				updateForestOfBatchInsert(from, pt);
				from = pt;
				info_nbr_Node++;
			}
		}				

		bool option_dy_insert = true;
		bool option_order_of_K_maintain = false;

		for (auto pt = forests_new_edges.begin(); pt < forests_new_edges.end(); pt++){
			NodeIndex x = pt->first, y = Edgepos[pt->second].first, z = getEdge(pt->second).to;
			NodeIndex y_id_x = util::getValue(ordMap[x], y);
			NodeIndex z_id_x = util::getValue(ordMap[x], z);
			auto& Forest_x = Forest[x];
			pair<NodeIndex, NodeIndex> edge;
			int t_yz_new = getEdge(pt->second).truss;
			/*
			for (int i=0; i<Forest[x].size(); i++){
				for (int j=0; j< Forest[x][i].size(); j++)
					cout << Forest[x][i][j].to<< " ";
				cout <<endl;
			}
			*/
			NodeIndex a = y_id_x , b = z_id_x , b_i, a_i;
			if (max(a, b) >= Forest_x.size()){
				insertEdgeIntoForest(x, y_id_x, z_id_x, t_yz_new);
				continue;
			}			
			b_i = findEdgeInForeset(x, a, b ,false);
			if ( b_i != IndexType(-1)){
				a_i = findEdgeInForeset(x, b, a ,false);
				Forest_x[a][b_i].truss = t_yz_new;
				Forest_x[b][a_i].truss = t_yz_new;
				if (option_order_of_K_maintain){
					sort(Forest_x[a].begin(), Forest_x[a].end(), _WeightEdge::trussDec);
					sort(Forest_x[b].begin(), Forest_x[b].end(), _WeightEdge::trussDec);
				}
			}
			int ret_code = -1 ;
			if (max(y_id_x, z_id_x) < Forest_x.size()){
				ret_code = dfsInForest(Forest_x, y_id_x, y_id_x, z_id_x, edge);
			}

			if (ret_code == 0){ // not in Forest_x.
				insertEdgeIntoForest(x, y_id_x, z_id_x, t_yz_new);
				continue;
			}

			a = edge.first;
			b_i = edge.second;
			b = Forest_x[a][b_i].to;
			a_i = findEdgeInForeset(x, b, a ,false);
			if (ret_code == 1){
				if (Forest_x[a][b_i].truss < t_yz_new) {
					removeEdgefromForest(x, a, b_i);
					removeEdgefromForest(x, b, a_i);
					insertEdgeIntoForest(x, y_id_x, z_id_x, t_yz_new);
				}

			}
			
	
		}		
			
	}
	void InsertGraph::TCPUpdateofMultiInsertMain(vector<pair<NodeIndex, NodeIndex> >& new_edges, string fn, int silent_lvl){
		//fainting now...2013-12-6
		vector<int> old_truss;
		vector<IndexType> ChangedEdges; 
		vector<IndexType> all_updated_edges;
		flushTruss(old_truss, ChangedEdges);
		output1(new_edges.size());
		size_t nbrTotalChangeEdge =0;
		Timer timer;
		timer.Start();
		for (auto it = new_edges.begin(); it != new_edges.end(); it++){
			UpdateTrussofInsert2(it->first, it->second, ChangedEdges);
			nbrTotalChangeEdge += ChangedEdges.size();
			this->UpdateTCPofInsertOneByOne(ChangedEdges, old_truss);
			//now the truss updated in this iteration is old truss
			this->flushTruss(old_truss, ChangedEdges);
		}	
		timer.End("TCPUpdateofMultiInsertMain");
		cout <<"nbrTotalChangeEdge = "<< nbrTotalChangeEdge <<endl;
	}
	int InsertGraph::dfsInForest(vector<vector<_WeightEdge> >& Forest_x, NodeIndex crt, NodeIndex src, NodeIndex dst, pair<NodeIndex, NodeIndex>& edge){
		//scr may not inside Forest, must judge outside to avoid function call
		//ret  0 = not found, 1 = found in path, 2 = found in neibr


		for (NodeIndex i = 0; i<Forest_x[crt].size(); i++){
			NodeIndex y = Forest_x[crt][i].to;
			//output1(Forest_x[crt][i].to);
			if (y == dst){
				edge = make_pair(crt, i);
				return 1;
			}
			if (y != src){
				if (dfsInForest(Forest_x, y, crt, dst, edge) == 1){
					//output2(edge.first, edge.second);
					if (Forest_x[crt][i].truss < Forest_x[edge.first][edge.second].truss){
						edge.first = crt;
						edge.second = i;
					}
					return 1;
				}
			}
			
		}
		//output1("ret");
		return 0;
	}
	void InsertGraph::updateForestOfInsert(pair<NodeIndex, IndexType>* begin, pair<NodeIndex, IndexType>* end ){
		//old batch insert, not consider truss, unzip all forest and rebuild
		assert(false);
		NodeIndex x = begin->first;
		NodeIndex forest_x_max_node_id = 0;
		NodeIndex nbrNeibr = min (Edges[x].size(), ordMap[x].size());
		static vector<_WeightEdge> tmpEdge;
		
		tmpEdge.clear();
		auto& Forest_x = Forest[x];
		forest_x_max_node_id = Forest_x.size();
		for (size_t i =0; i< Forest_x.size(); i++){
			for (size_t j= 0; j<Forest_x[i].size(); j++){
				if (i < Forest_x[i][j].to)
					tmpEdge.push_back(Forest_x[i][j]);
			}
		}

		for (pair<NodeIndex, IndexType>* pt = begin ; pt != end; pt++){
			IndexType e_yz_id = pt->second;
			NodeIndex y = Edgepos[e_yz_id].first, z_id_y = Edgepos[e_yz_id].second;
			NodeIndex z = Edges[y][z_id_y].to;
			NodeIndex y_id = util::getValue(ordMap[x], y);
			NodeIndex z_id = util::getValue(ordMap[x], z);
			forest_x_max_node_id = max(forest_x_max_node_id , max(y_id, z_id));
			//output2(Edges[x].size(), y_id);
			//output2(Edges[x].size(), z_id);
			//output2(Edges[y].size(), z_id_y);
			int min_w =min(min(Edges[x][y_id].truss, Edges[x][z_id].truss), Edges[y][z_id_y].truss);
			tmpEdge.push_back(_WeightEdge(y_id, z_id, min_w));
		}

		buildForestFromEdges(x, tmpEdge, forest_x_max_node_id);

	}
	void InsertGraph::updateForestOfBatchInsert(pair<NodeIndex, IndexType>* begin, pair<NodeIndex, IndexType>* end ){
		NodeIndex x = begin->first;
		NodeIndex forest_x_max_node_id = 0;
		NodeIndex nbrNeibr = min (Edges[x].size(), ordMap[x].size());
		static vector<_WeightEdge> tmpEdge;
		int min_truss_of_insert_edge = 0;
		for (pair<NodeIndex, IndexType>* pt = begin ; pt != end; pt++){
			
			IndexType e_yz_id = pt->second;
			NodeIndex y = Edgepos[e_yz_id].first, z_id_y = Edgepos[e_yz_id].second;
			NodeIndex z = Edges[y][z_id_y].to;
			NodeIndex y_id = util::getValue(ordMap[x], y);
			NodeIndex z_id = util::getValue(ordMap[x], z);
			forest_x_max_node_id = max(forest_x_max_node_id , max(y_id, z_id));
			int min_w =min(min(Edges[x][y_id].truss, Edges[x][z_id].truss), Edges[y][z_id_y].truss);
			tmpEdge.push_back(_WeightEdge(y_id, z_id, min_w));
			//[above inside loop not modified]
			min_truss_of_insert_edge = max(min_truss_of_insert_edge, min_w);
		}

		tmpEdge.clear();
		auto& Forest_x = Forest[x];
		forest_x_max_node_id = max(forest_x_max_node_id, (NodeIndex)Forest_x.size());

		this->unzipBelowLvlK(Forest[x], min_truss_of_insert_edge, tmpEdge); //unzip forest to tmpEdge
		insertEdgesIntoExistingForest(x, tmpEdge);
		//this->unzipBelowLvlK(Forest[x], 2147483647, tmpEdge); //unzip forest to tmpEdge
		//buildForestFromEdges(x, tmpEdge, forest_x_max_node_id);

	}
	void InsertGraph::updateForestOfBatchInsertTwo(
		pair<NodeIndex, pair<IndexType, IndexType> >* begin, 
		pair<NodeIndex, pair<IndexType, IndexType> >* end )
	{
		assert(false);
		/*
		NodeIndex x = begin->first;
		NodeIndex forest_x_max_node_id = 0;
		NodeIndex nbrNeibr = min (Edges[x].size(), ordMap[x].size());
		static vector<_WeightEdge> tmpEdge;
		int max_truss_of_insert_edge = 0;
		for (auto pt = begin ; pt != end; pt++){
			
			NodeIndex y_id = pt->second.first;
			NodeIndex z_id = pt->second.second;
			NodeIndex y = Edges[x][y_id].to;
			forest_x_max_node_id = max(forest_x_max_node_id , max(y_id, z_id));
			int min_w =min(min(Edges[x][y_id].truss, Edges[x][z_id].truss), Edges[y][z_id_y].truss);
			tmpEdge.push_back(_WeightEdge(y_id, z_id, min_w));
			//[above inside loop not modified]
			max_truss_of_insert_edge = max(max_truss_of_insert_edge, min_w);
		}

		tmpEdge.clear();
		auto& Forest_x = Forest[x];
		forest_x_max_node_id = max(forest_x_max_node_id, (NodeIndex)Forest_x.size());

		//this->unzipBelowLvlK(Forest[x], max_truss_of_insert_edge, tmpEdge); //unzip forest to tmpEdge
		this->unzipBelowLvlK(  Forest[x],2147483647, tmpEdge); //unzip forest to tmpEdge
		buildForestFromEdges(x, tmpEdge, forest_x_max_node_id);
		*/
	}
	void InsertGraph::UpdateTCPofInsert(vector<pair<NodeIndex, NodeIndex> >& new_edges, string fn, int silent_lvl){
		int offset = 1;
		cerr << "updating Truss\n";
		Timer timer;
		vector<IndexType> ChangedEdges; 
		Timer timer_ktruss, timer_tcp;
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
		output1(new_edges.size());
		timer.Start();
		for (auto it = new_edges.begin(); it != new_edges.end(); it++){
			UpdateTrussofInsert2(it->first, it->second, ChangedEdges);
			nbrTotalChangeEdge += ChangedEdges.size();
			for (int i=0; i<ChangedEdges.size(); i++){
				all_updated_edges.push_back(ChangedEdges[i]);
			}

		}		
		timer.End("insert time:");
		timer.Start();
		printf("%d nbrTotalChangeEdge\n", nbrTotalChangeEdge);
		sort(all_updated_edges.begin(), all_updated_edges.end());
		all_updated_edges.resize(unique(all_updated_edges.begin(), all_updated_edges.end()) - all_updated_edges.begin());
		old_truss.resize(Edgepos.size(), 2147483647); //resize the edgesize
		
		vector<pair<NodeIndex, IndexType> > forests_new_edges;
		for (int i=0; i<all_updated_edges.size(); i++){
			IndexType e_xy_id = all_updated_edges[i];
			NodeIndex x = Edgepos[e_xy_id].first;
			NodeIndex y = Edges[x][Edgepos[e_xy_id].second].to;
			auto& e_xy = Edges[Edgepos[e_xy_id].first][Edgepos[e_xy_id].second];
			TriangleListingXY(x, y, triangles);
			//output3(x, y, triangles.size());
			for (size_t tr_id = 0; tr_id < triangles.size(); tr_id++ )
			{
				auto& tr = triangles[tr_id];
				NodeIndex w = Edges[x][tr.first].to;
				//output3(x , y, w);
				int min_w_new =min(min(Edges[x][tr.first].truss, Edges[y][tr.second].truss), e_xy.truss);
				int min_w_old =min(min( old_truss[Edges[x][tr.first].id], 
										old_truss[Edges[y][tr.second].id]), 
										old_truss[e_xy.id]);

				if ((min_w_new != min_w_old) || (old_truss[e_xy.id] == 2147483647)){
					
					forests_new_edges.push_back(make_pair(w, e_xy.id));
					forests_new_edges.push_back(make_pair(x, Edges[y][tr.second].id));
					forests_new_edges.push_back(make_pair(y, Edges[x][tr.first].id));
				}
			}
		}
		sort(forests_new_edges.begin(), forests_new_edges.end(), util::KeyValueCmp<NodeIndex,NodeIndex>);
		pair<NodeIndex, IndexType> *from = forests_new_edges.data();
		pair<NodeIndex, IndexType> *end = forests_new_edges.data() + forests_new_edges.size();
		int info_nbr_Node = 0;
		cout << "Edges: "<< end - from << endl;
		for (pair<NodeIndex, IndexType>* pt = from + 1; pt < end + 1; pt++){
			if (pt == end || pt->first != (pt-1)->first){
				updateForestOfBatchInsert(from, pt);
				from = pt;
				info_nbr_Node++;
			}
		}		
		timer.End("TCP time:");
		printf("%d Node update TCP\n", info_nbr_Node);
	}
	
	
};
