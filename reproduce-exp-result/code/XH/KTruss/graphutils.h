#ifndef H_GRAPHUTILS
#define H_GRAPHUTILS
#include "global.h"
#include "util.h"
namespace sinz{
	namespace graph{
		class MovableBinVec;

		template<class T>
		class Vector2d;

		template<class T, class T2>
		class Pair_Hash;

		template<class T, class T2>
		class AdjList;

		struct edgeTruss{
			int to, eid, truss;
			edgeTruss(int t, int e, int tr):to(t),eid(e),truss(tr){}
			edgeTruss(){};
		};
		struct EdgeWed{
			int to, eid, w;
			EdgeWed(int t_, int e_, int w_):to(t_),eid(e_),w(w_){}
			EdgeWed(){};
		};		
	}
};
namespace sinz{
	namespace graph{

		template<class _IndexType>
		class movableVector2d:public vector2d<_IndexType, _IndexType>{
			//2013-12-2
			int* _sup; 
			_IndexType* _edgepos;
			vector<_IndexType> StartInd;
			_IndexType min_bin_id;
			void build_inside(){
				for (size_t it = 0, it_end = this->size(); it<it_end; it++){
					_edgepos[this->data()[it]] = it;
				}
				StartInd.resize(this->pt.size() - 1);
				for (size_t i = 0; i< this->pt.size() -1 ;i++)
					StartInd[i] = this->pt[i];

				min_bin_id = 0;
			}
		public:
			movableVector2d(){
				/*
				should be build before used.
				*/
			}
			void build(vector<_IndexType>& v, int* sup, _IndexType* edgepos){
				min_bin_id = 0;
				_sup = sup;
				_edgepos = edgepos;
				this->resize(v.size());
				copy(v.begin(), v.end(), this->begin());
				this->sort([&sup](const _IndexType& a, const _IndexType& b) {
					return	sup[a] < sup[b];	
				},[&sup](const _IndexType& a){
					return sup[a];
				});

				build_inside();

			}
			void build(_IndexType m, int* sup, _IndexType* edgepos){
				min_bin_id = 0;
				_sup = sup;
				_edgepos = edgepos;
				this->resize(m);
				for (_IndexType i=0; i<m; i++)
					this->data()[i] = i;
				this->sort([&sup](const _IndexType& a, const _IndexType& b) {
					return	sup[a] < sup[b];	
				},[&sup](const _IndexType& a){
					return sup[a];
				});

				build_inside();
			}
			void forward(_IndexType& e_xy){
				//using vector2d<_IndexType, _IndexType>;
				_IndexType* tv = this->data(); 
				int e_xy_bin = _sup[e_xy];

				_IndexType e_xy_pos = _edgepos[e_xy];
				//output2(e_xy_bin, e_xy_pos);
				if (e_xy_bin == 0){
					system("pause");
				}
				if (StartInd[e_xy_bin] == this->pt[e_xy_bin]){ //must have StartInd[e_xy_bin] == pt[e_xy_bin]

					_IndexType e_xy_bin_start_pos = this->pt[e_xy_bin];
					_IndexType e_uv = tv[e_xy_bin_start_pos];
					std::swap(tv[e_xy_bin_start_pos], tv[e_xy_pos]);
					std::swap(_edgepos[e_xy], _edgepos[e_uv]);
					this->pt[e_xy_bin]++;
					StartInd[e_xy_bin]++;
				}else{
					//put e_xy to pt[e_xy_bin]
					//put tv[StartInd[e_xy_bin]] to e_xy_pos
					tv[this->pt[e_xy_bin]] = e_xy;
					_edgepos[e_xy] = this->pt[e_xy_bin];
					this->pt[e_xy_bin]++;
					if (e_xy_pos != StartInd[e_xy_bin]){
						tv[e_xy_pos] = tv[StartInd[e_xy_bin]];
						_edgepos[tv[StartInd[e_xy_bin]]] = e_xy_pos;
					}
					StartInd[e_xy_bin]++;

				}
				//output3(StartInd[e_xy_bin], e_xy_bin, e_xy_pos);

				_sup[e_xy]--;
				min_bin_id = min((int)min_bin_id, _sup[e_xy]);

			}
			_IndexType Next(int lim = 2147483647){
				if (!hasNext(lim)){
					cerr<< "pop from empty bin";
					assert(false);
				}
		
				_IndexType ret = this->data()[StartInd[min_bin_id]];
				StartInd[min_bin_id]++;

				_edgepos[ret] = _IndexType(-1);
				return ret;
			}
			bool hasNext(int lim = 2147483647){
				if ( StartInd.size()== 0)
					return false;
				for (; min_bin_id <= min(StartInd.size() - 1, (size_t)lim); min_bin_id++)
					if (StartInd[min_bin_id] < this->pt[min_bin_id + 1])
						break;
				if (min_bin_id > min(StartInd.size() - 1, (size_t)lim))
					return false;
				else
					return true;
			}
		};
	};

};

class sinz::graph::MovableBinVec{
public:
	std::vector<vector<int> > edgeid;
	std::vector<pair<int, int> > edgepos;
	int max_sup;
	vector<int> cur_bin_pos;
	//vector<int> bin_end;
	int min_bin_id;
	static const int END = -1;
public:

