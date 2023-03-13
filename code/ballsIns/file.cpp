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
#if 0
/*****************
input:
        char *szFileName
        myG &mpG
description:
        read file and fill the list g_initG
******************/
int file_fillG(char *szFileName, myG &mpG)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    int iCnt = 0;

    if (NULL == szFileName)
    {
        fp = fopen(DATA_PATH, "rt");
    }
    else
    {
        fp = fopen(szFileName, "rt");
    }
    if (NULL == fp)
    {
        printf("error no file: %s or %s\n", DATA_PATH, szFileName);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
    {
        sscanf(szBuffer, "%d %d", &iTempX, &iTempY);
        iTempX = g_iOffset + iTempX;
        iTempY = g_iOffset + iTempY;
        DEBUG_ASSERT(0 < iTempX);
        DEBUG_ASSERT(0 < iTempY);

        if (iTempX == iTempY)
        {
            continue;
        }

        //printf("DEBUG add new edge: (%d, %d)\n", iTempX, iTempY);
        mpG.add(iTempX, iTempY);
        ++iCnt;
    }
    printf("READ get edges: %d\n", iCnt);
    fclose(fp);
    return iCnt;
}
#endif
/*****************
input:
        char *szFileName
        map<int, list<int> > &mpPrivate
description:
        read insert edge list
******************/
int file_readPrivate(char *pcFile, map<int, vector <int> > &mpPrivate, int *piMaxPid)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    vector <int> vNeighbor;
    int res = 0;
    int iNode = 0;
    int iLocalCnt = 0;
    //int iCntMax = 0;
    int iMaxPid = 0;

    DEBUG_ASSERT(NULL != pcFile);
    DEBUG_ASSERT(mpPrivate.empty());

    fp = fopen(pcFile, "rt");
    if (NULL == fp)
    {
        printf("error no file: %s\n", pcFile);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
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

                if (itmpNode != mpPrivate.end())
                {
                    itmpNode->second.insert(itmpNode->second.end(),
                                            vNeighbor.begin(), vNeighbor.end());
                    COMMON_UNIQUE(itmpNode->second);
                }
                else
                {
                    /* empty */
                    //mpPrivate[iNode].clear();
                    mpPrivate[iNode].swap(vNeighbor);

                    //iCntMax = COMMON_MAX(iCntMax, iLocalCnt);
                    //iLocalCnt = 0;
                }
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
        if (itmpNode != mpPrivate.end())
        {
            itmpNode->second.insert(itmpNode->second.end(),
                                    vNeighbor.begin(), vNeighbor.end());
            COMMON_UNIQUE(itmpNode->second);
        }
        else
        {
            /* empty */
            //mpPrivate[iNode].clear();
            mpPrivate[iNode].swap(vNeighbor);

            //iCntMax = COMMON_MAX(iCntMax, iLocalCnt);
            //iLocalCnt = 0;
        }
    }
    fclose(fp);
    *piMaxPid = iMaxPid;
    return iLocalCnt;
}
/*****************
input:
        char *szFileName
        list<pair<int, int> > &lstQuery
description:
        read insert edge list
******************/
int file_readQuery(char *pcFile, list<pair<int, int> > &lstQuery)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    int res = 0;

    DEBUG_ASSERT(NULL != pcFile);
    DEBUG_ASSERT(lstQuery.empty());

    fp = fopen(pcFile, "rt");
    if (NULL == fp)
    {
        printf("error no file: %s\n", pcFile);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
    {
        res = sscanf(szBuffer, "%d %d", &iTempX, &iTempY);
        iTempX = g_iOffset + iTempX;
        iTempY = g_iOffset + iTempY;

        DEBUG_ASSERT(2 == res);

        lstQuery.push_back(pair<int, int>(iTempX, iTempY));

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
int file_saveDeTruss(myG &oMpG, LIST_DECOMP_G &lstDeG, char *pcFile)
{
    LIST_DECOMP_G::iterator itDeG;
    list<TPST_DG_NODE>::iterator itlst;
    FILE *fp = NULL;
    pair<int, int> paXY;
    TPST_MAP_BY_EID *pstNode = NULL;

    fp = fopen(pcFile, "wt");

    if (NULL == fp)
    {
        printf("error write file: %s\n", pcFile);
        DEBUG_ASSERT(0);
    }

    for (itDeG = lstDeG.begin(); itDeG != lstDeG.end(); itDeG++)
    {
        fprintf(fp, "k,%d\n", itDeG->iTrussness);
        for (itlst = itDeG->lsENode.begin(); itlst != itDeG->lsENode.end(); ++itlst)
        {
            pstNode = oMpG.findNode(itlst->iEid);
            //paXY = oMpG.findByEid(itlst->iEid);
            fprintf(fp, "%d,%d\n",
                    pstNode->paXY.first - g_iOffset, pstNode->paXY.second - g_iOffset);

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
int file_saveTrussness(myG &oMpG, char *pcFile)
{
    //MAP_BASIC_G::iterator itMpG;
    LIST_DECOMP_G lstDeG;
    int x = 0;
    int y = 0;
    int iDebugCnt = 0;
    int iDebugNum = 100;
    TPST_MAP_BY_EID *pstTpNode = NULL;
    TPST_DECOMP_G stTpNode;
    TPST_DG_NODE stENode;

    map<int, list<TPST_DG_NODE> > mpTrussness;
    map<int, list<TPST_DG_NODE> >::iterator itmpT;

    //printf("DEBUT enter save trussness size: %d\n", oMpG.m_mpBasicG.size());
    for (int eid = 1; eid <= oMpG.m_iMaxEId; ++eid)
    {
        pstTpNode = oMpG.findNode(eid);
        DEBUG_ASSERT(NULL != pstTpNode);
        if (eid != pstTpNode->eid)
        {
            continue;
        }

        /*if (0 == iDebugCnt % iDebugNum)
        {
            printf("DEBUG cnt: %d get (%d, %d)\n", iDebugCnt, x, y);
        }*/
        stENode.iEid = pstTpNode->eid;
        stENode.iLayer = pstTpNode->iLayer;
        mpTrussness[pstTpNode->iTrussness].push_back(stENode);
        /*if (0 == iDebugCnt % iDebugNum)
        {
            printf("DEBUG trussness: %d insert eid: %d\n", pstTpNode->iTrussness,
                   pstTpNode->eid);
        }

        ++iDebugCnt;*/
    }
    //printf("DEBUT construct detruss\n");
    for (itmpT = mpTrussness.begin(); itmpT != mpTrussness.end(); itmpT++)
    {
        stTpNode.iTrussness = itmpT->first;
        stTpNode.lsENode.swap(itmpT->second);
        lstDeG.push_back(stTpNode);
    }
    //printf("DEBUT save detruss\n");
    file_saveDeTruss(oMpG, lstDeG, pcFile);
    return 0;
}
#if 0
/*****************
input:
        myG &oMpG
        char *szGPath
output:
        none
description:
        save truss decomposition
******************/
int file_saveG(myG &oMpG, char *szGPath, bool bFinal)
{
    vector<TPST_MAP_BY_EID>::iterator itDeG;
    vector<int>::iterator itE;
    FILE *fp = NULL;
    char szFileName[FILE_NAME_BUFFER] = {0};

    if (bFinal)
    {
        sprintf(szFileName, "%s/%s-result.myG", szGPath, g_pcDataLabel);
    }
    else
    {
        sprintf(szFileName, "%s/%s.myG", szGPath, g_pcDataLabel);
    }
    fp = fopen(szFileName, "wt");

    if (NULL == fp)
    {
        printf("error write file: %s\n", szFileName);
        DEBUG_ASSERT(0);
    }

    fprintf(fp, "c,%ld\n", oMpG.m_iMaxEId + 1000);
    fprintf(fp, "P,%ld\n", oMpG.m_iMaxPId - g_iOffset);
    fprintf(fp, "E,%ld\n", oMpG.m_iMaxEId);
    fprintf(fp, "D,%ld\n", oMpG.m_iMaxD);
    fprintf(fp, "K,%ld\n", oMpG.m_iMaxK);
    for (itDeG = oMpG.m_pvG->begin(); itDeG != oMpG.m_pvG->end(); ++itDeG)
    {
        fprintf(fp, "n,%ld\n", itDeG->eid);
        //fprintf(fp, "s,%d\n", itDeG->iSup);
        //fprintf(fp, "J,%d\n", itDeG->iJuSup);
        fprintf(fp, "S,%d\n", itDeG->iSeSup);
        fprintf(fp, "t,%d\n", itDeG->iTrussness);
        fprintf(fp, "L,%d\n", itDeG->iLayer);
        //fprintf(fp, "a,%d\n", itDeG->iAlleyType);
        fprintf(fp, "x,%d\n", itDeG->paXY.first - g_iOffset);
        fprintf(fp, "y,%d\n", itDeG->paXY.second - g_iOffset);
        /*for (itE = itDeG->vLfE.begin(); itE != itDeG->vLfE.end(); ++itE)
        {
            fprintf(fp, "l,%d\n", *itE);
        }
        for (itE = itDeG->vRtE.begin(); itE != itDeG->vRtE.end(); ++itE)
        {
            fprintf(fp, "r,%d\n", *itE);
        }*/
    }
    fclose(fp);
    fp = NULL;
    return 0;
}
#endif
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
    uint32_t uMaxBuffer = 100000000;

    /* fill data */
    sprintf(sFirstLine, "c,%d\nP,%d\nE,%d\nD,%d\nK,%d\n",
            oMpG.m_iMaxEId + 1000,
            oMpG.m_iMaxPId - g_iOffset,
            oMpG.m_iMaxEId,
            oMpG.m_iMaxD,
            oMpG.m_iMaxK);
    pcData = (char *)malloc(oMpG.m_iMaxEId * sizeof(SaveEntry));
    DEBUG_ASSERT(NULL != pcData);
    pstData = (SaveEntry *)pcData;
    for (uint32_t eid = 0; eid < oMpG.m_iMaxEId; ++eid)
    {
        pstData[eid].x = oMpG.m_pvG->at(eid + 1).paXY.first - g_iOffset;
        pstData[eid].y = oMpG.m_pvG->at(eid + 1).paXY.second - g_iOffset;
        pstData[eid].k = oMpG.m_pvG->at(eid + 1).iTrussness;
        pstData[eid].L = oMpG.m_pvG->at(eid + 1).iLayer;
        pstData[eid].SeSup = oMpG.m_pvG->at(eid + 1).iSeSup;
    }

    /* write */
    int fd = open(pcFile, O_CREAT|O_WRONLY|O_TRUNC, 0644);

    int ret = write(fd, sFirstLine, 100 * sizeof(char));
    DEBUG_ASSERT(100 == ret);

    long long llCurPos = 0;
    long long llTotoalSize = oMpG.m_iMaxEId * sizeof(SaveEntry);
    uint32_t uStep = llTotoalSize;
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
#if 0
/*****************
input:
        char *szFileName
        myG &mpG
description:
        read file and fill the list g_initG
******************/
int file_readG(myG &oMpG, char *szGPath)
{
    FILE *fp = NULL;
    int iTempX = 0;
    int iTempY = 0;
    int iCapacity = 0;
    char cTpType;
    char szBuffer[ONE_LINE_BUFFER] = {0};
    int iCnt = 0;
    char szFileName[FILE_NAME_BUFFER] = {0};
    TPST_MAP_BY_EID stTpNode = {0};
    TPST_MAP_BY_EID *pstTpNode = NULL;

    sprintf(szFileName, "%s/%s.myG", szGPath, g_pcDataLabel);

    fp = fopen(szFileName, "rt");

    if (NULL == fp)
    {
        printf("error no file: %s or %s\n", DATA_PATH, szFileName);
        DEBUG_ASSERT(0);
    }
    while (fgets(szBuffer, ONE_LINE_BUFFER - 1, fp) > 0)
    {
        sscanf(szBuffer, "%c,%ld", &cTpType, &iTempX);

        switch(cTpType)
        {
            case 'c':
                oMpG.m_pvG->clear();
                if (iTempX > 0)
                {
                    oMpG.m_pvG->reserve(iTempX);
                }
                else
                {
                    /* invalid value */
                    iCapacity = 0;
                }
                break;
            case 'P':
                iTempX = iTempX + g_iOffset;
                oMpG.m_iMaxPId = iTempX;
                oMpG.m_pvPNeCnt->reserve(iTempX + 1);
                oMpG.m_pvPNeCnt->resize(iTempX + 1);
                //printf("DEBUG get neighbor size: %d\n", iTempX);
                break;
            case 'E':
                oMpG.m_iMaxEId = iTempX;
                if (0 == iCapacity)
                {
                    /* set capacity by max EId */
                    iCapacity = oMpG.m_iMaxEId + 1;
                    oMpG.m_pvG->reserve(iCapacity);
                }
                break;
            case 'D':
                oMpG.m_iMaxD = iTempX;
                break;
            case 'K':
                oMpG.m_iMaxK = iTempX;
                break;
            case 'n':
                stTpNode.eid = iTempX;
                //stTpNode.bNewFlag = false;
                oMpG.m_pvG->push_back(stTpNode);
                pstTpNode = &((*(oMpG.m_pvG))[iTempX]);
                DEBUG_ASSERT(NULL != pstTpNode);
                DEBUG_ASSERT(stTpNode.eid == pstTpNode->eid);
                ++iCnt;
                break;
            case 's':
                //pstTpNode->iSup = iTempX;
                break;
            case 'J':
                //pstTpNode->iJuSup = iTempX;
                break;
            case 'S':
                pstTpNode->iSeSup = iTempX;
                break;
            case 't':
                pstTpNode->iTrussness = iTempX;
                break;
            case 'L':
                pstTpNode->iLayer = iTempX;
                break;
            case 'a':
                //pstTpNode->iAlleyType = iTempX;
                break;
            case 'x':
                iTempX = iTempX + g_iOffset;
                pstTpNode->paXY.first = iTempX;
                if (0 < iTempX)
                {
                    (*(oMpG.m_pvPNeCnt))[iTempX]++;
                }
                /*if (iTempX >= oMpG.m_pvPNeCnt->size())
                {
                    printf("DEBUG get pid: %d, size: %d\n",
                           iTempX, oMpG.m_pvPNeCnt->size());
                    DEBUG_ASSERT(0);
                }*/
                DEBUG_ASSERT(iTempX <= oMpG.m_iMaxPId);
                break;
            case 'y':
                iTempX = iTempX + g_iOffset;
                pstTpNode->paXY.second = iTempX;
                if (0 < iTempX)
                {
                    (*(oMpG.m_pvPNeCnt))[iTempX]++;
                    oMpG.m_mpBasicG[pstTpNode->paXY] = pstTpNode->eid;
                    oMpG.m_mpBasicG[pair<int, int>(pstTpNode->paXY.second, pstTpNode->paXY.first)] = pstTpNode->eid;
                    oMpG.addNeibTri(pstTpNode->eid);
                }
                DEBUG_ASSERT(iTempX <= oMpG.m_iMaxPId);
                break;
            /*case 'l':
                pstTpNode->vLfE.push_back(iTempX);
                break;
            case 'r':
                pstTpNode->vRtE.push_back(iTempX);
                break;*/
            default:
                printf("FILE ERROR %c\n", cTpType);
        }
    }
    fclose(fp);
    return iCnt;
}
#endif
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
    uint32_t uMaxBuffer = 100000000;

    /* read */
    int fd = open(pcFile, O_RDONLY);
    DEBUG_ASSERT(-1 != fd);
    int ret = read(fd, sFirstLine, 100 * sizeof(char));
    //printf("read file %s\n", sFirstLine);
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
    uint32_t uStep = llTotoalSize;
    while (llCurPos < llTotoalSize)
    {
        if (uStep <= uMaxBuffer)
        {
            ret = read(fd, pcData + llCurPos, uStep * sizeof(char));
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

    /* first line */
    oMpG.m_pvG->clear();
    DEBUG_ASSERT(0 < i_c);
    oMpG.m_pvG->reserve(i_c);
    i_P = i_P + g_iOffset;
    oMpG.m_iMaxPId = i_P;
    oMpG.m_pvPNeCnt->resize(i_P + 1);
    oMpG.m_vAdj.resize(i_P + 1);
    oMpG.m_iMaxEId = i_m;
    oMpG.m_iMaxD = i_D;
    oMpG.m_iMaxK = i_K;

    /* all */
    /* first empty node */
    oMpG.m_pvG->push_back(stTpNode);
    for (int i = 0; i < i_m; ++i)
    {
        int eid = i + 1;
        stTpNode.eid = eid;
        //stTpNode.bNewFlag = false;
        oMpG.m_pvG->push_back(stTpNode);
        pstTpNode = &((*(oMpG.m_pvG))[eid]);
        DEBUG_ASSERT(NULL != pstTpNode);
        DEBUG_ASSERT(stTpNode.eid == pstTpNode->eid);
        ++iCnt;

        pstTpNode->iTrussness = pstData[i].k;
        pstTpNode->iLayer = pstData[i].L;
        pstTpNode->iSeSup = pstData[i].SeSup;
        pstTpNode->bInit = false;

        int x = pstData[i].x + g_iOffset;
        pstTpNode->paXY.first = x;
        if (0 < x)
        {
            (*(oMpG.m_pvPNeCnt))[x]++;
        }
        DEBUG_ASSERT(x <= oMpG.m_iMaxPId);
        int y = pstData[i].y + g_iOffset;
        pstTpNode->paXY.second = y;
        if (0 < y)
        {
            (*(oMpG.m_pvPNeCnt))[y]++;
            oMpG.m_vAdj[x].push_back({y, eid});
            oMpG.m_vAdj[y].push_back({x, eid});
            //oMpG.m_mpBasicG[pstTpNode->paXY] = pstTpNode->eid;
            //oMpG.m_mpBasicG[pair<int, int>(pstTpNode->paXY.second, pstTpNode->paXY.first)] = pstTpNode->eid;
            //oMpG.addNeibTri(pstTpNode->eid);
        }
        DEBUG_ASSERT(y <= oMpG.m_iMaxPId);
    }

    free(pcData);
    pcData = NULL;
    pstData = NULL;
    return iCnt;
}
