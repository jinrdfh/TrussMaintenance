#include <climits>
#include  "util.h"
namespace sinz{
	namespace IO{
		void WriteBlock(FileHandler& fd, void* buf, size_t size ){
			fwrite(buf, size, 1, fd());
		}
		void WriteBlockN(FileHandler& fd, void* buf, size_t size, size_t blocks ){
			unsigned long long total_size = (unsigned long long)size * blocks;
			//output1(size);
			char* offset = (char* )buf;
			while (total_size>0){
				//output1(total_size);
				unsigned int nbrblocks = min(total_size, (unsigned long long)INT_MAX);
				if (fwrite(offset, nbrblocks, 1, fd()) <0)
					cerr << "err fwrite"<<endl;
				offset += nbrblocks;
				total_size -= nbrblocks;
			}
		}
		void ReadBlock(FileHandler& fd, void* buf, size_t size){
			fread(buf,size,1,fd());
		}
		void ReadBlockN(FileHandler& fd, void* buf, size_t size, size_t blocks ){
			unsigned long long total_size = (unsigned long long)size * blocks;
			//output1(size);
			char* offset = (char* )buf;
			while (total_size>0){
				//output1(total_size);
				unsigned int nbrblocks = min(total_size, (unsigned long long)INT_MAX);
				if (fread(offset, nbrblocks, 1, fd()) <0)
					cerr << "err fread"<<endl;
				offset += nbrblocks;
				total_size -= nbrblocks;
			}
		}
	}
	namespace Sample{
		unsigned int rand30(){
			return (((unsigned int)rand())<<30) | rand();
		}
		__int64 rand60(){
			return ((__int64)rand30())<<30 | rand30();
		}
		STRONG_INLINE __int64 int64(int x,int y){
			return ((((__int64)x)<<32) | y);
		}
	}
}
