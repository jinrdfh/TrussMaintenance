/***************
insertNode function
****************/

#include <unordered_map>

#include "common.h"
#include "myG.h"
#include "insertPart.h"

extern long g_lSimpleAddTime;
extern long g_lFillInfoTime;
extern long g_lBatchInsTotalTime;
extern long g_lBallDecTime;
extern long g_lLocalDecTime;
extern long g_lEdgeDecTime;
extern long g_lOutBallTime;
extern long g_lAllKTime;
extern long g_lRmQTime;


extern long g_lNeibCnt;
extern long g_lUsefulCnt;

long g_lBFSCnt;
long g_lUgCnt;
long g_lBallSize;
/*****************
input:
        myG &mpG
        int iSelfId
        int iLtId
        int iRtId
description:
        check whether increase seSup
******************/
bool insertPart::checkInsSeSup(myG &mpG, int iSelfId, int iLtId, int iRtId)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;

    pstNode = mpG.findNode(iSelfId);
    //DEBUG_ASSERT(NULL != pstNode);
    pstLfNode = mpG.findNode(iLtId);
    //DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = mpG.findNode(iRtId);
    //DEBUG_ASSERT(NULL != pstRtNode);

    if ((pstLfNode->iTrussness != pstNode->iTrussness) ||
        (pstLfNode->iLayer <= pstNode->iLayer))
    {
        return false;
    }
    if (pstLfNode->iLayer <= pstRtNode->iLayer)
    {
        if (pstRtNode->iTrussness > pstNode->iTrussness)
        {
            return true;
        }
        else if (pstRtNode->iTrussness == pstNode->iTrussness)
        {
            if (pstRtNode->iLayer > pstNode->iLayer)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (pstRtNode->iTrussness > pstNode->iTrussness)
        {
            return true;
        }
        else if (pstRtNode->iTrussness < pstNode->iTrussness)
        {
            return false;
        }
        else
        {
            if (pstRtNode->iLayer > pstNode->iLayer)
            {
                return false;
            }
            else if (pstRtNode->iLayer < pstNode->iLayer)
            {
                if (pstRtNode->bDoneQFlag)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if (pstRtNode->eid < pstNode->eid)
                {
                    if ((pstRtNode->bVisQFlag) &&
                        (pstRtNode->bUsedDoneFlag) &&
                        (pstRtNode->bInsLFlag))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    if ((pstRtNode->bCanQFlag) ||
                        (pstRtNode->bDoneQFlag) ||
                        ((pstRtNode->bVisQFlag) &&
                         (pstRtNode->bUsedDoneFlag)))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }
    return false;
}
/*****************
input:
        myG &mpG
        int iSelfId
        int iLtId
        int iRtId
description:
        check whether decrease seSup
******************/
bool insertPart::checkDesSeSup(myG &mpG, int iSelfId, int iLtId, int iRtId)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;

    pstNode = mpG.findNode(iSelfId);
    DEBUG_ASSERT(NULL != pstNode);
    pstLfNode = mpG.findNode(iLtId);
    DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = mpG.findNode(iRtId);
    DEBUG_ASSERT(NULL != pstRtNode);

    if (pstNode->bUsedDoneFlag)
    {
        if (pstLfNode->iTrussness != pstNode->iTrussness)
        {
            return false;
        }
        if (pstLfNode->iLayer <= pstNode->iLayer)
        {
            if (pstLfNode->bCanQFlag || pstLfNode->bDoneQFlag)
            {
                if (pstRtNode->iTrussness < pstNode->iTrussness)
                {
                    return false;
                }
                else if (pstRtNode->iTrussness > pstNode->iTrussness)
                {
                    return true;
                }
                else
                {
                    if (pstRtNode->iLayer < pstNode->iLayer)
                    {
                        if (pstRtNode->bDoneQFlag)
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else if (pstRtNode->iLayer > pstNode->iLayer)
                    {
                        return true;
                    }
                    else
                    {
                        if (pstRtNode->bCanQFlag ||
                            pstRtNode->bDoneQFlag ||
                            (pstRtNode->bVisQFlag &&
                             pstRtNode->bUsedDoneFlag))
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else if (pstLfNode->iLayer - 1 == pstNode->iLayer)
        {
            if (pstRtNode->iTrussness > pstNode->iTrussness)
            {
                return true;
            }
            else if (pstRtNode->iTrussness == pstNode->iTrussness)
            {
                if (pstRtNode->iLayer > pstNode->iLayer)
                {
                    return true;
                }
                else if (pstRtNode->iLayer < pstNode->iLayer)
                {
                    if (pstRtNode->bDoneQFlag)
                    {
                        return true;
                    }
                    return false;
                }
                else
                {
                    if (pstRtNode->bCanQFlag ||
                        pstRtNode->bDoneQFlag ||
                        (pstRtNode->bVisQFlag &&
                         pstRtNode->bUsedDoneFlag))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            /** pstLfNode->iLayer - 1 > pstNode->iLayer */
            if (pstRtNode->iTrussness > pstNode->iTrussness)
            {
                return true;
            }
            else if (pstRtNode->iTrussness == pstNode->iTrussness)
            {
                if (pstRtNode->iLayer > pstNode->iLayer)
                {
                    if (pstLfNode->bCanQFlag)
                    {
                        if (pstRtNode->bCanQFlag)
                        {
                            if (pstLfNode->iLayer > pstRtNode->iLayer)
                            {
                                return false;
                            }
                            else
                            {
                                return true;
                            }
                        }
                        else if (pstRtNode->bVisQFlag && pstRtNode->bUsedDoneFlag)
                        {
                            return true;
                        }
                        else
                        {
                            if (pstLfNode->iLayer > pstRtNode->iLayer)
                            {
                                return false;
                            }
                            else
                            {
                                return true;
                            }
                        }
                    }
                    else if (pstLfNode->bVisQFlag && pstLfNode->bUsedDoneFlag)
                    {
                        if (pstRtNode->bCanQFlag ||
                            (pstRtNode->bVisQFlag && pstRtNode->bUsedDoneFlag))
                        {
                            return true;
                        }
                        else
                        {
                            if (pstLfNode->bInsLFlag)
                            {
                                if (pstLfNode->iLayer - 1 == pstRtNode->iLayer)
                                {
                                    return true;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                if (pstLfNode->iLayer > pstRtNode->iLayer)
                                {
                                    return false;
                                }
                                else
                                {
                                    return true;
                                }
                            }
                        }
                    }
                    else
                    {
                        /** none or visited queue not from done queue */
                        if (pstLfNode->iLayer > pstRtNode->iLayer)
                        {
                            if (pstRtNode->bVisQFlag && pstRtNode->bUsedDoneFlag)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            return true;
                        }
                    }
                }
                else if (pstRtNode->iLayer < pstNode->iLayer)
                {
                    if (pstRtNode->bDoneQFlag)
                    {
                        return true;
                    }
                    return false;
                }
                else
                {
                    if (pstRtNode->bCanQFlag ||
                        pstRtNode->bDoneQFlag ||
                        (pstRtNode->bVisQFlag &&
                         pstRtNode->bUsedDoneFlag))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else
            {
                return false;
            }
        }
    }
    else
    {
        if (pstLfNode->iTrussness != pstNode->iTrussness)
        {
            --g_lUsefulCnt;
            return false;
        }
        if (pstLfNode->iLayer <= pstNode->iLayer)
        {
            if (pstLfNode->bCanQFlag || pstLfNode->bDoneQFlag)
            {
                if (pstRtNode->iTrussness < pstNode->iTrussness)
                {
                    return false;
                }
                else if (pstRtNode->iTrussness > pstNode->iTrussness)
                {
                    return true;
                }
                else
                {
                    if (pstRtNode->iLayer < pstNode->iLayer)
                    {
                        if (pstRtNode->bDoneQFlag)
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else if (pstRtNode->iLayer > pstNode->iLayer)
                    {
                        return true;
                    }
                    else
                    {
                        if (pstRtNode->bCanQFlag ||
                            pstRtNode->bDoneQFlag ||
                            (pstRtNode->bVisQFlag &&
                             pstRtNode->bUsedDoneFlag))
                        {
                            return true;
                        }
                        else
                        {
                            if (pstRtNode->eid > pstNode->eid)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }
            }
            else
            {
                return false;
            }
        }
        else if (pstLfNode->iLayer - 1 == pstNode->iLayer)
        {
            if (pstLfNode->bVisQFlag &&
                pstLfNode->bUsedDoneFlag &&
                pstLfNode->bInsLFlag)
            {
                if (pstRtNode->iTrussness < pstNode->iTrussness)
                {
                    return false;
                }
                else if (pstRtNode->iTrussness > pstNode->iTrussness)
                {
                    return true;
                }
                else
                {
                    if (pstRtNode->iLayer < pstNode->iLayer)
                    {
                        if (pstRtNode->bDoneQFlag)
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else if (pstRtNode->iLayer > pstNode->iLayer)
                    {
                        return true;
                    }
                    else
                    {
                        if (pstRtNode->bCanQFlag ||
                            pstRtNode->bDoneQFlag ||
                            (pstRtNode->bVisQFlag &&
                             pstRtNode->bUsedDoneFlag))
                        {
                            return true;
                        }
                        else
                        {
                            if (pstRtNode->eid > pstNode->eid)
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                }

            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

/*****************
input:
        myG &mpG
        int iCurK
        int iCurL
        myPriQueue &myVstQ
description:
        rm edge in visited Queue
******************/
int insertPart::rmEInVisQEasy(myG &mpG, int iCurK, int iCurL, myPriQueue &myVstQ)
{
	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    int iCurEid = 0;
    int iMinT = 0;
    bool bLeftFlag = false;
    bool bRightFlag = false;
    int iState = 0;
    int iMinLayer = 0;
    int iCnt = 0;
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;

	bool res = false;
    while (!myVstQ.empty())
    {
        iCurEid = myVstQ.getTop();
        pstNode = mpG.findNode(iCurEid);
        DEBUG_ASSERT(NULL != pstNode);

        if (!pstNode->bVisQFlag)
        {
            /* should be removed */
            myVstQ.pop();
            pstNode->bVisQFlag = false;
            pstNode->bUsedVisFlag = false;
            continue;
        }

        if (myVstQ.mergeKey(pstNode->iLayer, pstNode->bUsedDoneFlag) != myVstQ.getDy())
        {
            /* appear later */
            DEBUG_ASSERT(myVstQ.mergeKey(pstNode->iLayer, pstNode->bUsedDoneFlag) > myVstQ.getDy());
            myVstQ.pop();
            myVstQ.insetByOrder(myVstQ.mergeKey(pstNode->iLayer, pstNode->bUsedDoneFlag),
                                   pstNode->eid);
            continue;
        }

        if (pstNode->iLayer > iCurL)
        {
            break;
        }

        /*DEBUG_PRINTF("DEBUG -----visited queue: (%d, %d) k: %d layer: %d seSup: %d done: %d key: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup, pstNode->bUsedDoneFlag,
                     myVstQ.getDy());*/
        ++iCnt;

        DEBUG_ASSERT(pstNode->bInit);
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            ++g_lNeibCnt;
            ++g_lNeibCnt;
            ++g_lUsefulCnt;
            ++g_lUsefulCnt;

            /*DEBUG_PRINTF("DEBUG pop: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);*/

            bLeftFlag = checkDesSeSup(mpG, pstNode->eid, pstLfNode->eid, pstRtNode->eid);
            if (bLeftFlag)
            {
                pstLfNode->iSeSup--;
                /*DEBUG_PRINTF("DEBUG decrease : (%d, %d) k: %d layer: %d seSup: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstLfNode->iTrussness, pstLfNode->iLayer,
                             pstLfNode->iSeSup);*/
                if (pstLfNode->iSeSup + 2 == iCurK)
                {
                    if (pstLfNode->bDoneQFlag)
                    {
                        //DEBUG_ASSERT(!pstLfNode->bInsLFlag);
                        if (pstLfNode->iLayer < pstNode->iLayer + 1)
                        {
                            pstLfNode->bInsLFlag = true;
                            pstLfNode->iLayer = pstNode->iLayer + 1;
                        }
                        pstLfNode->bDoneQFlag = false;
                    }
                    else
                    {
                        //DEBUG_ASSERT(pstLfNode->bCanQFlag);
                        //DEBUG_ASSERT(!pstLfNode->bUsedDoneFlag);
                        pstLfNode->bCanQFlag = false;
                    }
                    //DEBUG_ASSERT(!pstLfNode->bVisQFlag);
                    pstLfNode->bVisQFlag = true;
                    if (!pstLfNode->bUsedVisFlag)
                    {
                        pstLfNode->bUsedVisFlag = true;
                        myVstQ.insetByOrder(myVstQ.mergeKey(pstLfNode->iLayer, pstLfNode->bUsedDoneFlag),
                                               pstLfNode->eid);
                    }
                    /*DEBUG_PRINTF("VISIT push back (%d, %d) layer: %d\n",
                                 pstLfNode->paXY.first, pstLfNode->paXY.second,
                                 pstLfNode->iLayer);*/
                }
            }

            bRightFlag = checkDesSeSup(mpG, pstNode->eid, pstRtNode->eid, pstLfNode->eid);
            if (bRightFlag)
            {
                pstRtNode->iSeSup--;
                /*DEBUG_PRINTF("DEBUG decrease : (%d, %d) k: %d layer: %d seSup: %d\n",
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstRtNode->iTrussness, pstRtNode->iLayer,
                             pstRtNode->iSeSup);*/
                if (pstRtNode->iSeSup + 2 == iCurK)
                {
                    if (pstRtNode->bDoneQFlag)
                    {
                        //DEBUG_ASSERT(!pstRtNode->bInsLFlag);
                        if (pstRtNode->iLayer < pstNode->iLayer + 1)
                        {
                            pstRtNode->bInsLFlag = true;
                            pstRtNode->iLayer = pstNode->iLayer + 1;
                        }
                        pstRtNode->bDoneQFlag = false;
                    }
                    else
                    {
                        //DEBUG_ASSERT(pstRtNode->bCanQFlag);
                        //DEBUG_ASSERT(!pstRtNode->bUsedDoneFlag);
                        pstRtNode->bCanQFlag = false;
                    }
                    //DEBUG_ASSERT(!pstRtNode->bVisQFlag);
                    pstRtNode->bVisQFlag = true;
                    if (!pstRtNode->bUsedVisFlag)
                    {
                        pstRtNode->bUsedVisFlag = true;
                        myVstQ.insetByOrder(myVstQ.mergeKey(pstRtNode->iLayer, pstRtNode->bUsedDoneFlag),
                                               pstRtNode->eid);
                    }
                    /*DEBUG_PRINTF("VISIT push back (%d, %d) layer: %d\n",
                                 pstRtNode->paXY.first, pstRtNode->paXY.second,
                                 pstRtNode->iLayer);*/
                }
            }
        }
        //DEBUG_ASSERT(itRtE == pstNode->vRtE.end());
        pstNode->bUsedDoneFlag = false;
        pstNode->bInsLFlag = false;
        myVstQ.pop();
        pstNode->bVisQFlag = false;
        pstNode->bUsedVisFlag = false;
    }
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    g_lRmQTime += lCurTime - lStartTime;
    return iCnt;
}

/*****************
input:
        myG &mpG
        int iCurT
        vector <int> &vSeed
        vector <int> &vUpGE
        vector <int> &vUpdSeed
description:
        increase k directly
******************/
int insertPart::insKByBFSEasy(myG &mpG, int iCurT, vector <int> &vSeed, vector <int> &vUpGE, vector <int> &vUpdSeed)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;
    vector <int> vVisit;
    vector<int>::iterator itvE;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;

    int iCurEid = 0;
    int iCurLayer = 0;
    int iMinT = 0;
    int iCurK = iCurT;
    int iMinLayer = 0;
    int iOldLayer = 0;
    int iNewLayer = 0;
    int iState = 0;

    bool bLeftFlag = false;
    bool bRightFlag = false;
    int iCnt = 0;

	bool res = false;

    /* <layer, eid> */
    myPriQueue myCanQ;
    vector <int> vDoneQ;
    myPriQueue myVstQ;

    DEBUG_ASSERT(vUpdSeed.empty());
    /*DEBUG_PRINTF("DEBUG start BFS k: %d\n",
                 iCurT);*/
    for (itvE = vSeed.begin(); itvE != vSeed.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);
        if (pstNode->iSeSup + 2 <= pstNode->iTrussness)
        {
            /* updated in ball decomposition */
            continue;
        }

        if (!pstNode->bVstFlag)
        {
            pstNode->bVstFlag = true;
            ++g_lBFSCnt;
            vVisit.push_back(pstNode->eid);
            myCanQ.insetByOrder(pstNode->iLayer, pstNode->eid);
            pstNode->bCanQFlag = true;
            pstNode->bUsedCanFlag = true;

            /*DEBUG_PRINTF("DEBUG seed: (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup);*/
            DEBUG_ASSERT(iCurT == pstNode->iTrussness);
            if (!pstNode->bInit)
            {
                mpG.init(pstNode->eid);
            }
        }
#if 0
        int iDebugCnt = 0;
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);

            iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (iMinT >= pstNode->iTrussness)
            {
                ++iDebugCnt;
            }

        }

        if (iDebugCnt != pstNode->vKLfE.size())
        {
            itLfE = pstNode->vKLfE.begin();
            itRtE = pstNode->vKRtE.begin();
            for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
            {
                pstLfNode = mpG.findNode(*itLfE);
                //DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                //DEBUG_ASSERT(NULL != pstRtNode);

                DEBUG_PRINTF("ERROR restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);

            }
            DEBUG_PRINTF("RESTORE (%d, %d) k: %d layer: %d cnt: %d %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         iDebugCnt, pstNode->vKLfE.size());
            DEBUG_ASSERT(0);
        }
#endif
    }

    //DEBUG_PRINTF("DEBUG start queue\n");
    while (!myCanQ.empty())
    {
        iCurEid = myCanQ.getTop();

        pstNode = mpG.findNode(iCurEid);
        DEBUG_ASSERT(NULL != pstNode);
        if (!pstNode->bCanQFlag)
        {
            /* has been removed, ignore */
            myCanQ.pop();
            pstNode->bUsedCanFlag = false;
            continue;
        }
        DEBUG_ASSERT(pstNode->iLayer == myCanQ.getDy());

        if (pstNode->iLayer > iCurLayer)
        {
            iCnt = 0;
            iCurLayer = pstNode->iLayer;
            //DEBUG_PRINTF("DEBUG =========layer: %d\n", iCurLayer);
            iCnt = rmEInVisQEasy(mpG, iCurK, iCurLayer - 1, myVstQ);
            if (0 < iCnt)
            {
                continue;
            }
        }

        /*DEBUG_PRINTF("DEBUG ------queue: (%d, %d) k: %d layer: %d seSup: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup);*/

        DEBUG_ASSERT(iCurEid == myCanQ.getTop());
        myCanQ.pop();
        pstNode->bCanQFlag = false;
        pstNode->bUsedCanFlag = false;
        DEBUG_ASSERT(pstNode->bInit);
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            /*DEBUG_PRINTF("UPDATE K BFS: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d visit: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstLfNode->bVstFlag, pstRtNode->bVstFlag,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);*/

            ++g_lNeibCnt;

            iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (iMinT != iCurK)
            {
                /* not meet, ignore */
                continue;
            }
            ++g_lUsefulCnt;
            /* visited Queue check */
            iNewLayer = COMMON_TRG_MIN(pstNode->iLayer,
                            pstLfNode->iLayer,
                            (pstLfNode->bCanQFlag ||
                             pstLfNode->bUsedDoneFlag ||
                             pstLfNode->iTrussness > iCurK),
                            pstRtNode->iLayer,
                            (pstRtNode->bCanQFlag ||
                             pstRtNode->bUsedDoneFlag ||
                             pstRtNode->iTrussness > iCurK));
            if (iNewLayer == pstNode->iLayer)
            {
                /* not meet, ignore */
                /* add to  visit queue */
                if ((pstLfNode->iTrussness == iCurK) &&
                    (iNewLayer == pstLfNode->iLayer) &&
                    (!pstLfNode->bCanQFlag) &&
                    (!pstLfNode->bUsedDoneFlag))
                {
                    if (!pstLfNode->bVisQFlag)
                    {
                        pstLfNode->bVisQFlag = true;
                        if (!pstLfNode->bUsedVisFlag)
                        {
                            pstLfNode->bUsedVisFlag = true;
                            myVstQ.insetByOrder(myVstQ.mergeKey(pstLfNode->iLayer, 0),
                                                   pstLfNode->eid);

                            if (!pstLfNode->bVstFlag)
                            {
                                pstLfNode->bVstFlag = true;
                                ++g_lBFSCnt;
                                vVisit.push_back(pstLfNode->eid);
                                if (!pstLfNode->bInit)
                                {
                                    mpG.init(pstLfNode->eid);
                                }
                            }
                        }
                    }
                }
                if ((pstRtNode->iTrussness == iCurK) &&
                    (iNewLayer == pstRtNode->iLayer) &&
                    (!pstRtNode->bCanQFlag) &&
                    (!pstRtNode->bUsedDoneFlag))
                {
                    if (!pstRtNode->bVisQFlag)
                    {
                        pstRtNode->bVisQFlag = true;
                        if (!pstRtNode->bUsedVisFlag)
                        {
                            pstRtNode->bUsedVisFlag = true;
                            myVstQ.insetByOrder(myVstQ.mergeKey(pstRtNode->iLayer, 0),
                                                   pstRtNode->eid);
                            if (!pstRtNode->bVstFlag)
                            {
                                pstRtNode->bVstFlag = true;
                                ++g_lBFSCnt;
                                vVisit.push_back(pstRtNode->eid);
                                if (!pstRtNode->bInit)
                                {
                                    mpG.init(pstRtNode->eid);
                                }
                            }
                        }
                    }
                }

                continue;
            }

            bLeftFlag = checkInsSeSup(mpG, pstNode->eid, pstLfNode->eid, pstRtNode->eid);
            if (bLeftFlag)
            {
                pstLfNode->iSeSup++;

                if (pstLfNode->iSeSup + 1 == iCurK)
                {
                    if (pstLfNode->bVisQFlag)
                    {
                        /* in visited queue */
                        pstLfNode->bVisQFlag = false;
                        pstLfNode->bInsLFlag = false;
                    }
                    pstLfNode->bCanQFlag = true;
                    if (!pstLfNode->bUsedCanFlag)
                    {
                        pstLfNode->bUsedCanFlag = true;
                        myCanQ.insetByOrder(pstLfNode->iLayer, pstLfNode->eid);

                        if (!pstLfNode->bVstFlag)
                        {
                            pstLfNode->bVstFlag = true;
                            ++g_lBFSCnt;
                            vVisit.push_back(pstLfNode->eid);
                            if (!pstLfNode->bInit)
                            {
                                mpG.init(pstLfNode->eid);
                            }
                        }
                    }
                }
                else if (pstLfNode->iSeSup + 1 < iCurK)
                {
                    if (!pstLfNode->bVisQFlag)
                    {
                        pstLfNode->bVisQFlag = true;
                        if (!pstLfNode->bUsedVisFlag)
                        {
                            pstLfNode->bUsedVisFlag = true;
                            myVstQ.insetByOrder(myVstQ.mergeKey(pstLfNode->iLayer, 0),
                                                   pstLfNode->eid);
                            if (!pstLfNode->bVstFlag)
                            {
                                pstLfNode->bVstFlag = true;
                                ++g_lBFSCnt;
                                vVisit.push_back(pstLfNode->eid);
                                if (!pstLfNode->bInit)
                                {
                                    mpG.init(pstLfNode->eid);
                                }
                            }
                        }
                    }
                }
            }

            bRightFlag = checkInsSeSup(mpG, pstNode->eid, pstRtNode->eid, pstLfNode->eid);
            if (bRightFlag)
            {
                pstRtNode->iSeSup++;

                if (pstRtNode->iSeSup + 1 == iCurK)
                {
                    if (pstRtNode->bVisQFlag)
                    {
                        /* in visited queue */
                        pstRtNode->bVisQFlag = false;
                        pstRtNode->bInsLFlag = false;
                    }
                    pstRtNode->bCanQFlag = true;
                    if (!pstRtNode->bUsedCanFlag)
                    {
                        pstRtNode->bUsedCanFlag = true;
                        myCanQ.insetByOrder(pstRtNode->iLayer, pstRtNode->eid);

                        if (!pstRtNode->bVstFlag)
                        {
                            pstRtNode->bVstFlag = true;
                            ++g_lBFSCnt;
                            vVisit.push_back(pstRtNode->eid);
                            if (!pstRtNode->bInit)
                            {
                                mpG.init(pstRtNode->eid);
                            }
                        }

                    }
                }
                else if (pstRtNode->iSeSup + 1 < iCurK)
                {
                    if (!pstRtNode->bVisQFlag)
                    {
                        pstRtNode->bVisQFlag = true;
                        if (!pstRtNode->bUsedVisFlag)
                        {
                            pstRtNode->bUsedVisFlag = true;
                            myVstQ.insetByOrder(myVstQ.mergeKey(pstRtNode->iLayer, 0),
                                                   pstRtNode->eid);
                            if (!pstRtNode->bVstFlag)
                            {
                                pstRtNode->bVstFlag = true;
                                ++g_lBFSCnt;
                                vVisit.push_back(pstRtNode->eid);
                                if (!pstRtNode->bInit)
                                {
                                    mpG.init(pstRtNode->eid);
                                }
                            }
                        }
                    }
                }
            }
        }
        DEBUG_ASSERT(!pstNode->bDoneQFlag);
        pstNode->bDoneQFlag = true;
        if (!pstNode->bUsedDoneFlag)
        {
            pstNode->bUsedDoneFlag = true;
            vDoneQ.push_back(pstNode->eid);
        }
    }
    //DEBUG_PRINTF("DEBUG =========rm all visited queue\n");
    while (!myVstQ.empty())
    {
        iCurEid = myVstQ.getTop();
        pstNode = mpG.findNode(iCurEid);
        DEBUG_ASSERT(NULL != pstNode);

        rmEInVisQEasy(mpG, iCurK, pstNode->iLayer, myVstQ);
    }
    if (!vDoneQ.empty())
    {
        /* upgrade */
        DEBUG_ASSERT(vUpGE.empty());
        DEBUG_ASSERT(vUpdSeed.empty());
        /* increase k */
        ++iCurK;
        //DEBUG_PRINTF("DEBUG ins k: %d\n", iCurK);
        /* upgrade trussness */
        for (auto atIt : vDoneQ)
        {
            pstNode = mpG.findNode(atIt);
            DEBUG_ASSERT(NULL != pstNode);

            if (!pstNode->bDoneQFlag)
            {
                /* has been removed, ignore */
                continue;
            }

            ++g_lUgCnt;
            pstNode->iTrussness = iCurK;
            pstNode->bUgdFlag = true;
            pstNode->iLayer = 1;
            pstNode->bDoneQFlag = false;
            pstNode->bUsedDoneFlag = false;
            /*DEBUG_PRINTF("DEBUG increase k: (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup);*/

            //vector <int> vKLfE;
            //vector <int> vKRtE;

            DEBUG_ASSERT(pstNode->bInit);
            itLfE = pstNode->vLfE.begin();
            itRtE = pstNode->vRtE.begin();
            for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
            {
                pstLfNode = mpG.findNode(*itLfE);
                //DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                //DEBUG_ASSERT(NULL != pstRtNode);
                ++g_lNeibCnt;

                iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                if (iMinT > iCurK)
                {
                    /* not meet, ignore */
                    /*vKLfE.push_back(pstLfNode->eid);
                    vKRtE.push_back(pstRtNode->eid);*/
                    continue;
                }
                else if (iMinT < iCurK)
                {
                    /* not meet, ignore */
                    continue;
                }

                ++g_lUsefulCnt;

                /*vKLfE.push_back(pstLfNode->eid);
                vKRtE.push_back(pstRtNode->eid);
                if (pstLfNode->iTrussness == iCurK)
                {
                    pstLfNode->vKLfE.push_back(pstRtNode->eid);
                    pstLfNode->vKRtE.push_back(pstNode->eid);
                }
                if (pstRtNode->iTrussness == iCurK)
                {
                    pstRtNode->vKLfE.push_back(pstNode->eid);
                    pstRtNode->vKRtE.push_back(pstLfNode->eid);
                }*/

                if ((pstLfNode->iTrussness == iCurK) &&
                    (pstLfNode->iLayer <= pstNode->iLayer) &&
                    (!pstLfNode->bUgdFlag))
                {
                    pstLfNode->iSeSup++;
                    if (pstLfNode->iSeSup + 1 == iCurK)
                    {
                        vUpdSeed.push_back(pstLfNode->eid);
                    }
                }
                if ((pstRtNode->iTrussness == iCurK) &&
                    (pstRtNode->iLayer <= pstNode->iLayer) &&
                    (!pstRtNode->bUgdFlag))
                {
                    pstRtNode->iSeSup++;
                    if (pstRtNode->iSeSup + 1 == iCurK)
                    {
                        vUpdSeed.push_back(pstRtNode->eid);
                    }
                }

            }
            //DEBUG_ASSERT(itRtE == pstNode->vRtE.end());

            /*pstNode->vKLfE.swap(vKLfE);
            pstNode->vKRtE.swap(vKRtE);*/

            vUpGE.push_back(pstNode->eid);
            if (pstNode->iSeSup + 2 > iCurK)
            {
                vUpdSeed.push_back(pstNode->eid);
            }
        }
    }

    /* restore */
    for (itvE = vVisit.begin(); itvE != vVisit.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);

        pstNode->bVstFlag = false;
        DEBUG_ASSERT(!pstNode->bInsLFlag);
        DEBUG_ASSERT(!pstNode->bVisQFlag);
        DEBUG_ASSERT(!pstNode->bCanQFlag);
        DEBUG_ASSERT(!pstNode->bDoneQFlag);
        DEBUG_ASSERT(!pstNode->bUsedCanFlag);
        DEBUG_ASSERT(!pstNode->bUsedDoneFlag);
        DEBUG_ASSERT(!pstNode->bUsedVisFlag);

        if (iCurT == pstNode->iTrussness)
        {
            DEBUG_ASSERT(pstNode->iSeSup + 2 <= pstNode->iTrussness);
        }
#if 0
        int iDebugCnt = 0;
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);

            iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (iMinT >= pstNode->iTrussness)
            {
                ++iDebugCnt;
            }

        }

        if (iDebugCnt != pstNode->vKLfE.size())
        {
            itLfE = pstNode->vKLfE.begin();
            itRtE = pstNode->vKRtE.begin();
            for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
            {
                pstLfNode = mpG.findNode(*itLfE);
                //DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                //DEBUG_ASSERT(NULL != pstRtNode);

                DEBUG_PRINTF("ERROR restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);

            }
            DEBUG_PRINTF("RESTORE (%d, %d) k: %d layer: %d cnt: %d %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         iDebugCnt, pstNode->vKLfE.size());
            DEBUG_ASSERT(0);
        }
#endif
    }

    return 0;
}

/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int insertPart::upgradeForAllK(myG &mpG, vector <int> &vCdtE)
{
    vector <int>::iterator itE;
    /* k, eid */
    map<int, vector <int> > mpKLevel;
    map<int, vector <int> >::iterator itmpNode;
    map<int, vector <int> >::reverse_iterator ritmpNode;
    /* eid, none */
    TPST_MAP_BY_EID* pstNode = NULL;
    vector <int> vUpd;
    vector <int> vSeed;
    int iCurK = 0;
    int iNextK = 0;
    vector <int> vCurUpd;
    vector <int> vUpdSeed;

    COMMON_UNIQUE(vCdtE);
    for (itE = vCdtE.begin(); itE != vCdtE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);
        /*DEBUG_PRINTF("UPGRADE_ALL (%d, %d) k: %d layer: %d seSup: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup);*/
        mpKLevel[pstNode->iTrussness].push_back(*itE);
    }

    /* kmax -> 3 avoid repeat */
    //for (ritmpNode = mpKLevel.rbegin(); ritmpNode != mpKLevel.rend(); ++ritmpNode)
    for (itmpNode = mpKLevel.begin(); itmpNode != mpKLevel.end(); ++itmpNode)
    {
        if (!vUpdSeed.empty())
        {
            /* maintain layer */
            iNextK = iCurK + 1;
            vSeed.swap(vUpdSeed);
            if (iNextK < itmpNode->first)
            {
                /* not yet */
                --itmpNode;
            }
            else
            {
                /* same merge */
                vSeed.insert(vSeed.end(), itmpNode->second.begin(), itmpNode->second.end());
                COMMON_UNIQUE(vSeed);
            }
            iCurK = iNextK;
        }
        else
        {
            /* normal */
            iCurK = itmpNode->first;
            vSeed.swap(itmpNode->second);
        }
        /*DEBUG_PRINTF("UPGRADE K: %d \n",
               iCurK);*/

        DEBUG_ASSERT(vCurUpd.empty());
        insKByBFSEasy(mpG, iCurK, vSeed, vCurUpd, vUpdSeed);
        vUpd.insert(vUpd.end(), vCurUpd.begin(), vCurUpd.end());
        vCurUpd.clear();
        vSeed.clear();
    }

    /* last */
    while (!vUpdSeed.empty())
    {
        /* maintain layer */
        iNextK = iCurK + 1;
        vSeed.swap(vUpdSeed);
        iCurK = iNextK;
        /*DEBUG_PRINTF("UPGRADE K: %d \n",
               iCurK);*/
        DEBUG_ASSERT(vCurUpd.empty());
        insKByBFSEasy(mpG, iCurK, vSeed, vCurUpd, vUpdSeed);
        vUpd.insert(vUpd.end(), vCurUpd.begin(), vCurUpd.end());
        vCurUpd.clear();
        vSeed.clear();
    }

    /* restore */
    for (itE = vUpd.begin(); itE != vUpd.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);
        pstNode->bUgdFlag = false;
    }
    /* last */
    if (!vUpd.empty())
    {
        --itE;
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);
        mpG.m_iMaxK = COMMON_MAX(mpG.m_iMaxK, pstNode->iTrussness);
    }

    return 0;
}

/*****************
input:
        myG &mpG
        int x
        int y
description:
        simply add
******************/
int insertPart::simpleAdd(myG &mpG, int x, int y)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    int iEid = 0;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;

    /* add and init */
    DEBUG_ASSERT(x < y);
    //DEBUG_PRINTF("ADD begin\n");
    iEid = mpG.add(x, y);
    //DEBUG_PRINTF("ADD end\n");
    DEBUG_ASSERT(0 != iEid);
    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    //pstNode->bNewFlag = true;

    pstNode->vLfE.clear();
    pstNode->vRtE.clear();
    mpG.findNeb(x, y, pstNode->vLfE, pstNode->vRtE);
    pstNode->bInit = true;

    itLfE = pstNode->vLfE.begin();
    itRtE = pstNode->vRtE.begin();
    for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
    {
        TPST_MAP_BY_EID* pstLfNode = NULL;
        TPST_MAP_BY_EID* pstRtNode = NULL;

        pstLfNode = mpG.findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = mpG.findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRtNode);

        if (pstLfNode->bInit)
        {
            if (pstLfNode->paXY.first == x)
            {
                /* z > x */
                pstLfNode->vLfE.push_back(iEid);
                pstLfNode->vRtE.push_back(*itRtE);
            }
            else
            {
                /* z < x */
                pstLfNode->vLfE.push_back(*itRtE);
                pstLfNode->vRtE.push_back(iEid);
            }
        }
        else
        {
            mpG.init(pstLfNode->eid);
        }

        if (pstRtNode->bInit)
        {
            if (pstRtNode->paXY.first == y)
            {
                /* z > y */
                pstRtNode->vLfE.push_back(iEid);
                pstRtNode->vRtE.push_back(*itLfE);
            }
            else
            {
                /* z < x */
                pstRtNode->vLfE.push_back(*itLfE);
                pstRtNode->vRtE.push_back(iEid);
            }
        }
        else
        {
            mpG.init(pstRtNode->eid);
        }


        /*DEBUG_PRINTF("DEBUG add vector %d %d %d\n",
               *itRtE, *itLfE, iEid);
        DEBUG_PRINTF("DEBUG add vector size %d %d %d\n",
               pstRtNode->vLfE.size(), pstLfNode->vLfE.size(),
                     pstNode->vLfE.size());*/
    }
    DEBUG_ASSERT(itRtE == pstNode->vRtE.end());
    return iEid;
}
#if 0
/*****************
input:
        myG &mpG
        map<pair<int, int>, int> &mpLocalG
        map<int, TPSTMP_P_INFO> &mpLocalP
        map<pair<int, int>, list<pair<int, int> > > &mpKPLs
description:
        local decomposition
******************/
int insertPart::localDec(myG &mpG, int iNode, map<pair<int, int>, int> &mpLocalG, unordered_map<int, TPSTMP_P_INFO> &mpLocalP, map<pair<int, int>, list<pair<int, int> > > &mpKPLs)
{
    /* k <sesup, pid> */
    map<int, list<pair<int, int> > > mpLowP;
    list<pair<int, int> >::iterator itpaNode;
    map<int, list<pair<int, int> > >::reverse_iterator ritmpP;
    map<pair<int, int>, list<pair<int, int> > >::iterator itmpKPLs;

    unordered_map<int, TPSTMP_P_INFO>::iterator itmpLNode;

    map<int, list<pair<int, int> > >::iterator itmpN;
    list<pair<int, int> >::iterator itpaN;

    list<int>::iterator itlsP;
    map<int, list<pair<int, int> > >::iterator itKE;
    int iCurK = 2;
    int iCurL = 0;
    int iTpL = 0;
    int iNextL = 0;
    int iNextRcL = -1;
    myIntA myIntArray(mpG.m_iMaxD + 1, mpG.m_iMaxPId + 1);

    list<int> lstWaitRm;
    list<int> lstWaitRmSla;
    list<int>::iterator itPid;
    TPST_MAP_BY_EID* pstNode = NULL;
    TPSTMP_P_INFO *pstPInfo = NULL;
    int iCurP = 0;
    int iMinD = 0;
    int iEMinK = 0;
    list<pair<int, int> >::iterator itlsKE;
    bool bOneFlag = false;

    list<pair<int, int> > *plsKE = NULL;

    enum state{ E_RM_NODE = 1, E_RM_E_INIT, E_RM_E, E_RM_E_NODE, E_RM_E_NODE_END, E_INS_K, E_IDLE, E_END} eCurS, eNextS;

    for (itmpLNode = mpLocalP.begin(); itmpLNode != mpLocalP.end(); ++itmpLNode)
    {
        int iMinD = itmpLNode->second.iLocalD;
        myIntArray.insetByOrder(iMinD, itmpLNode->second.iPid);
        /*DEBUG_PRINTF("DEBUG batch insert node: %d, degree: %d\n",
                     itmpLNode->second.iPid,
                     itmpLNode->second.iLocalD);*/
    }
    iCurK = 3;
    iCurL = 0;
    iNextL = 1;

    eCurS = E_RM_NODE;
    eNextS = E_RM_NODE;
    while (!myIntArray.empty() || !lstWaitRmSla.empty())
    {
        iCurL = iNextL;
        ++iNextL;
        eCurS = eNextS;

        /*DEBUG_PRINTF("DECOMP cur k: %d state: %d layer: %d\n",
                     iCurK, eCurS, iCurL);*/
        switch (eCurS)
        {
        case E_RM_NODE:
            iCurP = myIntArray.getTop();
            iMinD = myIntArray.getDyKey(iCurP);
            if (iCurK > iMinD + 2)
            {
                /* rm node, */
                //DEBUG_PRINTF("DECOMP rm nodes\n");
                DEBUG_ASSERT(lstWaitRm.empty());
                myIntArray.getTopList(lstWaitRm);
                for (itPid = lstWaitRm.begin(); itPid != lstWaitRm.end(); ++itPid)
                {
                    myIntArray.myDel(*itPid);
                    mpLocalP[*itPid].iSeSup = iMinD;
                    mpLocalP[*itPid].iLayer = 1;
                    //DEBUG_PRINTF("DECOMP batch rm %d seSup: %d\n", *itPid, iMinD);
                }
                lstWaitRmSla.merge(lstWaitRm);
                //DEBUG_PRINTF("DECOMP rm nodes done\n");
            }

            //eCurS = E_RM_E;
            eNextS = E_RM_E;
            iNextL = iCurL;
            break;

        case E_RM_E:
            eNextS = E_RM_E_NODE;

            /* rm edge */
            if (mpKPLs.empty())
            {
                if (lstWaitRmSla.empty())
                {
                    eNextS = E_INS_K;
                }
                break;
            }
            itmpKPLs = mpKPLs.begin();
            iEMinK = itmpKPLs->first.first;
            if (iEMinK >= iCurK)
            {
                /* no edge rm */
                if (lstWaitRmSla.empty())
                {
                    eNextS = E_INS_K;
                }
                break;
            }

            /* rm edge */
            iTpL = itmpKPLs->first.second;
            if (iTpL > iCurL)
            {
                /* not yet */
                iNextRcL = iTpL;
                /*if (lstWaitRmSla.empty())
                {
                    ++iCurL;
                }*/
                break;
            }
            plsKE = &(itmpKPLs->second);
            DEBUG_ASSERT(NULL != plsKE);

            /*DEBUG_PRINTF("DECOMP rm edges edge k: %d layer: %d\n",
                         iEMinK, iCurL);*/

            for (itlsKE = plsKE->begin(); itlsKE !=  plsKE->end(); ++itlsKE)
            {
                int iLcD = 0;
                int iLcCurP = 0;
                pair<int, int> paXY;
                paXY = *itlsKE;
                if (mpLocalG.find(paXY) == mpLocalG.end())
                {
                    /* has removed, ignore */
                    continue;
                }
                if (mpLocalP[paXY.first].bRmFlag)
                {
                    continue;
                }
                if (mpLocalP[paXY.second].bRmFlag)
                {
                    continue;
                }
                //DEBUG_PRINTF("DECOMP start update\n");
                iLcCurP = paXY.first;
                DEBUG_ASSERT(mpLocalP.find(iLcCurP) != mpLocalP.end());
                if (!(myIntArray.inIt(iLcCurP)))
                {
                    /* in queue */
                    mpLocalP[iLcCurP].iSeSup--;
                }
                else
                {
                    iLcD = mpLocalP[iLcCurP].iLocalD;
                    --iLcD;
                    mpLocalP[iLcCurP].iLocalD = iLcD;

                    if (iLcD < iCurK - 2)
                    {
                        /* cannot remained */
                        lstWaitRmSla.push_back(iLcCurP);
                        mpLocalP[iLcCurP].iLayer = iCurL + 1;
                        myIntArray.myDel(iLcCurP);
                        mpLocalP[iLcCurP].iSeSup = iLcD;
                        /*DEBUG_PRINTF("DECOMP update first rm %d layer: %d seSup: %d\n",
                                     iLcCurP, iCurL + 1, iLcD);*/
                    }
                    else
                    {
                        /* reinsert */
                        myIntArray.insetByOrder(iLcD, iLcCurP);
                        /*DEBUG_PRINTF("DECOMP reinsert p: %d d: %d\n",
                               iLcCurP, iLcD);*/
                    }
                    //DEBUG_PRINTF("DECOMP update done\n", iLcCurP);
                }

                iLcCurP = paXY.second;
                /*DEBUG_PRINTF("DECOMP now update p: %d\n",
                       iLcCurP);*/
                DEBUG_ASSERT(mpLocalP.find(iLcCurP) != mpLocalP.end());

                if (!(myIntArray.inIt(iLcCurP)))
                {
                    /* in queue */
                    mpLocalP[iLcCurP].iSeSup--;
                }
                else
                {
                    iLcD = mpLocalP[iLcCurP].iLocalD;
                    --iLcD;
                    mpLocalP[iLcCurP].iLocalD = iLcD;
                    /*DEBUG_PRINTF("DECOMP update p: %d d: %d\n",
                           iLcCurP, iLcD);*/

                    if (iLcD < iCurK - 2)
                    {
                        /* cannot remain */
                        lstWaitRmSla.push_back(iLcCurP);
                        mpLocalP[iLcCurP].iLayer = iCurL + 1;
                        myIntArray.myDel(iLcCurP);
                        mpLocalP[iLcCurP].iSeSup = iLcD;
                        /*DEBUG_PRINTF("DECOMP update second rm %d layer: %d seSup: %d\n",
                                     iLcCurP, iCurL + 1, iLcD);*/
                    }
                    else
                    {
                        /* reinsert */
                        myIntArray.insetByOrder(iLcD, iLcCurP);
                        /*DEBUG_PRINTF("DECOMP reinsert p: %d d: %d\n",
                               iLcCurP, iLcD);*/
                    }
                }
                mpLocalG.erase(paXY);
                mpLocalG.erase(pair<int, int>(paXY.second, paXY.first));
            }
            //DEBUG_PRINTF("DEBUG end edges removing\n");
            mpKPLs.erase(mpKPLs.begin());
            //DEBUG_PRINTF("DEBUG end rm edges\n");

            iNextRcL = -1;
            if (mpKPLs.empty())
            {
                break;
            }
            itmpKPLs = mpKPLs.begin();
            if (iEMinK < itmpKPLs->first.first)
            {
                /* k empty */
                break;
            }
            else
            {
                iNextRcL = itmpKPLs->first.second;
            }

            break;
        case E_RM_E_NODE:
            eNextS = E_RM_E;
            if (iCurL >= iNextRcL)
            {
                iNextL = iCurL;
            }
            else
            {
                iNextL = iCurL + 1;
            }
            break;
        case E_INS_K:
            /*if (!lstWaitRmSla.empty())
            {
                break;
            }*/
            /* fine, next k */
            //DEBUG_PRINTF("DECOMP save low\n");
            myIntArray.getAll(mpLowP[iCurK]);
            /* could be (k+1)-truss */
            /*if (iMinD > iCurK - 2)
            {
                DEBUG_PRINTF("DECOMP save up\n");
                myIntArray.getAll(mpUpP[iCurK + 1]);
            }*/
            ++iCurK;
            iCurL = 0;
            iNextL = 1;
            eNextS = E_RM_NODE;
            break;
        }

        if (E_RM_E == eCurS)
        {
            /* no need to rm */
            continue;
        }

        bOneFlag = false;

        //DEBUG_PRINTF("DECOMP start %d\n", !lstWaitRm.empty() || !lstWaitRmSla.empty());
        while(!lstWaitRm.empty() || !lstWaitRmSla.empty())
        {
            int x = 0;
            int y = 0;
            list<int> lstTpNeibP;
            int iRmPid = 0;

            if (lstWaitRm.empty())
            {
                /*DEBUG_PRINTF("DECOMP cur L: %d next L: %d next record L: %d state: %d\n",
                             iCurL, iNextL, iNextRcL, eCurS);*/

                if (bOneFlag)
                {
                    if (E_RM_NODE == eCurS)
                    {
                        /* just once */
                        break;
                    }
                    else if (E_RM_E_NODE == eCurS)
                    {
                        /* directly rm nodes, delay rm edges */
                        if (iCurL >= iNextRcL)
                        {
                            if ((0 < iNextRcL) && (!mpKPLs.empty()))
                            {
                                /* recharge */
                                iNextL = iCurL;
                                break;
                            }
                            else
                            {
                                /* empty, normal increase */
                                if (iCurL == iNextL)
                                {
                                    ++iCurL;
                                    iNextL = iCurL + 1;
                                }
                                else
                                {
                                    iCurL = iNextL;
                                    ++iNextL;
                                }
                            }
                        }
                        else if (iNextL >= iNextRcL)
                        {
                            /* current increase, but next not increase */
                            iCurL = iNextL;
                        }
                        else
                        {
                            /* no other layer need to be included */
                            if (iCurL == iNextL)
                            {
                                ++iCurL;
                                iNextL = iCurL + 1;
                            }
                            else
                            {
                                iCurL = iNextL;
                                ++iNextL;
                            }
                        }

                    }

                }

                lstWaitRm.merge(lstWaitRmSla);
                for (itlsP = lstWaitRm.begin(); itlsP != lstWaitRm.end(); )
                {
                    iRmPid = *itlsP;
                    if (mpLocalP[iRmPid].iLayer > iCurL)
                    {
                        /* delay rm */
                        lstWaitRmSla.push_back(iRmPid);
                        lstWaitRm.erase(itlsP++);
                        continue;
                    }
                    mpLocalP[iRmPid].bLock = true;
                    //DEBUG_PRINTF("DECOMP batch rm node: %d\n", iRmPid);
                    ++itlsP;
                }

                /* just once */
                /*if (E_RM_E_NODE == eCurS)
                {
                    eCurS = E_RM_E;
                }*/
                if (lstWaitRm.empty())
                {
                    continue;
                }
            }

            bOneFlag = true;
            DEBUG_ASSERT(!lstWaitRm.empty());
            iRmPid = lstWaitRm.front();
            lstWaitRm.pop_front();

            mpLocalP[iRmPid].iLayer = iCurL;
            mpLocalP[iRmPid].bRmFlag = true;
            mpLocalP[iRmPid].bLock = false;

            //DEBUG_PRINTF("DECOMP get node: %d layer: %d\n", iRmPid, iCurL);

            lstTpNeibP.clear();
            mpG.findPNebP(iRmPid, lstTpNeibP, mpLocalG);
            for (itlsP = lstTpNeibP.begin(); itlsP != lstTpNeibP.end(); ++itlsP)
            {
                int iLcD = 0;
                //DEBUG_PRINTF("DECOMP find neib node: %d\n", *itlsP);
                if (mpLocalP[*itlsP].bRmFlag || mpLocalP[*itlsP].bLock)
                {
                    continue;
                }
                if (!(myIntArray.inIt(*itlsP)))
                {
                    /* in rm queue */
                    mpLocalP[*itlsP].iSeSup--;
                    /*DEBUG_PRINTF("DECOMP p: %d seSup: %d\n",
                                 *itlsP, mpLocalP[*itlsP].iSeSup);*/
                    continue;
                }
                iLcD = mpLocalP[*itlsP].iLocalD;
                --iLcD;
                mpLocalP[*itlsP].iLocalD = iLcD;

                if (iLcD < iCurK - 2)
                {
                    /* cannot remain */
                    lstWaitRmSla.push_back(*itlsP);
                    myIntArray.myDel(*itlsP);
                    mpLocalP[*itlsP].iSeSup = iLcD;
                    mpLocalP[*itlsP].iLayer = iCurL + 1;
                    /*DEBUG_PRINTF("DECOMP update third rm %d seSup: %d layer: %d\n",
                                 *itlsP, iLcD, iCurL + 1);*/
                }
                else
                {
                    /* reinsert */
                    myIntArray.insetByOrder(iLcD, *itlsP);
                    /*DEBUG_PRINTF("DECOMP reinsert p: %d d: %d\n",
                           *itlsP, iLcD);*/
                }
            } // end for neighbor
        } // end for wait queue

        //DEBUG_PRINTF("DECOMP waited done\n");
    } // end for queue

    /*DEBUG_PRINTF("DECOMP test node %d layer: %d\n",
                 10, mpLocalP[10].iLayer);*/
    for (ritmpP = mpLowP.rbegin(); ritmpP != mpLowP.rend(); ++ritmpP)
    {
        int iTpK = ritmpP->first;
        for (itpaNode = ritmpP->second.begin(); itpaNode != ritmpP->second.end(); ++itpaNode)
        {
            int iTpD = itpaNode->first;
            int iTpPId = itpaNode->second;
            pstNode = mpG.findNode(iNode, iTpPId);
            DEBUG_ASSERT(NULL != pstNode);
            pstPInfo = &(mpLocalP[iTpPId]);
            if (iTpK >= pstPInfo->iLowB)
            {
                pstPInfo->iLowB = iTpK;
                if (pstNode->iTrussness > pstPInfo->iLowB)
                {
                    /*DEBUG_PRINTF("DECOMP change node %d k: %d lowB: %d layer: %d\n",
                                 iTpPId,
                                 pstNode->iTrussness, pstPInfo->iLowB,
                                 pstPInfo->iLayer);*/
                    pstPInfo->iLowB = pstNode->iTrussness;
                    pstPInfo->iJuSup = pstNode->iJuSup;
                    pstPInfo->iLayer = pstNode->iLayer;
                    pstPInfo->iSeSup = pstNode->iSeSup;
                }
                else
                {
                    pstPInfo->iJuSup = iTpD;
                }
            }
            /*DEBUG_PRINTF("DEBUG k: %d lowB: %d t: %d p: %d juSup: %d layer: %d seSup: %d\n",
                   iTpK, pstPInfo->iLowB, pstNode->iTrussness,
                   iTpPId, iTpD, pstPInfo->iLayer, pstPInfo->iSeSup);*/
        }
    }
    return 0;
}
#endif
/*****************
input:
        myG &mpG
        map<pair<int, int>, int> &mpLocalG
        map<int, TPSTMP_P_INFO> &mpLocalP
        map<pair<int, int>, list<pair<int, int> > > &mpKPLs
description:
        local decomposition
******************/
bool insertPart::cmpOrder(const TPST_ORDER& e1, const TPST_ORDER& e2)
{
    if (e1.k > e2.k)
    {
        return true;
    }
    else if (e1.k < e2.k)
    {
        return false;
    }
    else
    {
        if (e1.L > e2.L)
        {
            return true;
        }
        else if (e1.L < e2.L)
        {
            return false;
        }
    }
    return false;
}
/*****************
input:
        myG &mpG
        map<pair<int, int>, int> &mpLocalG
        map<int, TPSTMP_P_INFO> &mpLocalP
        map<pair<int, int>, list<pair<int, int> > > &mpKPLs
description:
        local decomposition
******************/
int insertPart::localDec(myG &mpG, int iNode, vector <TPST_ADJ> &vNeibE, vector <int> &vSeed)
{
    /* all are temp id */
    vector<vector<TPST_ADJ> > adj;
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID *pstLf = NULL;
    TPST_MAP_BY_EID *pstRt = NULL;
    TPST_MAP_BY_EID *pstDes = NULL;
    TPST_MAP_BY_EID *pstTrd = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;
    vector <int> vEdges;
    vector <TPST_ORDER> vTrdOrder;
    vector <pair<int, int> > vPair;
    int iSupMax = 0;

    int i_N = vNeibE.size();
    int i_M = 0;

    //DEBUG_PRINTF("LOCAL start degree: %d\n", i_N);

    //DEBUG_PRINTF("LOCAL reserve done\n");
    /* rename id */
    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        pstNode = mpG.findNode(vNeibE[iTpId].iEid);
        //DEBUG_ASSERT(NULL != pstNode);
        pstNode->iTpId = iTpId;
        if (!pstNode->bInit)
        {
            mpG.init(pstNode->eid);
        }
    }

    //DEBUG_PRINTF("LOCAL adj start\n");
    // initialize adjacency arrays
    adj.resize(i_N);
    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        int iEid = vNeibE[iTpId].iEid;

        pstNode = mpG.findNode(iEid);
        //DEBUG_ASSERT(NULL != pstNode);
        int iDegree = pstNode->vLfE.size();
        adj[pstNode->iTpId].reserve(iDegree);
        iSupMax = COMMON_MAX(iSupMax, iDegree);
        i_M += iDegree;
        /*DEBUG_PRINTF("LOCAL neighbor init (%d, %d) k: %d layer: %d seSup: %d degree: %d pos: %d neib: %d init: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup, iDegree, iTpId, vNeibE[iTpId].iPid, pstNode->bInit);*/
    }
    i_M /= 2;
    vEdges.reserve(i_M);
    vPair.reserve(i_M);

    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        int iEid = vNeibE[iTpId].iEid;
        //DEBUG_PRINTF("LOCAL current node: %d\n", iTpId);

        pstNode = mpG.findNode(iEid);
        //DEBUG_ASSERT(NULL != pstNode);

        /* fill neighbor truss vector */
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLf = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLf);
            pstRt = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRt);
            if ((pstLf->paXY.first == iNode) || (pstLf->paXY.second == iNode))
            {
                pstDes = pstLf;
                pstTrd = pstRt;
            }
            else
            {
                //DEBUG_ASSERT((pstRt->paXY.first == iNode) || (pstRt->paXY.second == iNode));
                pstDes = pstRt;
                pstTrd = pstLf;
            }
            //DEBUG_PRINTF("LOCAL neighbor: %d\n", pstDes->iTpId);
            if (pstNode->iTpId > pstDes->iTpId)
            {
                /* ignore half */
                continue;
            }
            if (!pstTrd->bInit)
            {
                mpG.init(pstTrd->eid);
            }
            vEdges.push_back(pstTrd->eid);
            vPair.push_back(pair<int, int>(pstNode->iTpId, pstDes->iTpId));
            int iTpEid = vEdges.size() - 1;
            pstTrd->iTpId = iTpEid;
            adj[pstNode->iTpId].push_back({pstDes->iTpId, iTpEid});
            adj[pstDes->iTpId].push_back({pstNode->iTpId, iTpEid});
            //DEBUG_PRINTF("LOCAL adj (%d, %d)\n", pstNode->iTpId, pstDes->iTpId);
            if (3 > pstTrd->iTrussness)
            {
                /* at least 3-truss */
                pstTrd->iTrussness = 3;

                ++g_lUgCnt;
            }
            vTrdOrder.push_back({pstTrd->iTrussness, pstTrd->iLayer, iTpEid});
            //DEBUG_PRINTF("LOCAL adj k: %d L:%d\n", pstTrd->iTrussness, pstTrd->iLayer);
        }

    }

    //DEBUG_PRINTF("LOCAL decomposition start\n");
    sort(vTrdOrder.begin(), vTrdOrder.end(), cmpOrder);
    // 2. decomposition
    // 2.1. sort the nodes according to their degrees
    //i_M = vEdges.size();
    //DEBUG_ASSERT(i_M == vEdges.size());
    vector<vector<int> > bin(iSupMax + 1);
    /*for (int i = 0; i <= iSupMax; ++i)
    {
        bin[i].reserve(i_N);
    }*/
    vector<int> v_D(i_N);
    for (int pid = 0; pid < i_N; ++pid)
    {
        v_D[pid] = adj[pid].size();
        bin[v_D[pid]].push_back(pid);
    }

    //DEBUG_PRINTF("LOCAL peeling start\n");
    // 2.2. peeling
    vector<int> v_t(i_N, 0);
    vector<int> v_L(i_N, 0);
    vector<int> v_SeSup(i_N, 0);
    vector<bool> vNRmd(i_N, false);
    vector<bool> vNChgD(i_N, false);
    vector<bool> vERmd(i_M, false);
    vector<int> vChgDQ;
    vChgDQ.reserve(i_N);
    /*for (auto atB : removed)
    {
        DEBUG_PRINTF("LOCAL DEBUG b: %d\n", atB);
    }
    removed[2] = true;
    for (int i = 0; i < removed.size(); ++i)
    {
        bool bTp = removed[i];
        DEBUG_PRINTF("LOCAL DEBUG b: %d\n", bTp);
    }*/
    int iCurK = 2;
    int iCurL = 1;
    int iRmCnt = 0;
    vector<int> vWait;
    vWait.reserve(i_N);
    // 2.2.1. process the edges layer by layer
    while (iRmCnt < i_N)
    {
        vWait.clear();
        //DEBUG_PRINTF("LOCAL current k: %d L: %d\n", iCurK, iCurL);
        while ((!vTrdOrder.empty()) && cmpOrder({iCurK, iCurL, 0}, vTrdOrder.back()))
        {
            /* rm edge */
            int iTpEid = vTrdOrder.back().eid;
            /*DEBUG_PRINTF("LOCAL rm edges (%d, %d) start len: %d\n",
                         vPair[iTpEid].first, vPair[iTpEid].second, vTrdOrder.size());*/
            vTrdOrder.pop_back();
            if (!vERmd[iTpEid])
            {
                vERmd[iTpEid] = true;
                //DEBUG_ASSERT(0 < v_D[vPair[iTpEid].first]);
                --v_D[vPair[iTpEid].first];
                if (!vNChgD[vPair[iTpEid].first])
                {
                    vNChgD[vPair[iTpEid].first] = true;
                    vChgDQ.push_back(vPair[iTpEid].first);
                }
                //bin[v_D[vPair[iTpEid].first]].push_back(vPair[iTpEid].first);
                //DEBUG_ASSERT(0 < v_D[vPair[iTpEid].second]);
                --v_D[vPair[iTpEid].second];
                if (!vNChgD[vPair[iTpEid].second])
                {
                    vNChgD[vPair[iTpEid].second] = true;
                    vChgDQ.push_back(vPair[iTpEid].second);
                }
                //bin[v_D[vPair[iTpEid].second]].push_back(vPair[iTpEid].second);
            }
        }
        for (int iTpPid : vChgDQ)
        {
            //DEBUG_ASSERT(-1 < v_D[iTpPid]);
            bin[v_D[iTpPid]].push_back(iTpPid);
            vNChgD[iTpPid] = false;
        }
        vChgDQ.clear();
        //DEBUG_PRINTF("LOCAL rm nodes start\n");
        for (int i = 0; i <= iCurK - 2; ++i)
        {
            for (auto pid : bin[i])
            {
                /* avoid repetition */
                if ((!vNRmd[pid]) && (iCurK != v_t[pid]))
                {
                    vWait.push_back(pid);
                    v_t[pid] = iCurK;
                    v_L[pid] = iCurL;
                    v_SeSup[pid] = i;
                }
            }
            bin[i].clear();
        }
        ++iCurL;

        if (vWait.empty())
        {
            if (!vTrdOrder.empty())
            {
                if ((cmpOrder({iCurK + 1, 1, 0}, vTrdOrder.back())))
                {
                    continue;
                }
            }
            ++iCurK;
            iCurL = 1;
            continue;
        }

        //DEBUG_PRINTF("LOCAL rm neighbor start\n");
        for (auto pid : vWait)
        {
            /*DEBUG_PRINTF("LOCAL rm node: %d\n", pid);
            DEBUG_PRINTF("LOCAL bool: %d\n", (bool)vNRmd[pid]);*/
            vNRmd[pid] = true;
            ++iRmCnt;

            for (const auto neiP : adj[pid])
            {
                /*DEBUG_PRINTF("LOCAL rm node neighbor: %d\n", neiP.iPid);
                DEBUG_PRINTF("LOCAL bool: %d\n", (bool)vNRmd[neiP.iPid]);*/
                if (vNRmd[neiP.iPid]) continue;
                if (vERmd[neiP.iEid]) continue;

                //DEBUG_PRINTF("LOCAL degree: %d\n", v_D[neiP.iPid]);

                //DEBUG_ASSERT(0 < v_D[neiP.iPid]);
                --v_D[neiP.iPid];
                if (!vNChgD[neiP.iPid])
                {
                    vNChgD[neiP.iPid] = true;
                    vChgDQ.push_back(neiP.iPid);
                }
                //bin[v_D[neiP.iPid]].push_back(neiP.iPid);
                vERmd[neiP.iEid] = true;
            }
        }
        for (int iTpPid : vChgDQ)
        {
            DEBUG_ASSERT(-1 < v_D[iTpPid]);
            bin[v_D[iTpPid]].push_back(iTpPid);
            vNChgD[iTpPid] = false;
        }
        vChgDQ.clear();
    }

    //DEBUG_PRINTF("LOCAL save start\n");
    /* save result */
    for (int iTpId = 0; iTpId < i_M; ++iTpId)
    {
        int iEid = vEdges[iTpId];
        int iLfTpPid = vPair[iTpId].first;
        int iRtTpPid = vPair[iTpId].second;
        int iLfEid = vNeibE[iLfTpPid].iEid;
        int iRtEid = vNeibE[iRtTpPid].iEid;

        pstNode = mpG.findNode(iEid);
        //DEBUG_ASSERT(NULL != pstNode);
        pstLf = mpG.findNode(iLfEid);
        //DEBUG_ASSERT(NULL != pstLf);
        pstRt = mpG.findNode(iRtEid);
        //DEBUG_ASSERT(NULL != pstRt);

        /*DEBUG_PRINTF("LOCAL (%d, %d) (%d, %d) k: %d %d %d %d layer: %d %d %d %d seSup: %d %d self: (%d, %d) layer: %d seSup: %d\n",
                     pstLf->paXY.first, pstLf->paXY.second,
                     pstRt->paXY.first, pstRt->paXY.second,
                     pstLf->iTrussness, pstRt->iTrussness,
                     v_t[iLfTpPid], v_t[iRtTpPid],
                     pstLf->iLayer, pstRt->iLayer,
                     v_L[iLfTpPid], v_L[iRtTpPid],
                     pstLf->iSeSup, pstRt->iSeSup,
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iLayer,
                     pstNode->iSeSup);*/

        if (cmpOrder({pstNode->iTrussness, pstNode->iLayer, 0}, {pstLf->iTrussness, pstLf->iLayer, 0}) ||
            cmpOrder({pstNode->iTrussness, pstNode->iLayer, 0}, {pstRt->iTrussness, pstRt->iLayer, 0}))
        {
            if (!cmpOrder({pstNode->iTrussness, pstNode->iLayer, 0}, {v_t[iLfTpPid], v_L[iLfTpPid], 0}) &&
                !cmpOrder({pstNode->iTrussness, pstNode->iLayer, 0}, {v_t[iRtTpPid], v_L[iRtTpPid], 0}))
            {
                /* add SeSup */
                pstNode->iSeSup++;
                if ((pstNode->iSeSup + 2 > pstNode->iTrussness) && (!pstNode->bCanQFlag))
                {
                    pstNode->bCanQFlag = true;
                    vSeed.push_back(pstNode->eid);
                }
            }
        }

        /*DEBUG_PRINTF("LOCAL (%d, %d) k: %d layer: %d seSup: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup);*/
    }
    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        int iEid = vNeibE[iTpId].iEid;

        pstNode = mpG.findNode(iEid);
        //DEBUG_ASSERT(NULL != pstNode);
        pstNode->bCanQFlag = false;
        if (pstNode->iTrussness > v_t[iTpId])
        {
            printf("ERROR self: %d local: %d\n", pstNode->iTrussness, v_t[iTpId]);
            DEBUG_PRINTF("LOCAL neighbor error (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup);
            DEBUG_ASSERT(0);
        }
        if (pstNode->iTrussness < v_t[iTpId])
        {
            pstNode->iTrussness = v_t[iTpId];
            ++g_lUgCnt;
        }
        pstNode->iLayer = v_L[iTpId];
        pstNode->iSeSup = v_SeSup[iTpId];

        /*DEBUG_PRINTF("LOCAL neighbor (%d, %d) k: %d layer: %d seSup: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup);*/
    }

    //DEBUG_PRINTF("LOCAL save end\n");
    return 0;
}
#if 0
/*****************
input:
        myG &mpG
        int iNode
description:
        find up/low bound for p neighbor
******************/
int insertPart::nodeDec(myG &mpG, int iNode, vector <int> &vSeed)
{
    vector <pair<int, int> > vNeibP;
    vector <pair<int, int> >::iterator itNeibP;
    vector<int> vThirdE;
    vector<int>::iterator itE;
    /* <k, layer>, <pid, pid> */
    map<pair<int, int>, list<pair<int, int> > > mpKPLs;
    map<pair<int, int>, int> mpLocalG;

    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;

    /* pid, nodeInfo */
    unordered_map<int, TPSTMP_P_INFO> mpLocalP;
    TPSTMP_P_INFO *pstPInfo = NULL;

    TPST_MAP_BY_EID* pstNode = NULL;

	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;

    //DEBUG_PRINTF("BALL--------------- node: %d\n", iNode);
    /* get valuable edges, filter no possible edges (sup+2 < k-1 or trussness > k-1) */
    mpG.findPNebPTrdE(iNode, vNeibP, vThirdE);
    for (itNeibP = vNeibP.begin(); itNeibP != vNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[itNeibP->first]);
        pstPInfo->iPid = itNeibP->first;
        pstPInfo->iEid = itNeibP->second;
        pstPInfo->iLocalD = 0;
        pstPInfo->iMaxK = 2;
        pstPInfo->iLayer = 1;

        pstPInfo->iLowB = 2;

        pstPInfo->iSup = 0;
        pstPInfo->iJuSup = 0;
        pstPInfo->iSeSup = 0;
        pstPInfo->bRmFlag = false;
        pstPInfo->bLock = false;
    }
    for (itE = vThirdE.begin(); itE != vThirdE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);
        /* at least 3-truss */
        if (pstNode->iTrussness < 3)
        {
            /* cannot increase more */
            pstNode->iTrussness = 3;
            pstNode->iJuSup = 0;
            pstNode->iSeSup = 0;
            pstNode->iLayer = 1;
        }
        pstPInfo = &(mpLocalP[pstNode->paXY.first]);
        pstPInfo->iLocalD++;
        pstPInfo->iMaxK = COMMON_MAX(pstPInfo->iMaxK, pstNode->iTrussness);

        pstPInfo = &(mpLocalP[pstNode->paXY.second]);
        pstPInfo->iLocalD++;
        pstPInfo->iMaxK = COMMON_MAX(pstPInfo->iMaxK, pstNode->iTrussness);

        mpLocalG[pair<int, int>(pstNode->paXY.first, pstNode->paXY.second)];
        mpLocalG[pair<int, int>(pstNode->paXY.second, pstNode->paXY.first)];
        mpKPLs[pair<int, int>(pstNode->iTrussness, pstNode->iLayer)].push_back(pstNode->paXY);
    }
#if 0
    /* debug */
    for (itNeibP = vNeibP.begin(); itNeibP != vNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[itNeibP->first]);
        pstNode = mpG.findNode(pstPInfo->iEid);
        DEBUG_ASSERT(NULL != pstNode);

        DEBUG_PRINTF("DEBUG before (%d, %d) k: %d layer: %d new: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->bNewFlag);

        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG all: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);
        }

        itLfE = pstNode->vKLfE.begin();
        itRtE = pstNode->vKRtE.begin();
        for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);
        }
    }
#endif

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    localDec(mpG, iNode, mpLocalG, mpLocalP, mpKPLs);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    g_lLocalDecTime += lCurTime - lStartTime;

    /* add juSup */
    for (itE = vThirdE.begin(); itE != vThirdE.end(); ++itE)
    {
        int iTpMinK = 0;
        int iTpMinT = 0;
        bool bNewTriFlag = false;
        TPSTMP_P_INFO *pstPLfInfo = NULL;
        TPSTMP_P_INFO *pstPRtInfo = NULL;

        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        //DEBUG_PRINTF("BALL third: (%d, %d)\n", pstNode->paXY.first, pstNode->paXY.second);

        pstPLfInfo = &(mpLocalP[pstNode->paXY.first]);
        pstPRtInfo = &(mpLocalP[pstNode->paXY.second]);

        iTpMinK = COMMON_MIN(pstPLfInfo->iLowB,
                            pstPRtInfo->iLowB);
        if (iTpMinK < pstNode->iTrussness)
        {
            /* cannot increase sup, ignore */
            continue;
        }

        pstLfNode = mpG.findNode(pstPLfInfo->iEid);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = mpG.findNode(pstPRtInfo->iEid);
        DEBUG_ASSERT(NULL != pstRtNode);

        iTpMinT = COMMON_MIN(pstLfNode->iTrussness,
                            pstRtNode->iTrussness);
#if 0
        if ((iTpMinT < pstNode->iTrussness) && (iTpMinK >= pstNode->iTrussness))
        {
            pstNode->vKLfE.push_back(pstLfNode->eid);
            pstNode->vKRtE.push_back(pstRtNode->eid);
        }
#endif
        if (pstLfNode->bNewFlag || pstRtNode->bNewFlag ||
            (pstNode->iTrussness > iTpMinT))
        {
            bNewTriFlag = true;
        }

        /* form new triangle */
        if (bNewTriFlag)
        {
            /* can increase sup */
            pstNode->iJuSup++;
        }

        /* add seSup */
        if ((iTpMinT <= pstNode->iTrussness) &&
            (iTpMinK >= pstNode->iTrussness))
        {
            bool bOldFlag = false;
            /* check old */
            if (iTpMinT == pstNode->iTrussness)
            {
                int iOldL = pstNode->iLayer;
                if (pstNode->iTrussness == pstLfNode->iTrussness)
                {
                    iOldL = COMMON_MIN(iOldL, pstLfNode->iLayer);
                }
                if (pstNode->iTrussness == pstRtNode->iTrussness)
                {
                    iOldL = COMMON_MIN(iOldL, pstRtNode->iLayer);
                }
                if (pstNode->iLayer <= iOldL)
                {
                    /* already formed, ignore */
                    bOldFlag = true;
                }
            }

            /* check new */
            if (!bOldFlag)
            {
                if (iTpMinK > pstNode->iTrussness)
                {
                    pstNode->iSeSup++;
                }
                else
                {
                    int iNewL = pstNode->iLayer;
                    if (pstNode->iTrussness == pstPLfInfo->iLowB)
                    {
                        iNewL = COMMON_MIN(iNewL, pstPLfInfo->iLayer);
                    }
                    if (pstNode->iTrussness == pstPRtInfo->iLowB)
                    {
                        iNewL = COMMON_MIN(iNewL, pstPRtInfo->iLayer);
                    }
                    if (pstNode->iLayer <= iNewL)
                    {
                        /* form new */
                        pstNode->iSeSup++;
                    }
                }
                if (pstNode->iSeSup + 2 > pstNode->iTrussness)
                {
                    vSeed.push_back(pstNode->eid);
                }
            }
        }
    }
#if 0
    /* debug */
    for (itNeibP = vNeibP.begin(); itNeibP != vNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[itNeibP->first]);
        pstNode = mpG.findNode(pstPInfo->iEid);
        DEBUG_ASSERT(NULL != pstNode);

        DEBUG_PRINTF("DEBUG after (%d, %d) k: %d layer: %d new: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->bNewFlag);

        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG all: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);
        }

        itLfE = pstNode->vKLfE.begin();
        itRtE = pstNode->vKRtE.begin();
        for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);
        }
    }
