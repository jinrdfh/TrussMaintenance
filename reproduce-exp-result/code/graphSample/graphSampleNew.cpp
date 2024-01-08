/***************
baseline
second step
input k and r
main cpp
****************/
#include "common.h"
#include "myG.h"
#include "file.h"

myG g_baseG;

/*****************
input:
        char *pcSrcPath: source data path
description:
        global init
******************/
int init(char *pcSrcFile)
{
    int iSize = 0;

    printf("init read G\n");
    iSize = file_fillG(g_baseG, pcSrcFile);
    printf("init done size: %d\n", iSize);
    DEBUG_ASSERT(0 < iSize);

    return iSize;
}

/*****************
input:
        source database Graph file name
        output result path name
        dataset label
        insert file
        output flag
description:
        main function
******************/
int main(int argc, char *argv[])
{
	char *pcSrcFile = NULL;
	char *pcPriPath = NULL;

	float fRatio = 0;
	int iSamNum = 0;
	int iBatNum = 0;
	int iRoot = 0;
	int i = 0;

    if (5 != argc)
    {
        printf("argc: %d\n", argc);
        debug_assert(0);
    }

    pcSrcFile = argv[1];
    pcPriPath = argv[2];
    sscanf(argv[3], "%d", &iSamNum);
    sscanf(argv[4], "%d", &iBatNum);

    init(pcSrcFile);
    vector <pair<int, int> > vPriE;
    vPriE.reserve(iSamNum);
    for (i = 0; i < iBatNum; ++i)
    {
        vPriE.clear();
        iRoot = g_baseG.sampleNew(vPriE, iSamNum);
        file_savePriGraph(g_baseG, iRoot, i + 1, vPriE, pcPriPath);
    }

    return 0;
}
