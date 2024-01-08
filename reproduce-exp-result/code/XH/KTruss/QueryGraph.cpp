#include "QueryGraph.h"
namespace sinz{
	
	namespace graph{


		bool OntologyGraph::SearchSetByUID(int u_id, int k, EdgeQueue& directed_edge_queue, vector<edgeTruss>& gv,
			vector<char>& vis){
			dbg3("SearchSetByUID",_id, u_id);

//			if (vised[u_id])
//				return false;
#ifndef debug_2013_11_14
			BFSqueue<int> q(C.size());
#else
			q.clear(C_begin.size() - 1);
#endif
			q.push(H[u_id].first);
			while (!q.empty()){
				int front = q.pop();
				if (front >= Edge.size())    //comment: maybe not edge conneced to this supernode 
					continue;
				for_iter_ (edge, Edge[front]){
					if (edge->second < k) 
						break;
					q.push(edge->first);
				}

			}
			dbg2(q.qr, C_begin.size());
			for (int c_id = 0; c_id < q.qr; c_id++){
				dbg2(_id, c_id);
				int c = q.q[c_id];
				for_ (w_it, C_begin[c], C_begin[c+ 1]){
					int w_id = C_ID[w_it];

					if (vis[gv[w_id].eid]  == false){
						directed_edge_queue.push(make_pair(_id, w_id));
						dbg3(_id, w_id, directed_edge_queue.sizeInQueue());

					}
				}

				
			}
			return true;
		}

		void QueryGraph::OIQuery(int v0, int k, Communities & C ){
			C.reset(_nbrEdge); //should clear at first.
			
			auto& gr = edgeTrusses._v; 
			//if ( edgeTrusses._v[v0].size() == 0 || edgeTrusses._v[v0][0].truss<4) //cmt: for btc test allowing k = 3
			if ( edgeTrusses._v[v0].size() == 0 )
				return ;
			
			//using trussOfEdge, fromto_
			//cerr <<"OIQuery "<<v0<<" "<<k<<endl;
#ifdef DEBUG_MODE 
			OIndex[v0].Display([&](const int& nid){
					return gr[v0][nid].to + 1;
				},[&](const int& eid){
					return eid + 1;
				});	
#endif
				
#ifndef SPEED_MODE
			cerr <<"OIQuery "<<v0<<" "<<k<<endl;
			cout <<"OIQuery "<<v0<<" "<<k<<endl;
#endif

		//	cerr << "tt"<<endl;
			//Communities C(&fromto_);
		
			//int Cid = 0;
			if (v0 >= gr.size())
				return ;
			dbg2(edgeTrusses._v[v0].size(), k);

			int nbrMatchedEdges = 0;

			edgeQueue.Reset();
			//dummy: vector<char> edgeused(fromto_.size(), false);
			auto& gr_v = gr[v0];
			//for ( auto e_it = gr[v0].begin(); e_it != edgeTrusses.begin(v0,k - 1); e_it++ ){


#ifndef onging_20131120
			static vector<int> curC;
#endif
			int __profile_cnt_call = 0, __profile_cnt_pop = 0;
			//Timer __profile__timer;

			for (int e_i = 0; e_i < edgeTrusses.beginInd(v0,k - 1); e_i++){
				dbg(gr_v[e_i].truss);
				if (gr_v[e_i].truss >= k && edgeQueue.notIn(v0, e_i)){
					// 0 == OIndex[v0].vised[e_i] => qid == OIndex[v0].vised[e_i]
#ifndef onging_20131120
					curC.clear();
#endif
					if (edgeQueue.notIn(v0, e_i)){
						//__profile__timer.Wake();
						OIndex[v0].SearchSetByUID(e_i, k, edgeQueue, gr[v0], _vised); //require index of u in gr[v0]instead of u
						//__profile__timer.Sleep();
						//__profile_cnt_call +=1;
					}
					dbg2(v0, e_i);
					
					while (!edgeQueue.empty()){
						auto front = edgeQueue.pop();
						
						int push_eid = gr[front.first][front.second].eid;
#ifndef onging_20131120
						if (0 == _vised[push_eid])
						{
							curC.push_back(push_eid);
							_vised[push_eid] = true;
						}
#else

						C.push_back(push_eid);
#endif


						__profile_cnt_pop++;
						const int& y = gr[front.first][front.second].to;
						const int& x = revertEdgeID.beginPt(front.first)[front.second];  //_indexOfNode
					
						if (edgeQueue.notIn(y, x)){
							//__profile__timer.Wake();
							OIndex[y].SearchSetByUID(x, k, edgeQueue, gr[y], _vised);
							//__profile__timer.Sleep();
							//__profile_cnt_call +=1;
						}
						

					}
#ifndef onging_20131120
					if (curC.size()>0){
						//should put edge in C, only need to check wether a node has expanded?
						C.append(curC);
						nbrMatchedEdges += curC.size();
					}

#else
					C.makeEndNonEmpty();
#endif
				
				
				}

			}
			
			dbg("reseting");
#ifndef onging_20131120
			/*
			for (int i= 0; i< _vised.size(); i++)
				cout << (int)_vised[i]<< " ";
			cout << endl;
			*/
			C.resetBitmap(_vised);
			//cerr<< __profile_cnt_call << " " << __profile_cnt_pop << " "<< C.size()<<endl;
			//cerr<<__profile__timer.Sum()<<endl;
#endif
	
			dbg(C.size());
			dbg("reseting resetOIndex");

//			q.resetOIndex(OIndex);
			dbg("reseting finished");
			//cout<<endl;
			//return C;
			//fflush(stdout);
#ifdef DEBUG_MODE
			system("pause");
#endif
		}
		
	};
};
