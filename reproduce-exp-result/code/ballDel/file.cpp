/***************
input file output file function
****************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "myG.h"
#include "file.h"

int g_iOffset = 1;
/*****************
input:
        char *szFileName
        map<int, list<int> > &mpPrivate
description:
        read insert edge list
******************/
int file_readPrivate(char *szFileName, map<int, vector <int> > &mpPrivate, int *piMaxPid)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    vector <int> vNeighbor;
    int res = 0;
    int iNode = 0;
    int iLocalCnt = 0;
    int iCntMax = 0;
    int iMaxPid = 0;

    DEBUG_ASSERT(NULL != szFileName);
    DEBUG_ASSERT(mpPrivate.empty());

    fp = fopen(szFileName, "rt");
    if (NULL == fp)
    {
        printf("error no file: %s\n", szFileName);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) != NULL)
    {
        res = sscanf(szBuffer, "n %d", &iTempX);
        iTempX = iTempX + g_iOffset;

        if (1 == res)
        {
            /* root node */
            if (!vNeighbor.empty())
            {
                /* save */
                map<int, vector <int> >::iterator itmpNode;
                itmpNode = mpPrivate.find(iNode);
                /* empty */
                DEBUG_ASSERT(itmpNode == mpPrivate.end());

                mpPrivate[iNode].clear();
                mpPrivate[iNode].swap(vNeighbor);
                iCntMax = iCntMax>iLocalCnt?iCntMax:iLocalCnt;
                iLocalCnt = 0;
            }
            iNode = iTempX;
            vNeighbor.clear();
            //printf("READ node: %d\n", iNode);
        }
        else
        {
            res = sscanf(szBuffer, "%d %d", &iTempX, &iTempY);
            iTempX = g_iOffset + iTempX;
            iTempY = g_iOffset + iTempY;
            DEBUG_ASSERT(0 < iTempX);
            DEBUG_ASSERT(0 < iTempY);
            iMaxPid = iMaxPid>iTempX?iMaxPid:iTempX;
            iMaxPid = iMaxPid>iTempY?iMaxPid:iTempY;
            /* neighbor node */
            if (iNode == iTempX)
            {
                vNeighbor.push_back(iTempY);
                //printf("READ neighbor: %d\n", iTempY);
            }
            else if (iNode == iTempY)
            {
                vNeighbor.push_back(iTempX);
                //printf("READ neighbor: %d\n", iTempX);
            }
            ++iLocalCnt;
        }

    }
    if (!vNeighbor.empty())
    {
        /* save */
        map<int, vector <int> >::iterator itmpNode;
        itmpNode = mpPrivate.find(iNode);
        /* empty */
        DEBUG_ASSERT(itmpNode == mpPrivate.end());

        mpPrivate[iNode].clear();
        mpPrivate[iNode].swap(vNeighbor);
        iCntMax = iCntMax>iLocalCnt?iCntMax:iLocalCnt;
        iLocalCnt = 0;
    }
    fclose(fp);
    *piMaxPid = iMaxPid;
    return iCntMax;
}
/*****************
input:
        char *szFileName
        list<pair<int, int> > &lstQuery
description:
        read insert edge list
******************/
int file_readQuery(char *szFileName, vector <pair<int, int> > &vQuery)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    int res = 0;

    DEBUG_ASSERT(NULL != szFileName);
    DEBUG_ASSERT(vQuery.empty());

    fp = fopen(szFileName, "rt");
    if (NULL == fp)
    {
        printf("error no file: %s\n", szFileName);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) != NULL)
    {
        res = sscanf(szBuffer, "%d %d", &iTempX, &iTempY);
        iTempX = g_iOffset + iTempX;
        iTempY = g_iOffset + iTempY;

        DEBUG_ASSERT(2 == res);

        vQuery.push_back(pair<int, int>(iTempX, iTempY));

    }
    fclose(fp);
    return 0;
}
/*****************
input:
        myG &oMpG,
        LIST_DECOMP_G &lstDeG
        char *szTrussPath
output:
        none
description:
        save truss decomposition
******************/
int file_saveDeTruss(myG &oMpG, map<int, vector<int> > &mpDeG, char *pcIndexFile)
{
    map<int, vector<int> >::iterator itDeG;
    vector<int>::iterator itvE;
    FILE *fp = NULL;
    pair<int, int> paXY;

    fp = fopen(pcIndexFile, "wt");

    if (NULL == fp)
    {
        printf("error write file: %s\n", pcIndexFile);
        DEBUG_ASSERT(0);
    }

    for (itDeG = mpDeG.begin(); itDeG != mpDeG.end(); itDeG++)
    {
        fprintf(fp, "k,%d\n", itDeG->first);
        for (itvE = itDeG->second.begin(); itvE != itDeG->second.end(); ++itvE)
        {
            paXY = oMpG.findByEid(*itvE);
            fprintf(fp, "%d,%d\n",
                    paXY.first - g_iOffset, paXY.second - g_iOffset);

        }
    }
    fclose(fp);
    fp = NULL;
    return 0;
}
/*****************
input:
        myG &oMpG,
        LIST_DECOMP_G &lstDeG
        char *szTrussPath
output:
        none
description:
        save truss decomposition
******************/
int file_saveTrussness(myG &oMpG, char *pcIndexFile)
{
    map<int, vector<int> > mpDeG;
    TPST_MAP_BY_EID *pstTpNode = NULL;

    for (int eid = 1; eid <= oMpG.m_iMaxEId; ++eid)
    {
        pstTpNode = oMpG.findNode(eid);
        DEBUG_ASSERT(NULL != pstTpNode);
        if (eid != pstTpNode->eid)
        {
            /* removed */
            continue;
        }

        mpDeG[pstTpNode->iTrussness].push_back(pstTpNode->eid);
    }

    file_saveDeTruss(oMpG, mpDeG, pcIndexFile);
    return 0;
}
/*****************
input:
        myG &oMpG
        char *szGPath
output:
        none
description:
        save truss decomposition
******************/
int file_saveBitG(myG &oMpG, char *pcFile)
{
    char sFirstLine[100] = {0};
    char *pcData = NULL;
    SaveEntry *pstData = NULL;
    int uMaxBuffer = 100000000;

    pcData = (char *)malloc(oMpG.m_iMaxEId * sizeof(SaveEntry));
    DEBUG_ASSERT(NULL != pcData);
    pstData = (SaveEntry *)pcData;
    int iRealEid = 0;
    TPST_MAP_BY_EID *pstNode = NULL;
    for (int eid = 1; eid <= oMpG.m_iMaxEId; ++eid)
    {
        pstNode = oMpG.findNode(eid);
        if (eid != pstNode->eid)
        {
            /* removed */
            continue;
        }
        pstData[iRealEid].x = pstNode->paXY.first - g_iOffset;
        pstData[iRealEid].y = pstNode->paXY.second - g_iOffset;
        pstData[iRealEid].k = pstNode->iTrussness;
        pstData[iRealEid].L = pstNode->iLayer;
        pstData[iRealEid].SeSup = pstNode->iSeSup;

        ++iRealEid;
    }

    sprintf(sFirstLine, "c,%d\nP,%d\nE,%d\nD,%d\nK,%d\n",
            oMpG.m_iMaxEId + 1000,
            oMpG.m_iMaxPId - g_iOffset,
            iRealEid,
            oMpG.m_iMaxD,
            oMpG.m_iMaxK);

    /* write */
    int fd = open(pcFile, O_CREAT|O_WRONLY|O_TRUNC, 0644);

    int ret = write(fd, sFirstLine, 100 * sizeof(char));
    DEBUG_ASSERT(100 == ret);

    long long llCurPos = 0;
    long long llTotoalSize = oMpG.m_iMaxEId * sizeof(SaveEntry);
    long long uStep = llTotoalSize;
    while (llCurPos < llTotoalSize)
    {
        if (uStep <= uMaxBuffer)
        {
            ret = write(fd, pcData + llCurPos, uStep * sizeof(char));
            DEBUG_ASSERT(uStep == ret);
            llCurPos += uStep;

            uStep = uStep < (llTotoalSize - llCurPos)? uStep : (llTotoalSize - llCurPos);
        }
        else
        {
            uStep = uMaxBuffer;
            DEBUG_ASSERT(1 < uStep);
        }
    }
    close(fd);
    free(pcData);
    pcData = NULL;
    pstData = NULL;
    return 0;
}
/*****************
input:
        char *szFileName
        myG &mpG
description:
        read file and fill the list g_initG
******************/
int file_readBitG(myG &oMpG, char *pcFile)
{
    int iCnt = 0;
    TPST_MAP_BY_EID stTpNode = {0};
    TPST_MAP_BY_EID *pstTpNode = NULL;
    char sFirstLine[100] = {0};
    char *pcData = NULL;
    SaveEntry *pstData = NULL;
    int uMaxBuffer = 100000000;
    //printf("start save file %s\n", sFile);

    /* read */
    int fd = open(pcFile, O_RDONLY);
    DEBUG_ASSERT(-1 != fd);
    int ret = read(fd, sFirstLine, 100 * sizeof(char));
    DEBUG_ASSERT(100 == ret);
    int i_c, i_P, i_m, i_D, i_K;
    ret = sscanf(sFirstLine, "c,%d\nP,%d\nE,%d\nD,%d\nK,%d\n", &i_c, &i_P, &i_m, &i_D, &i_K);
    DEBUG_ASSERT(5 == ret);

    /* fill data */
    pcData = (char *)malloc(i_m * sizeof(SaveEntry));
    DEBUG_ASSERT(NULL != pcData);
    pstData = (SaveEntry *)pcData;

    long long llCurPos = 0;
    long long llTotoalSize = i_m * sizeof(SaveEntry);
    long long uStep = llTotoalSize;
    while (llCurPos < llTotoalSize)
    {
        if (uStep <= uMaxBuffer)
        {
            ret = read(fd, pcData + llCurPos, uStep * sizeof(char));
            if (uStep != ret)
            {
                printf("Error read %d get %d\n", uStep, ret);
                DEBUG_ASSERT(0);
            }
            llCurPos += uStep;

            uStep = uStep < (llTotoalSize - llCurPos)? uStep : (llTotoalSize - llCurPos);
        }
        else
        {
            uStep = uMaxBuffer;
            DEBUG_ASSERT(1 < uStep);
        }
    }
    close(fd);

    /* first line */
    oMpG.m_pvG->clear();
    DEBUG_ASSERT(0 < i_c);
    oMpG.m_pvG->reserve(i_c);
    i_P = i_P + g_iOffset;
    oMpG.m_iMaxPId = i_P;
    oMpG.m_vAdj.resize(i_P + 1);
    oMpG.m_iMaxEId = i_m;
    oMpG.m_iMaxD = i_D;
    oMpG.m_iMaxK = i_K;

    /* all */
    /* first */
    pstTpNode = oMpG.add(0);
    for (int i = 0; i < i_m; ++i)
    {
        int eid = i + 1;
        pstTpNode = oMpG.add(eid);
        DEBUG_ASSERT(NULL != pstTpNode);
        DEBUG_ASSERT(eid == pstTpNode->eid);
        ++iCnt;

        pstTpNode->iTrussness = pstData[i].k;
        pstTpNode->iLayer = pstData[i].L;
        pstTpNode->iSeSup = pstData[i].SeSup;

        int x = pstData[i].x + g_iOffset;
        pstTpNode->paXY.first = x;
        DEBUG_ASSERT(x <= oMpG.m_iMaxPId);
        int y = pstData[i].y + g_iOffset;
        pstTpNode->paXY.second = y;
        if (0 < y)
        {
            oMpG.m_vAdj[x].push_back({y, eid});
            oMpG.m_vAdj[y].push_back({x, eid});
        }
        DEBUG_ASSERT(y <= oMpG.m_iMaxPId);
    }

    free(pcData);
    pcData = NULL;
    pstData = NULL;
    return iCnt;
}
