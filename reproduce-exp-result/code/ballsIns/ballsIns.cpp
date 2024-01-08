/***************
main cpp
the code for truss incremental maintenance in paper 
"Efficient Star-based Truss Maintenance on Dynamic Graphs".
****************/
#include "common.h"
#include "myG.h"
#include "insertPart.h"
#include "file.h"

// graph object
myG g_baseG;

// variables for timing
long g_lSimpleAddTime;
long g_lFillInfoTime;
long g_lBatchInsTotalTime;
long g_lBallDecTime;
long g_lLocalDecTime;
long g_lEdgeDecTime;
long g_lOutBallTime;
long g_lAllKTime;
long g_lRmQTime;

long g_lNeibCnt;
long g_lUsefulCnt;

extern long g_lBallSize;
extern long g_lInitCnt;
extern long g_lUgCnt;

/*****************
input:
        char *pcIndexFile: index file path
        char *pcPrivateFile
        map<int, vector <int> > &mpPrivate: output star motifs read from file
description:
        global init
******************/
int init(char *pcSrcPath, char *pcPrivateFile, map<int, vector <int> > &mpPrivate)
{
    int iPublicSize = 0;
    int iPirvateMaxSize = 0;
    int iMaxPid = 0;

    printf("init read G\n");
    iPublicSize = file_readBitG(g_baseG, pcSrcPath);

    printf("init read priG\n");
    iPirvateMaxSize = file_readPrivate(pcPrivateFile, mpPrivate, &iMaxPid);
    iMaxPid = iMaxPid>g_baseG.m_iMaxPId?iMaxPid:g_baseG.m_iMaxPId;

    printf("init reserve G pub size: %d, pri size: %d\n",
           iPublicSize, iPirvateMaxSize);
    g_baseG.m_pvG->reserve(iPublicSize + iPirvateMaxSize + 10);
    printf("init reserve neighbor\n");
    g_baseG.m_pvPNeCnt->resize(iMaxPid + 1);
    DEBUG_PRINTF("DEBUG init len: %d, capacity: %d reserve len: %d\n",
           g_baseG.m_pvPNeCnt->size(), g_baseG.m_pvPNeCnt->capacity(), iMaxPid + 10);
    g_baseG.m_pvPNeCnt->reserve(iMaxPid + 10);

    g_baseG.init();

    return 0;
}

/*****************
input:
        index file name
        star file
        query file
        output index file
        output truss file
description:
        main function
******************/
int main(int argc, char *argv[])
{
	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;
	long lQueryTime = 0;

	/* < nodeId, <neighbor> >*/
	map<int, vector <int> > mpPrivate;
	list<pair<int, int> > lstQuery;

    if (4 > argc)
    {
        printf("argc: %d\n", argc);
        DEBUG_ASSERT(0);
    }

    char *pcIndex = argv[1];
    char *pcPri = argv[2];
    char *pcQuery = argv[3];

    printf("start init\n");
    init(pcIndex, pcPri, mpPrivate);
    printf("start read query\n");
    file_readQuery(pcQuery, lstQuery);

    printf("start query\n");
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    insertPart::insertAll(g_baseG, mpPrivate, lstQuery);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    lQueryTime += lCurTime - lStartTime;

    /* save result */
    if (4 < argc)
    {
        file_saveBitG(g_baseG, argv[4]);
    }
    if (5 < argc)
    {
        file_saveTrussness(g_baseG, argv[5]);
    }

    printf("finish query\n");

    printf("DEBUG ball size: %d BFS: %ld upgraded: %ld\n",
           g_lBallSize, g_lInitCnt, g_lUgCnt);

	printf(" g_lSimpleAddTime: %ld ms\n",
        g_lSimpleAddTime);
	printf(" g_lFillInfoTime: %ld ms\n",
        g_lFillInfoTime);
	printf(" g_lBatchInsTotalTime: %ld ms\n",
        g_lBatchInsTotalTime);
	printf(" g_lBallDecTime: %.3f ms\n",
        g_lBallDecTime / 1000.0);
	printf(" g_lLocalDecTime: %ld ms\n",
        g_lLocalDecTime);
	printf(" g_lEdgeDecTime: %ld ms\n",
        g_lEdgeDecTime);
	printf(" g_lOutBallTime: %.3f ms\n",
        g_lOutBallTime / 1000.0);
	printf(" g_lRmQTime: %ld ms\n",
        g_lRmQTime);
	printf(" g_lNeibCnt: %ld g_lUsefulCnt: %ld\n",
        g_lNeibCnt, g_lUsefulCnt);
	printf(" total query time: %.3f ms\n",
        lQueryTime / 1000.0);

    return 0;
}
