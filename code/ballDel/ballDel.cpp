/***************
main cpp
the code for truss decremental maintenance in paper 
"Efficient Star-based Truss Maintenance on Dynamic Graphs".
****************/
#include "common.h"
#include "myG.h"
#include "delPart.h"
#include "file.h"

// variables for timing
long g_lPreLoadTime;
long g_lPreRmLoadTime;
long g_lPreRmTime;
long g_lPreRmCoreTime;
long g_lPreRmBasicTime;
long g_lPreFindTime;
long g_lLocalTime;
long g_lAfterLocalTime;
long g_lUpdateKTime;
long g_lRecalLTime;
long g_lUpdateLTime;

// graph object
myG g_baseG;
//char* g_pcDataLabel;

/*****************
input:
        char *pcIndexFile: index file path
        char *pcPrivateFile
        map<int, vector <int> > &mpPrivate: output star motifs read from file
description:
        global init
******************/
int init(char *pcIndexFile, char *pcPrivateFile, map<int, vector <int> > &mpPrivate)
{
    int iPublicSize = 0;
    int iPirvateMaxSize = 0;
    int iMaxPid = 0;

    printf("init read G\n");
    iPublicSize = file_readBitG(g_baseG, pcIndexFile);

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

	int iOutputFlag = 0;

	int iDebugCnt = 0;
	int iDebugNum = 10;

	/* < nodeId, <neighbor> >*/
	map<int, vector <int> > mpPrivate;
	vector <pair<int, int> > lstQuery;
    vector <pair<int, int> >::iterator itQuery;

    if (4 > argc)
    {
        printf("argc: %d\n", argc);
        DEBUG_ASSERT(0);
    }

    char *pcIndexFile = argv[1];
    char *pcPrivateFile = argv[2];
    char *pcQueryFile = argv[3];

    printf("start init\n");
    init(pcIndexFile, pcPrivateFile, mpPrivate);
    printf("start read query\n");
    file_readQuery(pcQueryFile, lstQuery);

    printf("start query\n");
    for (itQuery = lstQuery.begin(); itQuery != lstQuery.end(); itQuery++)
    {
        gettimeofday(&tv, NULL);
        lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
        delPart::delOneNodePart(g_baseG, mpPrivate,
                      itQuery->first);
        gettimeofday(&tv, NULL);
        lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
        lQueryTime += lCurTime - lStartTime;
        printf("DEBUG query time %.3f ms\n", (lCurTime - lStartTime) / 1000.0);
    }

    printf("finish query\n");
    /* save result */
    if (4 < argc)
    {
        file_saveBitG(g_baseG, argv[4]);
    }
    if (5 < argc)
    {
        file_saveTrussness(g_baseG, argv[5]);
    }

	printf(" g_lPreFindTime: %.3f ms\n",
        g_lPreFindTime / 1000.0);
	printf(" g_lPreRmLoadTime: %.3f ms\n",
        g_lPreRmLoadTime / 1000.0);
	printf(" g_lPreRmCoreTime: %.3f ms\n",
        g_lPreRmCoreTime / 1000.0);
	printf(" g_lPreRmBasicTime: %.3f ms\n",
        g_lPreRmBasicTime / 1000.0);
	printf(" g_lPreRmTime: %.3f ms\n",
        g_lPreRmTime / 1000.0);
	printf(" g_lPreLoadTime: %.3f ms\n",
        g_lPreLoadTime / 1000.0);
	printf(" g_lLocalTime: %.3f ms\n",
        g_lLocalTime / 1000.0);
	printf(" g_lAfterLocalTime: %.3f ms\n",
        g_lAfterLocalTime / 1000.0);
	printf(" g_lUpdateKTime: %.3f ms\n",
        g_lUpdateKTime / 1000.0);
	printf(" g_lRecalLTime: %.3f ms\n",
        g_lRecalLTime / 1000.0);
	printf(" g_lUpdateLTime: %.3f ms\n",
        g_lUpdateLTime / 1000.0);

	printf(" total query time: %.3f ms\n",
        lQueryTime / 1000.0);

    return 0;
}
