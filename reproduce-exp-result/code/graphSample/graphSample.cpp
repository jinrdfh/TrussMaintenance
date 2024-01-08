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
	char *pcPubFile = NULL;
	char *pcPriFile = NULL;

	float fRatio = 0;
	int iSamNum = 0;

	vector <pair<int, int> > vPubE;
	vector <pair<int, int> > vPriE;

    if (6 != argc)
    {
        printf("argc: %d\n", argc);
        debug_assert(0);
    }

    pcSrcFile = argv[1];
    pcPubFile = argv[2];
    pcPriFile = argv[3];
    sscanf(argv[4], "%f", &fRatio);
    sscanf(argv[5], "%d", &iSamNum);

    init(pcSrcFile);
    g_baseG.sample(vPubE, vPriE, fRatio, iSamNum);
    printf("sample done\n");
    file_saveGraph(g_baseG, vPubE, pcPubFile);
    file_saveGraph(g_baseG, vPriE, pcPriFile);
    printf("save done\n");

    return 0;
}