	MovableBinVec(const std::vector<int>& sup, int bin_size){

		edgeid.resize(bin_size);
		edgepos.resize(sup.size());
		cur_bin_pos.resize(bin_size, 0);
		//bin_end.resize(bin_size);
		max_sup = bin_size - 1;
		min_bin_id = 2147483647;
		for_ (eid, 0, sup.size()){
			int e_sup = sup[eid];
			min_bin_id = min(min_bin_id, e_sup);
			//dbg(eid);
			edgeid[e_sup].push_back(eid);
			edgepos[eid] = make_pair(e_sup, int(edgeid[e_sup].size()) - 1);
		}

	}
	void build(vector<unsigned int>& v, int* sup){
		/*
		edgeid.resize(bin_size);
		edgepos.resize(sup.size());
		cur_bin_pos.resize(bin_size, 0);
		//bin_end.resize(bin_size);
		max_sup = bin_size - 1;
		min_bin_id = 2147483647;
		for_ (eid, 0, sup.size()){
			int e_sup = sup[eid];
			min_bin_id = min(min_bin_id, e_sup);
			//dbg(eid);
			edgeid[e_sup].push_back(eid);
			edgepos[eid] = make_pair(e_sup, int(edgeid[e_sup].size()) - 1);
		}
		*/
	}

	int max_bin_pos(vector<pair<int, int> >& _fromto){
		int ret= -1000;
		for_ (bid, 0, edgeid.size()){
			if (cur_bin_pos[bid] < edgeid[bid].size()){
				ret = bid;
			}
		}
		return ret;
	}
	inline void forward(int eid){

		//edgepos in (bin_id, bin_pos)
		int bid(edgepos[eid].first), pid(edgepos[eid].second);
		//output2(bid, pid);
		if (bid < 0)
			return ;

		if (pid >= cur_bin_pos[bid]){ //cmt: if eid is in Bin
			int org_eid = edgeid[bid][cur_bin_pos[bid]];
			edgeid[bid][pid] = org_eid; //cmt: move to front and increase pos
			edgepos[org_eid] =  edgepos[eid];
			cur_bin_pos[bid]++;
		}
		//output3(bid, pid, cur_bin_pos[bid]);

		edgepos[eid].first --;
		int cur_bin = edgepos[eid].first; //move to the previous bin

		if (edgepos[eid].first < min_bin_id){  // cmt: move min_bin_id forward
			min_bin_id = edgepos[eid].first;
		}
		//output2(eid, edgepos.size());
		if (cur_bin_pos[cur_bin] == 0 ){ //not space in front of bin
			edgeid[cur_bin].push_back(eid);
			edgepos[eid].second = edgeid[cur_bin].size() - 1;
		}
		else {
			cur_bin_pos[cur_bin]--;

			edgeid[cur_bin][cur_bin_pos[cur_bin]] = eid;
			edgepos[eid].second = cur_bin_pos[cur_bin];
		}

	}
	int next(int _limk){
		//dbg(min_bin_id);
		for (; min_bin_id <= _limk ; min_bin_id++){
			vector<int>& bin = edgeid[min_bin_id]; 
			int& _pos = cur_bin_pos[min_bin_id]; 
			for ( ;_pos< bin.size();_pos++){
				if (edgepos[bin[_pos]].first <0 )
					continue;
				edgepos[bin[_pos]].first = -1;
				return bin[_pos++];
			}
		}
		return -1;
	}
};



//#define Pair_Hash_ongoing
#ifndef Pair_Hash_ongoing
template<class T, class T2>
class sinz::graph::Pair_Hash{
	int _nbrFirst;
	vector<unordered_map<T, T2> > v;
public:
	Pair_Hash(){}
	Pair_Hash(int nbrFirst):_nbrFirst(nbrFirst){
		v.resize(_nbrFirst);		
	}
	template<class VectorValueType>
	Pair_Hash(vector2d<VectorValueType>& _v2d){
		v.resize(_v2d.size1d());
		for (int i=0; i<_v2d.size1d(); i++)
		{
			v[i].max_load_factor(1.5);
			v[i].rehash(_v2d.size2d(i));
		}
	}
	STRONG_INLINE bool hasKey(T x, T y){
		return v[x].find(y) != v[x].end();
	}
	STRONG_INLINE T2 getValue(T x, T y){
		//dbg2(x,v.size());
		auto it = v[x].find(y);
		return it == v[x].end() ? -1 : it->second;
	}
	STRONG_INLINE unordered_map<T, T2>& operator[](T x){
		return v[x];
	}
	STRONG_INLINE T2&  operator()(T& x, T& y){
		return v[x][y];
	}
	STRONG_INLINE void erase(T x, T y){
		v[x].erase(y);
	}
	void deallocate(){
		vector<unordered_map<T, T2> >().swap(v);
	}


#else
template<class T, class T2>
class sinz::graph::Pair_Hash{
	int _n;
	vector<unordered_map<T, T2> > v;
public:
	Pair_Hash(){}
	Pair_Hash(int n):_n(n){
		v.resize(_n);		
	}
	STRONG_INLINE bool hasKey(T x, T y){
		return v[x].find(y) != v[x].end();
	}
	STRONG_INLINE T2 getValue(T x, T y){
		auto it = v[x].find(y);
		return it == v[x].end() ? -1 : it->second;
	}
	STRONG_INLINE T2&  operator()(T& x, T& y){
		return v[x][y];
	}
	STRONG_INLINE void erase(T x, T y){
		v[x].erase(y);
	}
};
#endif

};

#endif