#endif
    /* set trussness and juSup, layer */
    for (itNeibP = vNeibP.begin(); itNeibP != vNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[itNeibP->first]);
        pstNode = mpG.findNode(pstPInfo->iEid);
        DEBUG_ASSERT(NULL != pstNode);

        /*DEBUG_PRINTF("BALL (%d, %d) k: %d lowB: %d layer: %d new: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstPInfo->iLowB, pstNode->iLayer,
                     pstNode->bNewFlag);*/

        if (pstNode->bNewFlag)
        {
            pstNode->vKLfE.clear();
            pstNode->vKRtE.clear();

            itLfE = pstNode->vLfE.begin();
            itRtE = pstNode->vRtE.begin();
            for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
            {
                TPST_MAP_BY_EID* pstLfNode = NULL;
                TPST_MAP_BY_EID* pstRtNode = NULL;

                pstLfNode = mpG.findNode(*itLfE);
                DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                DEBUG_ASSERT(NULL != pstRtNode);

                /*DEBUG_PRINTF("BALL neighbor: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d new: %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstLfNode->bNewFlag, pstRtNode->bNewFlag,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);*/

                int iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                //DEBUG_PRINTF("BALL iMinT %d\n", iMinT);
                if (iMinT > pstPInfo->iLowB)
                {
                    pstNode->vKLfE.push_back(pstLfNode->eid);
                    pstNode->vKRtE.push_back(pstRtNode->eid);
                }
                else if (iMinT == pstPInfo->iLowB)
                {
                    pstNode->vKLfE.push_back(pstLfNode->eid);
                    pstNode->vKRtE.push_back(pstRtNode->eid);
                    if (pstLfNode->iTrussness == pstPInfo->iLowB)
                    {
                        pstLfNode->vKLfE.push_back(pstRtNode->eid);
                        pstLfNode->vKRtE.push_back(pstNode->eid);
                    }
                    if (pstRtNode->iTrussness == pstPInfo->iLowB)
                    {
                        pstRtNode->vKLfE.push_back(pstNode->eid);
                        pstRtNode->vKRtE.push_back(pstLfNode->eid);
                    }
                }
                else
                {
                    //DEBUG_PRINTF("BALL bool %d\n", (!pstLfNode->bNewFlag) && (!pstRtNode->bNewFlag));
                    if ((!pstLfNode->bNewFlag) && (!pstRtNode->bNewFlag))
                    {
                        //DEBUG_PRINTF("BALL iTrussness %d %d\n", pstLfNode->iTrussness, pstRtNode->iTrussness);
                        if (pstLfNode->iTrussness == iMinT)
                        {
                            pstLfNode->vKLfE.push_back(pstRtNode->eid);
                            pstLfNode->vKRtE.push_back(pstNode->eid);
                            //DEBUG_PRINTF("BALL push back %d, %d\n", pstRtNode->eid, pstNode->eid);
                        }
                        if (pstRtNode->iTrussness == iMinT)
                        {
                            pstRtNode->vKLfE.push_back(pstNode->eid);
                            pstRtNode->vKRtE.push_back(pstLfNode->eid);
                            //DEBUG_PRINTF("BALL push back %d, %d\n", pstNode->eid, pstLfNode->eid);
                        }
                    }
                }
            }
            DEBUG_ASSERT(itRtE == pstNode->vRtE.end());
        }
        else if (pstPInfo->iLowB > pstNode->iTrussness)
        {
            vector<int> vKLfE;
            vector<int> vKRtE;

            itLfE = pstNode->vKLfE.begin();
            itRtE = pstNode->vKRtE.begin();
            for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
            {
                TPST_MAP_BY_EID* pstLfNode = NULL;
                TPST_MAP_BY_EID* pstRtNode = NULL;

                pstLfNode = mpG.findNode(*itLfE);
                DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                DEBUG_ASSERT(NULL != pstRtNode);

                /*DEBUG_PRINTF("BALL not new neighbor: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d new: %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstLfNode->bNewFlag, pstRtNode->bNewFlag,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);*/

                int iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                if (iMinT > pstPInfo->iLowB)
                {
                    vKLfE.push_back(pstLfNode->eid);
                    vKRtE.push_back(pstRtNode->eid);
                }
                else if (iMinT == pstPInfo->iLowB)
                {
                    vKLfE.push_back(pstLfNode->eid);
                    vKRtE.push_back(pstRtNode->eid);
                    if (pstLfNode->iTrussness == pstPInfo->iLowB)
                    {
                        pstLfNode->vKLfE.push_back(pstRtNode->eid);
                        pstLfNode->vKRtE.push_back(pstNode->eid);
                    }
                    if (pstRtNode->iTrussness == pstPInfo->iLowB)
                    {
                        pstRtNode->vKLfE.push_back(pstNode->eid);
                        pstRtNode->vKRtE.push_back(pstLfNode->eid);
                    }
                }
                else if (iMinT > pstNode->iTrussness)
                {
                    if (pstLfNode->iTrussness == iMinT)
                    {
                        pstLfNode->vKLfE.push_back(pstRtNode->eid);
                        pstLfNode->vKRtE.push_back(pstNode->eid);
                    }
                    if (pstRtNode->iTrussness == iMinT)
                    {
                        pstRtNode->vKLfE.push_back(pstNode->eid);
                        pstRtNode->vKRtE.push_back(pstLfNode->eid);
                    }
                }
            }
            DEBUG_ASSERT(itRtE == pstNode->vKRtE.end());
            pstNode->vKLfE.swap(vKLfE);
            pstNode->vKRtE.swap(vKRtE);
        }

        /* set low bound as trussness */
        pstNode->iTrussness = pstPInfo->iLowB;
        pstNode->iJuSup = pstPInfo->iJuSup;
        pstNode->iLayer = pstPInfo->iLayer;
        pstNode->iSeSup = pstPInfo->iSeSup;
        pstNode->bNewFlag = false;
    }
