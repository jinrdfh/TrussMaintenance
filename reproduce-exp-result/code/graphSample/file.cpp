/***************
input file output file function
****************/

#include "common.h"
#include "myG.h"
#include "file.h"

int g_iOffset = 1;
/*****************
input:
        char *szFileName
        myG &mpG
description:
        read file and fill the list g_initG
******************/
int file_fillG(myG &mpG, char *szFileName)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    int iCnt = 0;
    bool res = false;

    DEBUG_ASSERT(NULL != szFileName);
    fp = fopen(szFileName, "rt");
    if (NULL == fp)
    {
        printf("error no file: %s\n", szFileName);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
    {
        sscanf(szBuffer, "%d%d", &iTempX, &iTempY);
        iTempX = g_iOffset + iTempX;
        iTempY = g_iOffset + iTempY;
        DEBUG_ASSERT(0 < iTempX);
        DEBUG_ASSERT(0 < iTempY);

        if (iTempX == iTempY)
        {
            printf("error loop: %s\n", szBuffer);
            continue;
        }
        res = mpG.add(iTempX, iTempY);
        if (res)
        {
            ++iCnt;
        }
    }
    fclose(fp);
    return iCnt;
}

/*****************
input:
        myG &oMpG
        list <pair<int, int> > &lsE
        char *pcFileName
output:
        none
description:
        save graph
******************/
int file_saveGraph(myG &oMpG, vector <pair<int, int> > &vEdge, char *pcFileName)
{
    vector <pair<int, int> >::iterator itE;
    FILE *fp = NULL;

    fp = fopen(pcFileName, "wt");

    if (NULL == fp)
    {
        printf("error write file: %s\n", pcFileName);
        DEBUG_ASSERT(0);
    }

    for (itE = vEdge.begin(); itE != vEdge.end(); ++itE)
    {
        fprintf(fp, "%d %d\n",
                itE->first - g_iOffset, itE->second - g_iOffset);
    }
    fclose(fp);
    fp = NULL;
    return 0;
}
/*****************
input:
        myG &oMpG
        list <pair<int, int> > &lsE
        char *pcFileName
output:
        none
description:
        save graph
******************/
int file_savePriGraph(myG &oMpG, int iRoot, int iId, vector <pair<int, int> > &vEdge, char *pcPriPath)
{
    vector <pair<int, int> >::iterator itE;
    FILE *fp = NULL;
    char szFileName[FILE_NAME_BUFFER] = {0};

    sprintf(szFileName, "%s/private_%d.txt", pcPriPath, iId);

    fp = fopen(szFileName, "wt");

    if (NULL == fp)
    {
        printf("error write file: %s\n", szFileName);
        DEBUG_ASSERT(0);
    }

    fprintf(fp, "n %d\n", iRoot - g_iOffset);
    for (itE = vEdge.begin(); itE != vEdge.end(); ++itE)
    {
        fprintf(fp, "%d %d\n",
                itE->first - g_iOffset, itE->second - g_iOffset);
    }
    fclose(fp);
    fp = NULL;
    return 0;
}




