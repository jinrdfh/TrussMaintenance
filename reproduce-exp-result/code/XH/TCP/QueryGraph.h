#pragma once
#ifndef H_QUERYGRAPH
#define H_QUERYGRAPH

//#define onging_20131120
//#define debug_2013_11_20

#include "Graph.h"


namespace sinz{
	namespace graph{
		void QueryLoader(FileHandler& fd, vector<vector<pair<int,int> > >& Query);
		void QueryDumper(FileHandler& fd, vector<vector<pair<int,int> > >& Query);

		class EdgeQueue:public sinz::queue<pair<int, int> >{
			int _nbrFirst;
			int qid;
			vector<int> s;
			vector<int> DirectVis;
		public:
			EdgeQueue(){
				qid = 0;
				_nbrFirst = -1;
			}
			void Init(vector<int>& v){
				s.resize(v.size());
				s[0] = 0;
				for (int i=1; i<v.size(); i++)
					s[i] = s[i-1] + v[i - 1];
				_nbrFirst = v.size();
				DirectVis.resize(s[s.size() - 1]);
				//cerr <<  "Init\n";
			}
			void Reset(){
				assert(_nbrFirst > 0);
				qid++;
			}
			inline void push(pair<int, int>&& a){
				int t = s[a.first] + a.second;
				if (DirectVis[t] != qid){
					push_back(a);
					DirectVis[t] = qid;
				}
			}

			inline bool notIn(int x, int y){
				return DirectVis[s[x] + y] != qid; 
			}


		};		
		class Communities: public vector2d<int>{
			typedef int flag_type;


		public:
			Communities(){
		
			}			

			
			template<class _Pr, class _Pr2>
			void DisplayCommunityAndOverlapping(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){


				cout<<"#Communities = "<<size1d()<<"\n";
				
				for (int b_i=0; b_i<size1d(); b_i++){
					set<int> nodeids;
					//fprintf(stderr, "[%d %d) ", _B[b_i], _B[b_i + 1]);
					for (int*  e_it = beginPt(b_i); e_it< beginPt(b_i +1 ) ; e_it++){
						//dbg3(e_it, beginPt(b_i), beginPt(b_i +1 ));
						int e = * e_it;

						auto edge = fun_Edge(e);
						//printf("(%d, %d) ", fun_NodeName(edge.first), fun_NodeName(edge.second));
						if (edge.first == rid) continue;
						nodeids.insert(edge.first);
						if (edge.second == rid) continue;
						nodeids.insert(edge.second);
					}
					cout<<"Community "<<b_i + 1<<":";
					int newline_cnt = 0;
					for_iter_(it, nodeids){
						if (newline_cnt++ % 10 ==0)
							cout<<"\n";
						cout<<(it==nodeids.begin() ? " " : ", ");
						cout<<fun_NodeName(*it);

					}
					cout<<"\n";
				}
				
			} 
		template<class _Pr, class _Pr2>
			void DisplayCommunityForAnswerQuery(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){
				//comments: this function is added when resubmit to sigmod14
				//			for illustrate query accuracy.

				cout<<size1d()<<"\n";
				
				for (int b_i=0; b_i<size1d(); b_i++){
					set<int> nodeids;
					//fprintf(stderr, "[%d %d) ", _B[b_i], _B[b_i + 1]);
					for (int*  e_it = beginPt(b_i); e_it< beginPt(b_i +1 ) ; e_it++){
						//dbg3(e_it, beginPt(b_i), beginPt(b_i +1 ));
						int e = * e_it;
						auto edge = fun_Edge(e);
						nodeids.insert(edge.first);
						nodeids.insert(edge.second);
					}
					//cout<<b_i + 1<<":";
					int newline_cnt = 0;
					if (rid != -1)
						nodeids.erase(rid);
					for_iter_(it, nodeids){
						cout<<fun_NodeName(*it)<<" ";
					}
					cout<<"\n";
				}
				
			} 
			template<class _Pr, class _Pr2>
			void DisplayCommunityAndSum(_Pr fun_Edge, _Pr2 fun_NodeName, int rid = -1){
			

				cout<<"#Communities = "<<size1d()<<"\n";
				
				for (int b_i=0; b_i<size1d(); b_i++){
					set<int> nodeids;
					//fprintf(stderr, "[%d %d) ", _B[b_i], _B[b_i + 1]);
					for (int*  e_it = beginPt(b_i); e_it< beginPt(b_i +1 ) ; e_it++){
						//dbg3(e_it, beginPt(b_i), beginPt(b_i +1 ));
						int e = * e_it;

						auto edge = fun_Edge(e);
						//printf("(%d, %d) ", fun_NodeName(edge.first), fun_NodeName(edge.second));
						if (edge.first == rid) continue;
						nodeids.insert(edge.first);
						if (edge.second == rid) continue;
						nodeids.insert(edge.second);
					}
					cout<<"Community "<<b_i + 1<<":";
					int newline_cnt = 0;
					for_iter_(it, nodeids){
						if (newline_cnt++ % 10 ==0)
							cout<<"\n";
						cout<<(it==nodeids.begin() ? " " : ", ");
						cout<<fun_NodeName(*it);

					}
					cout<<"\n";
				}
			}

		};