#if 0
    /* debug */
    for (itNeibP = vNeibP.begin(); itNeibP != vNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[itNeibP->first]);
        pstNode = mpG.findNode(pstPInfo->iEid);
        DEBUG_ASSERT(NULL != pstNode);

        int iDebugCnt = 0;
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);

            int iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (iMinT >= pstNode->iTrussness)
            {
                ++iDebugCnt;
            }

        }

        if (iDebugCnt != pstNode->vKLfE.size())
        {
            itLfE = pstNode->vKLfE.begin();
            itRtE = pstNode->vKRtE.begin();
            for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
            {
                pstLfNode = mpG.findNode(*itLfE);
                //DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                //DEBUG_ASSERT(NULL != pstRtNode);

                DEBUG_PRINTF("ERROR restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);

            }
            DEBUG_PRINTF("RESTORE (%d, %d) k: %d layer: %d cnt: %d %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         iDebugCnt, pstNode->vKLfE.size());
            DEBUG_ASSERT(0);
        }
    }

    for (itE = vThirdE.begin(); itE != vThirdE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        int iDebugCnt = 0;
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("DEBUG restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);

            int iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (iMinT >= pstNode->iTrussness)
            {
                ++iDebugCnt;
            }

        }

        if (iDebugCnt != pstNode->vKLfE.size())
        {
            itLfE = pstNode->vKLfE.begin();
            itRtE = pstNode->vKRtE.begin();
            for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
            {
                pstLfNode = mpG.findNode(*itLfE);
                //DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                //DEBUG_ASSERT(NULL != pstRtNode);

                DEBUG_PRINTF("ERROR restore: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);

            }
            DEBUG_PRINTF("RESTORE (%d, %d) k: %d layer: %d cnt: %d %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         iDebugCnt, pstNode->vKLfE.size());
            DEBUG_ASSERT(0);
        }
    }
#endif
    return 0;
}
#endif
/*****************
input:
        myG &mpG
        int iEid
description:
        find low bound for edge
******************/
int insertPart::edgeDec(myG &mpG, int iEid, vector <int> &vSeed)
{
    //list<int> lstCdtE;
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;

    //vector <int> vNebK;
    //int iNebKSize = 0;
    int iTriT = 0;
    int iMaxK = 0;

    vector <int> vNebL;
    int iNebLSize = 0;
    int iNebLCnt = 0;
    int iTriL = 0;
    int iHighL = 0;

	bool bLfFlag = false;
	bool bRtFlag = false;

	vector <vector <pair<int, int> > > vBin(mpG.m_iMaxK + 1);

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    /*DEBUG_PRINTF("DEBUG edge: (%d, %d) global max k: %d\n",
                 pstNode->paXY.first, pstNode->paXY.second, mpG.m_iMaxK);*/

    /* fill neighbor truss vector */
    itLfE = pstNode->vLfE.begin();
    itRtE = pstNode->vRtE.begin();
    for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLfNode = mpG.findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = mpG.findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRtNode);

        if (pstLfNode->iTrussness < 3)
        {
            /* must be 3-truss */
            pstLfNode->iTrussness = 3;
            ++g_lUgCnt;
        }
        if (pstRtNode->iTrussness < 3)
        {
            /* must be 3-truss */
            pstRtNode->iTrussness = 3;
            ++g_lUgCnt;
        }

        iTriT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        //DEBUG_PRINTF("DEBUG neighbor k: %d\n", iTriT);

        vBin[iTriT].push_back(pair<int, int>(pstLfNode->eid, pstRtNode->eid));
        iMaxK = COMMON_MAX(iMaxK, iTriT);
    }
    //DEBUG_ASSERT(itRtE == pstNode->vRtE.end());
    vBin.resize(iMaxK + 1);
    //DEBUG_PRINTF("DEBUG max k: %d\n", iMaxK);

    /* count neighbor truss vector, find trussness */
    int iNebKCnt = 0;
	int k = iMaxK;
    for (; k > 2; --k)
    {
        iNebKCnt += vBin[k].size();
        /*DEBUG_PRINTF("DEBUG (%d, %d) k: %d number: %d\n", pstNode->paXY.first, pstNode->paXY.second,
                     i, vNebK[i]);*/
        if (iNebKCnt + 2 >= k)
        {
            /* meet truss requirement */
            break;
        }
    }
    pstNode->iTrussness = COMMON_MAX(k, ((iMaxK > 0)?3:2));
    ++g_lUgCnt;
    /*DEBUG_PRINTF("DEBUG set (%d, %d) k: %d\n", pstNode->paXY.first, pstNode->paXY.second,
                 pstNode->iTrussness);*/

    //DEBUG_PRINTF("DEBUG get k: %d\n", pstNode->iTrussness);
    pstNode->iLayer = 1;
    iHighL = iNebKCnt - vBin[pstNode->iTrussness].size();
    /* fill neighbor layer vector */
    for (auto atPa : vBin[pstNode->iTrussness])
    {
        pstLfNode = mpG.findNode(atPa.first);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = mpG.findNode(atPa.second);
        DEBUG_ASSERT(NULL != pstRtNode);

        iTriT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        /*if (iTriT < pstNode->iTrussness)
        {
            continue;
        }
        else if (iTriT > pstNode->iTrussness)
        {
            ++iHighL;
            continue;
        }*/
        DEBUG_ASSERT(iTriT == pstNode->iTrussness);
        iTriL = COMMON_TRG_MIN(pstNode->iLayer,
                               pstLfNode->iLayer,
                               (pstLfNode->iTrussness > pstNode->iTrussness),
                               pstRtNode->iLayer,
                               (pstRtNode->iTrussness > pstNode->iTrussness));
        while (iTriL >= iNebLSize)
        {
            vNebL.push_back(0);
            ++iNebLSize;
        }
        DEBUG_ASSERT(iTriL < vNebL.size());
        vNebL[iTriL]++;
    }
    //DEBUG_ASSERT(itRtE == pstNode->vRtE.end());

    /* count neighbor layer vector, find layer */
    iNebLCnt = iHighL;
    if (0 == iNebLSize)
    {
        pstNode->iLayer = 1;
        pstNode->iSeSup = iNebLCnt;
    }
    else
    {
        int i = iNebLSize - 1;
        for (; i > 0; --i)
        {
            /*DEBUG_PRINTF("DEBUG (%d, %d) l: %d number: %d\n", pstNode->paXY.first, pstNode->paXY.second,
                         i, vNebL[i]);*/
            if (iNebLCnt + vNebL[i] + 2 > pstNode->iTrussness)
            {
                /* meet truss requirement */
                break;
            }
            iNebLCnt += vNebL[i];
        }
        /*if (0 == iNebLCnt)
        {
            pstNode->iLayer = i;
            pstNode->iSeSup = iNebLCnt + vNebL[i];
            if (pstNode->iSeSup + 2 > pstNode->iTrussness)
            {
                vSeed.push_back(pstNode->eid);
            }
        }
        else
        {
            pstNode->iLayer = i + 1;
            pstNode->iSeSup = iNebLCnt;
        }*/
        pstNode->iLayer = i + 1;
        pstNode->iSeSup = iNebLCnt;
    }
    /*DEBUG_PRINTF("DEBUG set (%d, %d) l: %d\n", pstNode->paXY.first, pstNode->paXY.second,
                 pstNode->iLayer);
    DEBUG_PRINTF("DEBUG set (%d, %d) seSup: %d\n", pstNode->paXY.first, pstNode->paXY.second,
                 pstNode->iSeSup);*/

    //DEBUG_PRINTF("DEBUG get L: %d\n", pstNode->iLayer);
    //pstNode->vKLfE.clear();
    //pstNode->vKRtE.clear();
    /* find candidature */
    itLfE = pstNode->vLfE.begin();
    itRtE = pstNode->vRtE.begin();
    for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLfNode = mpG.findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = mpG.findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRtNode);

        bLfFlag = false;
        bRtFlag = false;

        /*DEBUG_PRINTF("DEBUG visit: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d eid: %d %d self: (%d, %d) layer: %d\n",
                     pstLfNode->paXY.first, pstLfNode->paXY.second,
                     pstRtNode->paXY.first, pstRtNode->paXY.second,
                     pstLfNode->iTrussness, pstRtNode->iTrussness,
                     pstLfNode->iLayer, pstRtNode->iLayer,
                     pstLfNode->iSeSup, pstRtNode->iSeSup,
                     pstLfNode->eid, pstRtNode->eid,
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iLayer);*/

        iTriT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        if (iTriT > pstNode->iTrussness)
        {
            /*pstNode->vKLfE.push_back(pstLfNode->eid);
            pstNode->vKRtE.push_back(pstRtNode->eid);*/
            continue;
        }
        else if (iTriT < pstNode->iTrussness)
        {
            /*if (pstLfNode->iTrussness == iTriT)
            {
                pstLfNode->vKLfE.push_back(pstRtNode->eid);
                pstLfNode->vKRtE.push_back(pstNode->eid);
            }
            if (pstRtNode->iTrussness == iTriT)
            {
                pstRtNode->vKLfE.push_back(pstNode->eid);
                pstRtNode->vKRtE.push_back(pstLfNode->eid);
            }*/

            if (pstLfNode->iTrussness == pstRtNode->iTrussness)
            {
                iTriL = COMMON_MIN(pstLfNode->iLayer, pstRtNode->iLayer);
                if (pstLfNode->iLayer == iTriL)
                {
                    bLfFlag = true;
                }
                if (pstRtNode->iLayer == iTriL)
                {
                    bRtFlag = true;
                }
            }
            else if (iTriT == pstRtNode->iTrussness)
            {
                bRtFlag = true;
            }
            else if (iTriT == pstLfNode->iTrussness)
            {
                bLfFlag = true;
            }
            else
            {
                DEBUG_ASSERT(0);
            }
        }
        else
        {
            /*pstNode->vKLfE.push_back(pstLfNode->eid);
            pstNode->vKRtE.push_back(pstRtNode->eid);
            if (pstLfNode->iTrussness == pstNode->iTrussness)
            {
                pstLfNode->vKLfE.push_back(pstRtNode->eid);
                pstLfNode->vKRtE.push_back(pstNode->eid);
            }
            if (pstRtNode->iTrussness == pstNode->iTrussness)
            {
                pstRtNode->vKLfE.push_back(pstNode->eid);
                pstRtNode->vKRtE.push_back(pstLfNode->eid);
            }*/

            iTriL = COMMON_TRG_MIN(pstNode->iLayer,
                                   pstLfNode->iLayer,
                                   (pstLfNode->iTrussness > pstNode->iTrussness),
                                   pstRtNode->iLayer,
                                   (pstRtNode->iTrussness > pstNode->iTrussness));
            if (iTriL > pstNode->iLayer)
            {
                /* no change, ignore */
                continue;
            }

            if ((pstLfNode->iTrussness == pstNode->iTrussness) &&
                (pstLfNode->iLayer == iTriL))
            {
                bLfFlag = true;
            }
            if ((pstRtNode->iTrussness == pstNode->iTrussness) &&
                (pstRtNode->iLayer == iTriL))
            {
                bRtFlag = true;
            }
        }

        if (bLfFlag)
        {
            pstLfNode->iSeSup++;
            /*DEBUG_PRINTF("DEBUG increase left: (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstLfNode->iTrussness, pstLfNode->iLayer,
                         pstLfNode->iSeSup);*/
            if ((pstLfNode->iSeSup + 2 > pstLfNode->iTrussness) && (!pstLfNode->bCanQFlag))
            {
                /* may increase truss or layer */
                pstLfNode->bCanQFlag = true;
                vSeed.push_back(pstLfNode->eid);
            }
        }
        if (bRtFlag)
        {
            pstRtNode->iSeSup++;
            /*DEBUG_PRINTF("DEBUG increase right: (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstRtNode->iTrussness, pstRtNode->iLayer,
                         pstRtNode->iSeSup);*/
            if ((pstRtNode->iSeSup + 2 > pstRtNode->iTrussness) && (!pstRtNode->bCanQFlag) )
            {
                /* may increase truss or layer */
                pstRtNode->bCanQFlag = true;
                vSeed.push_back(pstRtNode->eid);
            }
        }
    }
    //DEBUG_ASSERT(itRtE == pstNode->vRtE.end());

    //pstNode->bNewFlag = false;
    //DEBUG_PRINTF("DEBUG edge done\n");

    return 0;
}
/*****************
input:
        myG &mpG
        map<int, list<int> > &mpPrivate
        int iNode
description:
        find all alley type
******************/
int insertPart::batchIns(myG &mpG, map<int, TPST_BALL_INFO > &mpBallInfo, vector<int> &vNodes)
{
    vector <int> vSeed;
	struct timeval tv;
	long lStartTime = 0;
	long lSubStartTime = 0;
	long lCurTime = 0;
#if 0
    /* simple add */
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    for (int iNode : vNodes)
    {
        TPST_BALL_INFO *pstBall = &(mpBallInfo[iNode]);
        if (INSERT_BALL_I == pstBall->iType)
        {
            if (pstBall->iPid < pstBall->iNeibP)
            {
                pstBall->iEid = simpleAdd(mpG, pstBall->iPid, pstBall->iNeibP);
            }
            else
            {
                pstBall->iEid = simpleAdd(mpG, pstBall->iNeibP, pstBall->iPid);
            }
        }
        else
        {
            for (auto atNeibP : pstBall->mpNeibP)
            {
                if (0 == atNeibP.second)
                {
                    /* new edge */
                    if (pstBall->iPid < atNeibP.first)
                    {
                        simpleAdd(mpG, pstBall->iPid, atNeibP.first);
                    }
                    else
                    {
                        simpleAdd(mpG, atNeibP.first, pstBall->iPid);
                    }
                }
            }
        }
    }
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    g_lSimpleAddTime += lCurTime - lStartTime;
#endif

    /* ball decomposition */
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    for (int iNode : vNodes)
    {
        TPST_BALL_INFO *pstBall = &(mpBallInfo[iNode]);
        //DEBUG_PRINTF("DEBUG node: %d type: %d\n", iNode, pstBall->iType);
        if (INSERT_BALL_I == pstBall->iType)
        {
            if (pstBall->iPid < pstBall->iNeibP)
            {
                pstBall->iEid = simpleAdd(mpG, pstBall->iPid, pstBall->iNeibP);
            }
            else
            {
                pstBall->iEid = simpleAdd(mpG, pstBall->iNeibP, pstBall->iPid);
            }
            /*gettimeofday(&tv, NULL);
            lSubStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;*/
            edgeDec(mpG, pstBall->iEid, vSeed);
            ++g_lBallSize;
            /*gettimeofday(&tv, NULL);
            lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            g_lEdgeDecTime += lCurTime - lSubStartTime;*/
        }
        else
        {
            //DEBUG_PRINTF("DEBUG add begin\n");
            for (int iNeibP : pstBall->vNewP)
            {
                /* new edge */
                if (pstBall->iPid < iNeibP)
                {
                    simpleAdd(mpG, pstBall->iPid, iNeibP);
                }
                else
                {
                    simpleAdd(mpG, iNeibP, pstBall->iPid);
                }
            }
            //DEBUG_PRINTF("DEBUG add end\n");
            /*gettimeofday(&tv, NULL);
            lSubStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;*/
            localDec(mpG, iNode, mpG.m_vAdj[iNode], vSeed);
            g_lBallSize += mpG.m_vAdj[iNode].size();
            /*gettimeofday(&tv, NULL);
            lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            g_lLocalDecTime += lCurTime - lSubStartTime;*/
            //DEBUG_PRINTF("DEBUG ball end\n");
        }
        //DEBUG_PRINTF("DEBUG node: %d done\n", iNode);
    }
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lBallDecTime += lCurTime - lStartTime;
    //DEBUG_PRINTF("BATCH_INSERT local done\n");
    /* maintain outside */
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    upgradeForAllK(mpG, vSeed);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lOutBallTime += lCurTime - lStartTime;
    return 0;
}
#if 0
/*****************
input:
        myG &mpG
        map<int, vector <int> > &mpPrivate
        int iNode
description:
        find all alley type
******************/
int insertPart::disjoint(map<int, TPST_BALL_INFO > &mpBallInfo, vector< vector<int> > &vInsScheme)
{
    /* <d, pid>, none */
    map<pair<int, int>, int> mpOrder;
    map<pair<int, int>, int>::reverse_iterator ritMp;
    /* pid */
    vector<int> vOrder;
    vector<int>::iterator itNode;
    map<int, TPST_BALL_INFO >::iterator itmpNode;

    /* init */
    for (itmpNode = mpBallInfo.begin(); itmpNode != mpBallInfo.end(); ++itmpNode)
    {
        itmpNode->second.bUsed = false;
        mpOrder[pair<int, int>(itmpNode->second.mpNeibP.size(), itmpNode->second.iPid)] = 0;
    }
    for (ritMp = mpOrder.rbegin(); ritMp != mpOrder.rend(); ++ritMp)
    {
        DEBUG_PRINTF("DEBUG DISJOINT node: %d degree: %d\n", ritMp->first.second, ritMp->first.first);
        vOrder.push_back(ritMp->first.second);
    }

    for (itNode = vOrder.begin(); itNode != vOrder.end(); ++itNode)
    {
        TPST_BALL_INFO *pstBall = &(mpBallInfo[*itNode]);
        if (pstBall->bUsed)
        {
            /* used */
            continue;
        }

        /* new scheme */
        vector<int> vScheme;
        vector<int> *pvScheme;

        /* pid, none */
        map<int, int> mpNodePool;
        vInsScheme.push_back(vScheme);
        vector< vector<int> >::iterator itScheme = vInsScheme.end();
        --itScheme;
        pvScheme = &(*itScheme);

        pvScheme->push_back(pstBall->iPid);
        //mpNodePool = pstBall->mpNeibP;
        mpNodePool[pstBall->iPid] = 0;
        pstBall->bUsed = true;

        /* find next */
        vector<int>::iterator itNext = itNode;
        ++itNext;
        for (; itNext != vOrder.end(); ++itNext)
        {
            TPST_BALL_INFO *pstNextBall = &(mpBallInfo[*itNext]);
            if (pstNextBall->bUsed)
            {
                /* used */
                continue;
            }
            bool bIntFlag = false;
            for (int iChoPid : *pvScheme)
            {
                if (pstNextBall->mpNeibP.find(iChoPid) != pstNextBall->mpNeibP.end())
                {
                    /* Intersect */
                    bIntFlag = true;
                    break;
                }
                pstBall = &(mpBallInfo[iChoPid]);
                if (pstBall->mpNeibP.find(pstNextBall->iPid) != pstBall->mpNeibP.end())
                {
                    /* Intersect */
                    bIntFlag = true;
                    break;
                }
            }
            if (bIntFlag)
            {
                /* Intersect */
                continue;
            }

            /* add one */
            pvScheme->push_back(pstNextBall->iPid);
            pstNextBall->bUsed = true;
            //mpNodePool.insert(pstNextBall->mpNeibP.begin(), pstNextBall->mpNeibP.end());
            mpNodePool[pstNextBall->iPid] = 0;
        }
    }
    return 0;
}
#endif
/*****************
input:
        myG &mpG
        map<int, vector <int> > &mpPrivate
        int iNode
description:
        find all alley type
******************/
int insertPart::fillInfo(myG &mpG, map<int, vector <int> > &mpPrivate, list<pair<int, int> > &lstQuery, map<int, TPST_BALL_INFO > &mpBallInfo)
{
    //vector <pair<int, int> > vNeibP;
    //vector <int> vThirdE;
    TPST_BALL_INFO *pstBall;

    map<int, vector <int> >::iterator itPri;
    for (auto atQ : lstQuery)
    {
        int iNode = atQ.first;

        itPri = mpPrivate.find(iNode);
        if (itPri == mpPrivate.end())
        {
            /* no such ball */
            continue;
        }
        if (mpBallInfo.find(iNode) != mpBallInfo.end())
        {
            /* visited */
            continue;
        }

        DEBUG_ASSERT(!itPri->second.empty());
        if (1 == itPri->second.size())
        {
            /* I-ball */
            int iNeib = itPri->second[0];
            /*DEBUG_PRINTF("FILL_INFO I-Ball (%d, %d) maxP: %d\n",
                         iNode, iNeib, mpG.m_iMaxPId);*/
            if (NULL != mpG.findNode(iNode, iNeib))
            {
                /* joined, ignore */
                continue;
            }

            /* I-ball */
            pstBall = &(mpBallInfo[iNode]);
            pstBall->iPid = iNode;
            pstBall->iType = INSERT_BALL_I;

            pstBall->iNeibP = iNeib;
        }
        else
        {
            /* II-ball *///
            /* II-ball */
            pstBall = &(mpBallInfo[iNode]);
            pstBall->iPid = iNode;
            pstBall->iType = INSERT_BALL_II;
            pstBall->vNewP.swap(itPri->second);
        }
    }
    return 0;
}
/*****************
input:
        myG &mpG
        map<int, vector <int> > &mpPrivate
        int iNode
description:
        find all alley type
******************/
int insertPart::insertAll(myG &mpG, map<int, vector <int> > &mpPrivate, list<pair<int, int> > &lstQuery)
{
    map<int, TPST_BALL_INFO > mpBallInfo;
    vector< vector<int> > vInsScheme;
    vector<int> vAllQ;

	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;

	//DEBUG_PRINTF("INSERT_ALL fill info start\n");
    /* fill info */
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    fillInfo(mpG, mpPrivate, lstQuery, mpBallInfo);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    g_lFillInfoTime += lCurTime - lStartTime;
	//DEBUG_PRINTF("INSERT_ALL fill info end\n");

    /* find disjoint insertion */
    //disjoint(mpBallInfo, vInsScheme);

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    /*for (auto atScheme : vInsScheme)
    {
        batchIns(mpG, mpBallInfo, atScheme);
    }*/

    for (auto atQ : lstQuery)
    {
        vAllQ.push_back(atQ.first);
    }
    batchIns(mpG, mpBallInfo, vAllQ);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    g_lBatchInsTotalTime += lCurTime - lStartTime;
	//DEBUG_PRINTF("INSERT_ALL insert end\n");

    return 0;
}
