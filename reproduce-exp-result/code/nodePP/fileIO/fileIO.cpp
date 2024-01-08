/*******
used for file reading/writing
*******/
#include <map>
#include <utility>
#include <list>
#include <stdio.h>

using namespace std;

#undef NDEBUG
#include <assert.h>
#include "../Graph/Graph.h"
#include "../KTrussIndex/KTrussIndex.h"
#include "fileIO.h"

/*****************
input:
        char *szFileName
        KTrussIndex &oKTIndex
output:
        int
description:
        get index from file
******************/
int fileIO_getIndex(char *szFileName, KTrussIndex &oKTIndex, Graph *poG)
{
    map < pair < int, int >, int > mpTpSave;

    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    int iTrussness = 0;
    int iRet = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    bool bFirstLineFlag = true;

    assert(NULL != szFileName);

    fp = fopen(szFileName, "rt");

    if (NULL == fp)
    {
        printf("error no file: %s\n", szFileName);
        assert(0);
    }

    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
    {
        iRet = sscanf(szBuffer, "%d %d %d", &iTempX, &iTempY, &iTrussness);
        if (3 != iRet)
        {
            printf("error input: %s\n", szBuffer);
            continue;
        }

        if (bFirstLineFlag)
        {
            /* ignore first line */
            bFirstLineFlag = false;
            continue;
        }
        /* input is support */
        iTrussness += 2;

        if (iTempX == iTempY)
        {
            continue;
        }
        if (poG->hasEdge(iTempX, iTempY))
        {
            continue;
        }
        poG->addEdge(iTempX, iTempY);

        mpTpSave[pair<int, int>(iTempX, iTempY)] = iTrussness;
    }
    fclose(fp);

    oKTIndex.init(poG);
    for (auto &mpNode: mpTpSave)
    {
        oKTIndex.setTruss(mpNode.first.first, mpNode.first.second, mpNode.second);
    }

    printf("get map size: %d\n", mpTpSave.size());
    return 0;
}
/*****************
input:
        char *szFileName
        KTrussIndex &oKTIndex
output:
        int
description:
        save index from file
******************/
int fileIO_saveIndex(char *szFileName, KTrussIndex &oKTIndex)
{
    Graph *poG = oKTIndex.getGraph();

    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    int iTrussness = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};

    assert(NULL != szFileName);

    fp = fopen(szFileName, "wt");

    if (NULL == fp)
    {
        printf("error no file: %s\n", szFileName);
        assert(0);
    }

    for (Edge e: poG->getEdges())
    {
        iTempX = e.getX();
        iTempY = e.getY();
        iTrussness = oKTIndex.getTruss(e);
        fprintf(fp, "%d, %d, %d\n", iTempX, iTempY, iTrussness);
    }

    fclose(fp);

    return 0;
}
/*****************
input:
        char *szFileName
        list<pair<int, int> > &lsE
output:
        int
description:
        get edges from file
******************/
int fileIO_getEdges(char *szFileName, list<pair<int, int> > &lsE)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    int iRet = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};

    assert(NULL != szFileName);

    fp = fopen(szFileName, "rt");

    if (NULL == fp)
    {
        printf("error no file: %s\n", szFileName);
        assert(0);
    }

    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
    {
        iRet = sscanf(szBuffer, "%d %d", &iTempX, &iTempY);
        if (2 != iRet)
        {
            printf("error input: %s, maybe first line\n", szBuffer);
            continue;
        }

        if (iTempX == iTempY)
        {
            continue;
        }
        lsE.push_back(pair<int, int>(iTempX, iTempY));
    }

    printf("get list size: %d\n", lsE.size());
    return 0;
}