		class QueryGraph{
		public:
			struct _WeightEdge{
				int from, to, truss;
				_WeightEdge(){}
				_WeightEdge(int f_, int t_, int tr_):from(f_), to(t_), truss(tr_){}
			};
			Vector2d<graph::edgeTruss> edgeTrusses;
			Pair_Hash<int, int> _edgehash;
			vector<char> _vised;

			vector<_WeightEdge> _edge;
			Pair_Hash<int, int> _indexOfNode;
			vector2d<int> revertEdgeID;
			EdgeQueue edgeQueue;
			int _nbrEdge;
			int _nbrNode;
			int _maxTruss;
		public:


			QueryGraph(){

			}
			QueryGraph(kTrussGraph& kG, const char* prefix, const char *suffix, int offset){
				kG.loadDataAdaptive(prefix, suffix , offset); 
				kG.kTruss();
			}
			
			STRONG_INLINE pair<int, int> getEdge(int eid_){
				//dbg2(eid_, _edge.size());
				return make_pair(_edge[eid_].from, _edge[eid_].to);
			}
			/*
			void getTrussGraph(const char* truss_file_name){
				cerr << "TCP getTrussGraph "<<endl;
				int m = 0;
				if (!FileHandler::testOpen(truss_file_name)) return ;

				FileHandler fd(truss_file_name, "rb");
				IO::readVector(fd, _edge);

				int maxTrust(0);
				_nbrNode = 0;
				for (int i=0; i<m; i++){
					_nbrNode = max(_nbrNode, max(_edge[i].from, _edge[i].to));
					maxTrust = max(maxTrust, _edge[i].truss);
					//fprintf(stderr, "%d %d %d\n", _edge[i].from, _edge[i].to, _edge[i].truss);
				}
				_nbrNode += 1;
				_nbrEdge = m;
				fprintf(stderr, "#nbrNode = %d, #nbrEdge = %d, maxTrust = %d\n", _nbrNode, _nbrEdge, maxTrust);
				ContrustSimpleIndex();
			}
			*/
			/*
			void ContrustSimpleIndex(){

				auto& v = edgeTrusses._v; 

				edgeTrusses.resize(_nbrNode);
				_edgehash = Pair_Hash<int, int>(_nbrNode);
				_maxTruss = 0;
				for_ (eid, 0, _nbrEdge) {
					v[_edge[eid].from].push_back( edgeTruss(_edge[eid].to, eid, _edge[eid].truss));
					v[_edge[eid].to].push_back( edgeTruss(_edge[eid].from, eid, _edge[eid].truss));
					_maxTruss = max(_maxTruss,  _edge[eid].truss);
				}
				cout <<"Construct Ktruss Index"<<endl;
				Timer timer;
				int nid = 0;
				for_iter_ (node , v){
					sort(node->begin(), node->end(), [&v](const edgeTruss& a, const edgeTruss& b){
						return (a.truss == b.truss) ? 	(v[a.to].size() == v[b.to].size() ?  a.to < b.to : v[a.to].size() < v[b.to].size()) : (a.truss > b.truss);
					});
					for_ (e_i,0, node->size())
						_edgehash[nid][(*node)[e_i].to] = (*node)[e_i].eid;
					nid++;
				}

				_vised.resize(_nbrEdge, false);

				edgeTrusses.buildIndex();
				//timer.getTick("Build Simple Index", global::ts);
				timer.End("Build Simple Index");
				fill(_vised.begin(), _vised.end(), false);
			}
			void getTrussGraph(vector<pair<int, int> >& fromto_, vector<int>& trussOfEdge_, int nbrNode_){
				fprintf(stderr, "noting");
				assert(false);		
			}	
			*/

		};



	}
}
#endif
