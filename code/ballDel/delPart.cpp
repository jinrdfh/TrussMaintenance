/***************
insertNode function
****************/

#include <unordered_map>

#include "common.h"
#include "myG.h"
#include "delPart.h"

extern long g_lPreLoadTime;
extern long g_lPreRmLoadTime;
extern long g_lPreRmTime;
extern long g_lPreRmCoreTime;
extern long g_lPreRmBasicTime;
extern long g_lPreFindTime;
extern long g_lLocalTime;
extern long g_lAfterLocalTime;
extern long g_lUpdateKTime;
extern long g_lRecalLTime;
extern long g_lUpdateLTime;

long g_lBFSCnt;
long g_lDeCnt;
#if 0
/*****************
input:
        myG &mpG
        int iEid
description:
        check removal condition
******************/
int delPart::lcDecomp(myG &mpG, int iCurT, int iCurL, vector <int> &vUpSeed)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;
    int iMinT = 0;
    int iNewLayer = 0;
    int iCurLayer = iCurL;
    int iCurK = iCurT;
    bool bLfFlag = false;
    bool bRtFlag = false;
    vector<int>::iterator itvE;
    /* little */
    myPriQueue myCanQ;
    vector <int> vWaitQ;
    vector <int> vWaitQSla;
    vector <int> vVisitQ;

    DEBUG_PRINTF("LOCAL enter k: %d layer: %d\n", iCurT, iCurL);

    /* Find neighbor and increase SeSup */
    for (itvE = vUpSeed.begin(); itvE != vUpSeed.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);

        DEBUG_ASSERT(pstNode->iSeSup + 2 > iCurK);
        pstNode->bCanQFlag = false;
        pstNode->bUsedCanFlag = false;
        pstNode->bDgdFlag = true;
        vVisitQ.push_back(pstNode->eid);
        DEBUG_PRINTF("LOCAL seed: (%d, %d)\n",
                     pstNode->paXY.first, pstNode->paXY.second);

        DEBUG_ASSERT(iCurLayer == pstNode->iLayer);

        vector<int> vLfE;
        vector<int> vRtE;
        mpG.findNeb(pstNode->eid, vLfE, vRtE);

        itLfE = vLfE.begin();
        itRtE = vRtE.begin();
        for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = mpG.findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = mpG.findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRtNode);

            DEBUG_PRINTF("LOCAL visit: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d eid: %d %d self: (%d, %d) layer: %d\n",
                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                         pstLfNode->iTrussness, pstRtNode->iTrussness,
                         pstLfNode->iLayer, pstRtNode->iLayer,
                         pstLfNode->iSeSup, pstRtNode->iSeSup,
                         pstLfNode->eid, pstRtNode->eid,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iLayer);

            iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (iMinT != iCurK)
            {
                /* not meet, ignore */
                continue;
            }
            iNewLayer = COMMON_TRG_MIN(pstNode->iLayer + 1,
                            pstLfNode->iLayer,
                            (pstLfNode->iTrussness > iCurK),
                            pstRtNode->iLayer,
                            (pstRtNode->iTrussness > iCurK));
            if (iNewLayer != iCurLayer)
            {
                continue;
            }

            if ((iCurK == pstLfNode->iTrussness) &&
                (iNewLayer == pstLfNode->iLayer) &&
                (pstLfNode->iSeSup + 2 <= iCurK) &&
                (!pstLfNode->bCanQFlag))
            {
                vWaitQ.push_back(pstLfNode->eid);
                pstLfNode->bCanQFlag = true;
                pstLfNode->bUsedCanFlag = true;
                vVisitQ.push_back(pstLfNode->eid);
            }
            if ((iCurK == pstRtNode->iTrussness) &&
                (iNewLayer == pstRtNode->iLayer) &&
                (pstRtNode->iSeSup + 2 <= iCurK) &&
                (!pstRtNode->bCanQFlag))
            {
                vWaitQ.push_back(pstRtNode->eid);
                pstRtNode->bCanQFlag = true;
                pstRtNode->bUsedCanFlag = true;
                vVisitQ.push_back(pstRtNode->eid);
            }
        }
    }

    /* peeling */
    while (!vWaitQ.empty())
    {
        /* set flag */
        for (itvE = vWaitQ.begin(); itvE != vWaitQ.end(); ++itvE)
        {
            pstNode = mpG.findNode(*itvE);
            DEBUG_ASSERT(NULL != pstNode);

            DEBUG_ASSERT(pstNode->bCanQFlag);
            pstNode->bDgdFlag = false;
        }
        for (itvE = vWaitQ.begin(); itvE != vWaitQ.end(); ++itvE)
        {
            pstNode = mpG.findNode(*itvE);
            DEBUG_ASSERT(NULL != pstNode);

            DEBUG_PRINTF("LOCAL current: (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup);

            DEBUG_ASSERT(pstNode->iSeSup >= 0);

            pstNode->bRmFlag = true;
            pstNode->bCanQFlag = false;
            pstNode->bUsedCanFlag = false;

            vector<int> vLfE;
            vector<int> vRtE;
            mpG.findNeb(pstNode->eid, vLfE, vRtE);

            itLfE = vLfE.begin();
            itRtE = vRtE.begin();
            for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
            {
                pstLfNode = mpG.findNode(*itLfE);
                DEBUG_ASSERT(NULL != pstLfNode);
                pstRtNode = mpG.findNode(*itRtE);
                DEBUG_ASSERT(NULL != pstRtNode);

                DEBUG_PRINTF("LOCAL visit: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d eid: %d %d bool: %d %d %d %d %d %d self: (%d, %d) layer: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstLfNode->eid, pstRtNode->eid,
                             pstLfNode->bRmFlag, pstRtNode->bRmFlag,
                             pstLfNode->bDgdFlag, pstRtNode->bDgdFlag,
                             pstLfNode->bCanQFlag, pstRtNode->bCanQFlag,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer);

                iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                if (iMinT != iCurK)
                {
                    /* not meet, ignore */
                    continue;
                }
                if (pstLfNode->bRmFlag || pstRtNode->bRmFlag)
                {
                    continue;
                }
                bLfFlag = false;
                bRtFlag = false;
                if (pstLfNode->bDgdFlag && pstRtNode->bDgdFlag)
                {
                    bLfFlag = true;
                    bRtFlag = true;
                }
                else if (pstLfNode->bDgdFlag)
                {
                    if ((pstRtNode->iTrussness > iCurK) ||
                        (pstRtNode->bDgdFlag) ||
                        (pstRtNode->bCanQFlag))
                    {
                        bLfFlag = true;
                    }
                }
                else if (pstRtNode->bDgdFlag)
                {
                    if ((pstLfNode->iTrussness > iCurK) ||
                        (pstLfNode->bDgdFlag) ||
                        (pstLfNode->bCanQFlag))
                    {
                        bRtFlag = true;
                    }
                }
                else
                {
                    continue;
                }

                if (bLfFlag)
                {
                    pstLfNode->iSeSup--;
                    DEBUG_PRINTF("LOCAL decrease SeSup: (%d, %d) k: %d layer: %d seSup: %d\n",
                                 pstLfNode->paXY.first, pstLfNode->paXY.second,
                                 pstLfNode->iTrussness, pstLfNode->iLayer,
                                 pstLfNode->iSeSup);
                    if (pstLfNode->iSeSup + 2 <= iCurK)
                    {
                        /* wait to be removed */
                        if (!pstLfNode->bCanQFlag)
                        {
                            pstLfNode->bCanQFlag = true;
                            pstLfNode->bUsedCanFlag = true;
                            pstLfNode->iLayer = iCurLayer + 1;
                            vWaitQSla.push_back(pstLfNode->eid);
                            vVisitQ.push_back(pstLfNode->eid);
                            DEBUG_PRINTF("LOCAL set: (%d, %d) k: %d layer: %d seSup: %d\n",
                                         pstLfNode->paXY.first, pstLfNode->paXY.second,
                                         pstLfNode->iTrussness, pstLfNode->iLayer,
                                         pstLfNode->iSeSup);
                        }
                    }
                }
                if (bRtFlag)
                {
                    pstRtNode->iSeSup--;
                    DEBUG_PRINTF("LOCAL decrease SeSup: (%d, %d) k: %d layer: %d seSup: %d\n",
                                 pstRtNode->paXY.first, pstRtNode->paXY.second,
                                 pstRtNode->iTrussness, pstRtNode->iLayer,
                                 pstRtNode->iSeSup);
                    if (pstRtNode->iSeSup + 2 <= iCurK)
                    {
                        /* wait to be removed */
                        if (!pstRtNode->bCanQFlag)
                        {
                            pstRtNode->bCanQFlag = true;
                            pstRtNode->bUsedCanFlag = true;
                            pstRtNode->iLayer = iCurLayer + 1;
                            vWaitQSla.push_back(pstRtNode->eid);
                            vVisitQ.push_back(pstRtNode->eid);
                            DEBUG_PRINTF("LOCAL set: (%d, %d) k: %d layer: %d seSup: %d\n",
                                         pstRtNode->paXY.first, pstRtNode->paXY.second,
                                         pstRtNode->iTrussness, pstRtNode->iLayer,
                                         pstRtNode->iSeSup);
                        }
                    }
                }
            }
        }
        ++iCurLayer;
        vWaitQ.swap(vWaitQSla);
        vWaitQSla.clear();
    }
    /* restore */
    for (itvE = vVisitQ.begin(); itvE != vVisitQ.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);
        pstNode->bCanQFlag = false;
        pstNode->bUsedCanFlag = false;
        pstNode->bDgdFlag = false;
        pstNode->bRmFlag = false;
    }
    return 0;
}
/*****************
input:
        myG &mpG
        int iCurT
        vector <int> &vSeed
        vector <int> &vDgdE
        vector <int> &vDgdSeed
description:
        increase k directly
******************/
int delPart::deKByBFSEasy(myG &mpG, int iCurT, vector <int> &vSeed, vector <int> &vDgdE, vector <int> &vDgdSeed)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstTpNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;
    list<int>::iterator itE;
    vector <int> vVisit;
    vector <int> vUgdSeed;
    vector<int>::iterator itvE;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;

    int iCurEid = 0;
    int iCurLayer = 0;
    int iMinT = 0;
    int iCurK = iCurT;
    int iNewLayer = 0;
    int iHighLCnt = 0;
    int iSameLCnt = 0;
    int iCurMaxLayer = 0;
    int iLfLayer = 0;
    int iRtLayer = 0;

    struct timeval tv;
	long lStartTime = 0;
	long lLcStartTime = 0;
	long lCurTime = 0;

    /* <layer, eid> */
    myPriQueueBig myCanQ;
    list <int> lsCurLayer;

    bool res = false;
	bool bLfCheckFlag = false;
	bool bRtCheckFlag = false;

    DEBUG_ASSERT(vDgdSeed.empty());
    DEBUG_PRINTF("DEBUG start BFS k: %d\n",
                 iCurT);
    for (itvE = vSeed.begin(); itvE != vSeed.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);

        if (!pstNode->bVstFlag)
        {
            pstNode->bVstFlag = true;
            ++g_lBFSCnt;
            vVisit.push_back(pstNode->eid);
            myCanQ.insetByOrder(pstNode->iLayer, pstNode->eid);
            pstNode->bCanQFlag = true;
            pstNode->bUsedCanFlag = true;

            iCurMaxLayer = COMMON_MAX(iCurMaxLayer, pstNode->iLayer);

            DEBUG_PRINTF("DEBUG seed: (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup);
        }
    }

    iCurLayer = iCurMaxLayer;

    DEBUG_PRINTF("DEBUG start queue\n");
    while ((!myCanQ.empty()) || (!lsCurLayer.empty()))
    {
        if (!myCanQ.empty())
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
                /* insert back */
                while (!lsCurLayer.empty())
                {
                    iCurEid = lsCurLayer.front();
                    pstTpNode = mpG.findNode(iCurEid);
                    DEBUG_ASSERT(NULL != pstTpNode);

                    myCanQ.insetByOrder(pstTpNode->iLayer, pstTpNode->eid);
                    pstTpNode->bUsedCanFlag = true;
                    lsCurLayer.pop_front();
                }
                iCurLayer = pstNode->iLayer;
            }

            /* fill current layer */
            while (pstNode->iLayer >= iCurLayer)
            {
                lsCurLayer.push_back(pstNode->eid);
                myCanQ.pop();
                pstNode->bUsedCanFlag = false;
                if (myCanQ.empty())
                {
                    break;
                }
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
            }
        }

        if (lsCurLayer.empty())
        {
            /* decrease layer, set the lowest value */
            iCurLayer = 1;
            continue;
        }
        DEBUG_PRINTF("DEBUG ------layer: %d\n",
                         iCurLayer);
        if (1 == iCurLayer)
        {
            /* BFS */
            for (itE = lsCurLayer.begin(); itE != lsCurLayer.end(); )
            {
                pstNode = mpG.findNode(*itE);
                DEBUG_ASSERT(NULL != pstNode);

                DEBUG_PRINTF("DEBUG ------queue: (%d, %d) k: %d layer: %d seSup: %d\n",
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iTrussness, pstNode->iLayer,
                             pstNode->iSeSup);
                DEBUG_ASSERT(iCurLayer == pstNode->iLayer);

                vector<int> vLfE;
                vector<int> vRtE;
                mpG.findNeb(pstNode->eid, vLfE, vRtE);

                itLfE = vLfE.begin();
                itRtE = vRtE.begin();
                for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
                {
                    pstLfNode = mpG.findNode(*itLfE);
                    DEBUG_ASSERT(NULL != pstLfNode);
                    pstRtNode = mpG.findNode(*itRtE);
                    DEBUG_ASSERT(NULL != pstRtNode);

                    bLfCheckFlag = false;
                    bRtCheckFlag = false;

                    DEBUG_PRINTF("DEBUG visit: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d eid: %d %d self: (%d, %d) layer: %d\n",
                                 pstLfNode->paXY.first, pstLfNode->paXY.second,
                                 pstRtNode->paXY.first, pstRtNode->paXY.second,
                                 pstLfNode->iTrussness, pstRtNode->iTrussness,
                                 pstLfNode->iLayer, pstRtNode->iLayer,
                                 pstLfNode->iSeSup, pstRtNode->iSeSup,
                                 pstLfNode->eid, pstRtNode->eid,
                                 pstNode->paXY.first, pstNode->paXY.second,
                                 pstNode->iLayer);

                    iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                    if (iMinT != iCurK)
                    {
                        /* not meet, ignore */
                        continue;
                    }

                    /* visited Queue check */
                    iNewLayer = COMMON_TRG_MIN(pstNode->iLayer + 1,
                                    pstLfNode->iLayer,
                                    (pstLfNode->iTrussness > iCurK),
                                    pstRtNode->iLayer,
                                    (pstRtNode->iTrussness > iCurK));
                    if (1 > iNewLayer)
                    {
                        /* no change */
                        continue;
                    }
                    else if (iNewLayer > 1)
                    {
                        if ((pstLfNode->iTrussness == iCurK) &&
                            (pstLfNode->iLayer == 2) &&
                            (!pstLfNode->bCanQFlag))
                        {
                            bLfCheckFlag = true;
                        }
                        if ((pstRtNode->iTrussness == iCurK) &&
                            (pstRtNode->iLayer == 2) &&
                            (!pstRtNode->bCanQFlag))
                        {
                            bRtCheckFlag = true;
                        }
                    }
                    else if (iNewLayer == 1)
                    {
                        /* should decrease others SeSup */
                        if (pstLfNode->iTrussness == iCurK)
                        {
                            if (pstLfNode->iLayer == 1)
                            {
                                if (!pstLfNode->bCanQFlag)
                                {
                                    res = checkRmFirstCdt(mpG, pstLfNode->eid);
                                    if (!res)
                                    {
                                        lsCurLayer.push_back(pstLfNode->eid);
                                        pstLfNode->bCanQFlag = true;
                                        DEBUG_PRINTF("BFS push left: (%d, %d) k: %d layer: %d seSup: %d\n",
                                                     pstLfNode->paXY.first, pstLfNode->paXY.second,
                                                     pstLfNode->iTrussness, pstLfNode->iLayer,
                                                     pstLfNode->iSeSup);
                                    }
                                }
                            }
                            else if ((pstLfNode->iLayer == 2) &&
                                     (!pstLfNode->bCanQFlag))
                            {
                                bLfCheckFlag = true;
                            }
                        }
                        if (pstRtNode->iTrussness == iCurK)
                        {
                            if (pstRtNode->iLayer == 1)
                            {
                                if (!pstRtNode->bCanQFlag)
                                {
                                    res = checkRmFirstCdt(mpG, pstRtNode->eid);
                                    if (!res)
                                    {
                                        lsCurLayer.push_back(pstRtNode->eid);
                                        pstRtNode->bCanQFlag = true;
                                        DEBUG_PRINTF("BFS push right: (%d, %d) k: %d layer: %d seSup: %d\n",
                                                     pstRtNode->paXY.first, pstRtNode->paXY.second,
                                                     pstRtNode->iTrussness, pstRtNode->iLayer,
                                                     pstRtNode->iSeSup);
                                    }
                                }
                            }
                            else if ((pstRtNode->iLayer == 2) &&
                                     (!pstRtNode->bCanQFlag))
                            {
                                bRtCheckFlag = true;
                            }
                        }
                    }

                    if (bLfCheckFlag)
                    {
                        res = checkRmSecCdt(mpG, pstLfNode->eid);
                        if (!res)
                        {
                            DEBUG_ASSERT(!pstLfNode->bCanQFlag);
                            DEBUG_ASSERT(!pstLfNode->bUsedCanFlag);
                            myCanQ.insetByOrder(pstLfNode->iLayer, pstLfNode->eid);
                            pstLfNode->bCanQFlag = true;
                            pstLfNode->bUsedCanFlag = true;
                        }
                    }
                    if (bRtCheckFlag)
                    {
                        res = checkRmSecCdt(mpG, pstRtNode->eid);
                        if (!res)
                        {
                            DEBUG_ASSERT(!pstRtNode->bCanQFlag);
                            DEBUG_ASSERT(!pstRtNode->bUsedCanFlag);
                            myCanQ.insetByOrder(pstRtNode->iLayer, pstRtNode->eid);
                            pstRtNode->bCanQFlag = true;
                            pstRtNode->bUsedCanFlag = true;
                        }
                    }
                }
                ++itE;
            }

            /* clear other layer */
            if (!myCanQ.empty())
            {
                continue;
            }

            /* cannot be lower, degrade */
            DEBUG_ASSERT(vDgdE.empty());
            iCurK--;
            DEBUG_PRINTF("DEBUG ------k: %d\n",
                         iCurK);
            /* find maximum layer */
            for (itE = lsCurLayer.begin(); itE != lsCurLayer.end(); ++itE)
            {
                pstNode = mpG.findNode(*itE);
                DEBUG_ASSERT(NULL != pstNode);

                vector<int> vLfE;
                vector<int> vRtE;
                mpG.findNeb(pstNode->eid, vLfE, vRtE);

                itLfE = vLfE.begin();
                itRtE = vRtE.begin();
                for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
                {
                    pstLfNode = mpG.findNode(*itLfE);
                    DEBUG_ASSERT(NULL != pstLfNode);
                    pstRtNode = mpG.findNode(*itRtE);
                    DEBUG_ASSERT(NULL != pstRtNode);

                    DEBUG_PRINTF("DEBUG find: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d eid: %d %d bool: %d %d self: (%d, %d) layer: %d\n",
                                 pstLfNode->paXY.first, pstLfNode->paXY.second,
                                 pstRtNode->paXY.first, pstRtNode->paXY.second,
                                 pstLfNode->iTrussness, pstRtNode->iTrussness,
                                 pstLfNode->iLayer, pstRtNode->iLayer,
                                 pstLfNode->iSeSup, pstRtNode->iSeSup,
                                 pstLfNode->eid, pstRtNode->eid,
                                 pstLfNode->bCanQFlag, pstRtNode->bCanQFlag,
                                 pstNode->paXY.first, pstNode->paXY.second,
                                 pstNode->iLayer);

                    iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                    /* decrease first layer SeSup */
                    if (iMinT == iCurK + 1)
                    {
                        iLfLayer = (pstLfNode->iTrussness > iMinT)?pstNode->iLayer + 1:pstLfNode->iLayer;
                        iLfLayer = (pstLfNode->bCanQFlag)?0:iLfLayer;
                        iRtLayer = (pstRtNode->iTrussness > iMinT)?pstNode->iLayer + 1:pstRtNode->iLayer;
                        iRtLayer = (pstRtNode->bCanQFlag)?0:iRtLayer;
                        iNewLayer = COMMON_MIN(iLfLayer, iRtLayer);

                        DEBUG_PRINTF("DEBUG layer: %d left: %d right: %d\n",
                                     iNewLayer, iLfLayer, iRtLayer);
                        if (1 == iNewLayer)
                        {
                            if (1 == iLfLayer)
                            {
                                pstLfNode->iSeSup--;
                                DEBUG_PRINTF("DEBUG decrease back: (%d, %d) k: %d layer: %d seSup: %d\n",
                                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                                             pstLfNode->iTrussness, pstLfNode->iLayer,
                                             pstLfNode->iSeSup);
                            }
                            if (1 == iRtLayer)
                            {
                                pstRtNode->iSeSup--;
                                DEBUG_PRINTF("DEBUG decrease back: (%d, %d) k: %d layer: %d seSup: %d\n",
                                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                                             pstLfNode->iTrussness, pstLfNode->iLayer,
                                             pstLfNode->iSeSup);
                            }
                        }
                        else if (0 == iNewLayer)
                        {
                            if ((0 == iLfLayer) &&
                                (pstNode->eid < pstLfNode->eid) &&
                                (1 == iRtLayer))
                            {
                                pstRtNode->iSeSup--;
                                DEBUG_PRINTF("DEBUG decrease back: (%d, %d) k: %d layer: %d seSup: %d\n",
                                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                                             pstLfNode->iTrussness, pstLfNode->iLayer,
                                             pstLfNode->iSeSup);
                            }
                            if ((0 == iRtLayer) &&
                                (pstNode->eid < pstRtNode->eid) &&
                                (1 == iLfLayer))
                            {
                                pstLfNode->iSeSup--;
                                DEBUG_PRINTF("DEBUG decrease back: (%d, %d) k: %d layer: %d seSup: %d\n",
                                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                                             pstLfNode->iTrussness, pstLfNode->iLayer,
                                             pstLfNode->iSeSup);
                            }
                        }
                        continue;
                    }
                    else if (iMinT != iCurK)
                    {
                        /* not meet, ignore */
                        continue;
                    }

                    iNewLayer = COMMON_TRG_MIN(pstNode->iLayer,
                                    pstLfNode->iLayer,
                                    (pstLfNode->iTrussness > iCurK),
                                    pstRtNode->iLayer,
                                    (pstRtNode->iTrussness > iCurK));

                    if (iNewLayer > iCurMaxLayer)
                    {
                        iCurMaxLayer = iNewLayer;
                    }
                }
            }

            /* set value */
            ++iCurMaxLayer;
            iCurLayer = iCurMaxLayer;
            for (itE = lsCurLayer.begin(); itE != lsCurLayer.end(); ++itE)
            {
                pstNode = mpG.findNode(*itE);
                DEBUG_ASSERT(NULL != pstNode);
                /* set value */
                pstNode->bCanQFlag = false;
                pstNode->iTrussness = iCurK;
                /* maximum */
                pstNode->iLayer = iCurLayer;

                vDgdE.push_back(pstNode->eid);

                res = checkRmCdt(mpG, pstNode->eid);
                if (!res)
                {
                    /* seed in k-1 */
                    vDgdSeed.push_back(pstNode->eid);
                }

                /* handle upgrade case */
                if (pstNode->iSeSup + 2 > iCurK)
                {
                    vUgdSeed.push_back(pstNode->eid);
                }
                DEBUG_PRINTF("DEGRADE set: (%d, %d) k: %d layer: %d seSup: %d\n",
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iTrussness, pstNode->iLayer,
                             pstNode->iSeSup);
            }
            break;
        }
        else
        {
            for (itE = lsCurLayer.begin(); itE != lsCurLayer.end(); )
            {
                pstNode = mpG.findNode(*itE);
                DEBUG_ASSERT(NULL != pstNode);

                DEBUG_PRINTF("DEBUG ------queue: (%d, %d) k: %d layer: %d seSup: %d\n",
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iTrussness, pstNode->iLayer,
                             pstNode->iSeSup);
                DEBUG_ASSERT(iCurLayer == pstNode->iLayer);
                pstNode->iLayer--;
                pstNode->bCanQFlag = false;

                iHighLCnt = 0;
                iSameLCnt = 0;

                vector<int> vLfE;
                vector<int> vRtE;
                mpG.findNeb(pstNode->eid, vLfE, vRtE);

                itLfE = vLfE.begin();
                itRtE = vRtE.begin();
                for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
                {
                    pstLfNode = mpG.findNode(*itLfE);
                    DEBUG_ASSERT(NULL != pstLfNode);
                    pstRtNode = mpG.findNode(*itRtE);
                    DEBUG_ASSERT(NULL != pstRtNode);

                    bLfCheckFlag = false;
                    bRtCheckFlag = false;

                    DEBUG_PRINTF("DEBUG visit: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d eid: %d %d self: (%d, %d) layer: %d\n",
                                 pstLfNode->paXY.first, pstLfNode->paXY.second,
                                 pstRtNode->paXY.first, pstRtNode->paXY.second,
                                 pstLfNode->iTrussness, pstRtNode->iTrussness,
                                 pstLfNode->iLayer, pstRtNode->iLayer,
                                 pstLfNode->iSeSup, pstRtNode->iSeSup,
                                 pstLfNode->eid, pstRtNode->eid,
                                 pstNode->paXY.first, pstNode->paXY.second,
                                 pstNode->iLayer);

                    iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
                    if (iMinT > iCurK)
                    {
                        /* not meet, ignore */
                        ++iHighLCnt;
                        continue;
                    }
                    else if (iMinT < iCurK)
                    {
                        /* not meet, ignore */
                        continue;
                    }
                    iLfLayer = pstLfNode->iLayer;
                    iRtLayer = pstRtNode->iLayer;
                    /* visited Queue check */
                    iNewLayer = COMMON_TRG_MIN(pstNode->iLayer + 1,
                                    iLfLayer,
                                    (pstLfNode->iTrussness > iCurK),
                                    iRtLayer,
                                    (pstRtNode->iTrussness > iCurK));
                    if (1 > iNewLayer)
                    {
                        /* no change */
                        continue;
                    }
                    else if (iNewLayer > pstNode->iLayer + 1)
                    {
                        ++iHighLCnt;
                        if ((pstLfNode->iTrussness == iCurK) &&
                            (pstLfNode->iLayer == pstNode->iLayer + 2) &&
                            (!pstLfNode->bCanQFlag))
                        {
                            bLfCheckFlag = true;
                        }
                        if ((pstRtNode->iTrussness == iCurK) &&
                            (pstRtNode->iLayer == pstNode->iLayer + 2) &&
                            (!pstRtNode->bCanQFlag))
                        {
                            bRtCheckFlag = true;
                        }
                    }
                    else if (iNewLayer == pstNode->iLayer + 1)
                    {
                        ++iHighLCnt;
                        /* should decrease others SeSup */
                        if (pstLfNode->iTrussness == iCurK)
                        {
                            if (pstLfNode->iLayer == iNewLayer)
                            {
                                pstLfNode->iSeSup--;
                                DEBUG_PRINTF("BFS decrease left: (%d, %d) k: %d layer: %d seSup: %d\n",
                                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                                             pstLfNode->iTrussness, pstLfNode->iLayer,
                                             pstLfNode->iSeSup);
                                if (!pstLfNode->bCanQFlag)
                                {
                                    bLfCheckFlag = true;
                                }
                            }
                            else if (pstLfNode->iLayer == iNewLayer + 1)
                            {
                                if (!pstLfNode->bCanQFlag)
                                {
                                    bLfCheckFlag = true;
                                }
                            }
                        }
                        if (pstRtNode->iTrussness == iCurK)
                        {
                            if (pstRtNode->iLayer == iNewLayer)
                            {
                                pstRtNode->iSeSup--;
                                DEBUG_PRINTF("BFS decrease right: (%d, %d) k: %d layer: %d seSup: %d\n",
                                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                                             pstRtNode->iTrussness, pstRtNode->iLayer,
                                             pstRtNode->iSeSup);
                                if (!pstRtNode->bCanQFlag)
                                {
                                    bRtCheckFlag = true;
                                }
                            }
                            else if (pstRtNode->iLayer == iNewLayer + 1)
                            {
                                if (!pstRtNode->bCanQFlag)
                                {
                                    bRtCheckFlag = true;
                                }
                            }
                        }
                    }
                    else if (iNewLayer == pstNode->iLayer)
                    {
                        /* should decrease own SeSup */
                        ++iSameLCnt;
                        pstNode->iSeSup++;
                        DEBUG_PRINTF("BFS increase: (%d, %d) k: %d layer: %d seSup: %d\n",
                                     pstNode->paXY.first, pstNode->paXY.second,
                                     pstNode->iTrussness, pstNode->iLayer,
                                     pstNode->iSeSup);
                    }

                    if (bLfCheckFlag)
                    {
                        res = checkRmCdt(mpG, pstLfNode->eid);
                        if (!res)
                        {
                            if (pstNode->iLayer + 1 == pstLfNode->iLayer)
                            {
                                lsCurLayer.push_back(pstLfNode->eid);
                                pstLfNode->bCanQFlag = true;
                            }
                            else if (pstNode->iLayer + 2 == pstLfNode->iLayer)
                            {
                                DEBUG_ASSERT(!pstLfNode->bCanQFlag);
                                DEBUG_ASSERT(!pstLfNode->bUsedCanFlag);
                                myCanQ.insetByOrder(pstLfNode->iLayer, pstLfNode->eid);
                                pstLfNode->bCanQFlag = true;
                                pstLfNode->bUsedCanFlag = true;
                            }
                            else
                            {
                                DEBUG_ASSERT(0);
                            }
                        }
                    }
                    if (bRtCheckFlag)
                    {
                        res = checkRmCdt(mpG, pstRtNode->eid);
                        if (!res)
                        {
                            if (pstNode->iLayer + 1 == pstRtNode->iLayer)
                            {
                                lsCurLayer.push_back(pstRtNode->eid);
                                pstRtNode->bCanQFlag = true;
                            }
                            else if (pstNode->iLayer + 2 == pstRtNode->iLayer)
                            {
                                DEBUG_ASSERT(!pstRtNode->bCanQFlag);
                                DEBUG_ASSERT(!pstRtNode->bUsedCanFlag);
                                myCanQ.insetByOrder(pstRtNode->iLayer, pstRtNode->eid);
                                pstRtNode->bCanQFlag = true;
                                pstRtNode->bUsedCanFlag = true;
                            }
                            else
                            {
                                DEBUG_ASSERT(0);
                            }
                        }
                    }
                }

                if (pstNode->iSeSup != iHighLCnt + iSameLCnt)
                {
                    DEBUG_PRINTF("ERROR get: (%d, %d) k: %d layer: %d seSup: %d high: %d same: %d\n",
                                 pstNode->paXY.first, pstNode->paXY.second,
                                 pstNode->iTrussness, pstNode->iLayer,
                                 pstNode->iSeSup, iHighLCnt, iSameLCnt);
                    DEBUG_ASSERT(0);
                }

                res = checkRmCdt(mpG, pstNode->eid);
                if (res)
                {
                    /* should be removed, set layer back */
                    pstNode->bCanQFlag = false;
                    lsCurLayer.erase(itE++);
                    DEBUG_PRINTF("BFS erase: (%d, %d) k: %d layer: %d seSup: %d\n",
                                 pstNode->paXY.first, pstNode->paXY.second,
                                 pstNode->iTrussness, pstNode->iLayer,
                                 pstNode->iSeSup);
                    /* special case */
                    DEBUG_ASSERT(0 < pstNode->iLayer);
                    continue;
                }
                else
                {
                    pstNode->bCanQFlag = true;
                }
                ++itE;
            }

        }

        iCurLayer--;
    }

    /* restore */
    for (itvE = vVisit.begin(); itvE != vVisit.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);

        DEBUG_PRINTF("RESTORE (%d, %d) k: %d layer: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer);
        pstNode->bVstFlag = false;
        pstNode->bDgdFlag = false;
        DEBUG_ASSERT(!pstNode->bCanQFlag);
        DEBUG_ASSERT(!pstNode->bUsedCanFlag);
    }

    /* handle second case */
    if (!vUgdSeed.empty())
    {
        lcDecomp(mpG, iCurK, iCurMaxLayer, vUgdSeed);
    }
    DEBUG_PRINTF("DEBUG end BFS\n");
    return 0;
}

/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int delPart::degradeForAllK(myG &mpG, vector <int> &vCdtE)
{
    list<int>::iterator itE;
    vector <int>::iterator itvE;
    /* k, eid */
    map<int, vector <int> > mpKLevel;
    map<int, vector <int> >::iterator itmpNode;
    /* eid, none */
    TPST_MAP_BY_EID* pstNode = NULL;
	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;
    vector <int> vUpd;
    vector <int> vSeed;
    int iCurK = 0;
    vector <int> vCurDgd;
    vector <int> vDgdSeed;
    int iSeSup = 0;

    for (itvE = vCdtE.begin(); itvE != vCdtE.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);
        DEBUG_PRINTF("SEED k: %d (%d, %d)\n",
               pstNode->iTrussness, pstNode->paXY.first, pstNode->paXY.second);
        mpKLevel[pstNode->iTrussness].push_back(*itvE);
    }

    /* 3 -> kmax avoid repeat */
    for (itmpNode = mpKLevel.begin(); itmpNode != mpKLevel.end(); ++itmpNode)
    {
        iCurK = itmpNode->first;

        DEBUG_PRINTF("DEGRADE k: %d\n",
               iCurK);
        DEBUG_ASSERT(vCurDgd.empty());
        DEBUG_ASSERT(vDgdSeed.empty());
        deKByBFSEasy(mpG, iCurK, itmpNode->second, vCurDgd, vDgdSeed);
        vUpd.insert(vUpd.end(), vCurDgd.begin(), vCurDgd.end());
        vCurDgd.clear();
        if (!vDgdSeed.empty())
        {
            /* maintain layer */
            vSeed.swap(vDgdSeed);
            deKByBFSEasy(mpG, iCurK - 1, vSeed, vCurDgd, vDgdSeed);
            DEBUG_ASSERT(vCurDgd.empty());
        }
        vSeed.clear();
    }

    /* restore */
    for (itvE = vUpd.begin(); itvE != vUpd.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        debug_assert(NULL != pstNode);
        pstNode->bDgdFlag = false;

        //mpG.checkSeSup(pstNode->eid);
    }

    printf("DEBUG BFS total: %ld degraded: %ld\n",
           g_lBFSCnt, g_lDeCnt);
    return 0;
}
#endif
/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int delPart::updateK(myG &mpG, vector <int> &vCdtE, vector <int> &vChgKE)
{
    vector <int> vStack;
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLf = NULL;
    TPST_MAP_BY_EID* pstRt = NULL;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    /* k, eid */
    map<int, vector<int> > mpOrder;
    map<int, vector<int> >::reverse_iterator ritE;

    DEBUG_ASSERT(vChgKE.empty());
    for (int iEid : vCdtE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        DEBUG_ASSERT(pstNode->bInit);
        if (pstNode->iKSup + 2 < pstNode->iTrussness)
        {
            mpOrder[pstNode->iTrussness].push_back(iEid);
            pstNode->bCanQFlag = true;
        }
        else
        {
            DEBUG_ASSERT(0);
        }
    }
    for (ritE = mpOrder.rbegin(); ritE != mpOrder.rend(); ++ritE)
    {
        vStack.insert(vStack.end(), ritE->second.begin(), ritE->second.end());
    }
    while (!vStack.empty())
    {
        int iCurEid = vStack.back();
        vStack.pop_back();

        pstNode = mpG.findNode(iCurEid);
        DEBUG_ASSERT(NULL != pstNode);
        DEBUG_ASSERT(pstNode->iKSup + 2 < pstNode->iTrussness);
        DEBUG_ASSERT(pstNode->bInit);
        pstNode->bCanQFlag = false;

        vector<int> vLfE;
        vector<int> vRtE;
        mpG.findNeb(pstNode->eid, vLfE, vRtE);

        itLfE = vLfE.begin();
        itRtE = vRtE.begin();
        for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLf = mpG.findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLf);
            pstRt = mpG.findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRt);

            if (!pstLf->bInit)
            {
                mpG.init(pstLf->eid);
            }
            if (!pstRt->bInit)
            {
                mpG.init(pstRt->eid);
            }

            int iMinT = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
            iMinT = COMMON_MIN(iMinT, pstNode->iTrussness);

            if (iMinT == pstNode->iTrussness)
            {
                /* triangle will decrease 1 */
                if (iMinT == pstLf->iTrussness)
                {
                    pstLf->iKSup--;
                    if (!pstLf->bCanQFlag)
                    {
                        if (pstLf->iKSup + 2 < pstLf->iTrussness)
                        {
                            vStack.push_back(pstLf->eid);
                            pstLf->bCanQFlag = true;
                        }
                    }
                }
                if (iMinT == pstRt->iTrussness)
                {
                    pstRt->iKSup--;
                    if (!pstRt->bCanQFlag)
                    {
                        if (pstRt->iKSup + 2 < pstRt->iTrussness)
                        {
                            vStack.push_back(pstRt->eid);
                            pstRt->bCanQFlag = true;
                        }
                    }
                }
            }
            else if (iMinT == pstNode->iTrussness - 1)
            {
                /* increase self support */
                pstNode->iKSup++;
            }
            else
            {
                /* no change */
                continue;
            }
        }

        /* decrease self */
        pstNode->iTrussness--;
        pstNode->iOldL = pstNode->iLayer;
        pstNode->iLayer = INT_MAX;
        pstNode->iSeSup = 0;
        pstNode->iKMSup = 0;
        DEBUG_ASSERT(pstNode->iKSup + 2 >= pstNode->iTrussness);
        DEBUG_ASSERT(!pstNode->bDgdFlag);
        vChgKE.push_back(pstNode->eid);
        pstNode->bDgdFlag = true;
        pstNode->bCanQFlag = true;
    }
    return 0;
}

/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int delPart::updNeibL(myG &mpG, int iEid, vector <int> &vCdtChgLE)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLf = NULL;
    TPST_MAP_BY_EID* pstRt = NULL;
    vector<int> vLfE;
    vector<int> vRtE;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);

    mpG.findNeb(pstNode->eid, vLfE, vRtE);

    itLfE = vLfE.begin();
    itRtE = vRtE.begin();
    for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLf = mpG.findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLf);
        pstRt = mpG.findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRt);
        DEBUG_ASSERT(pstLf->bInit);
        DEBUG_ASSERT(pstRt->bInit);

        /*DEBUG_PRINTF("UPDATE_L (%d, %d) k: %d L: %d oldL: %d QFlag: %d left: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d QFlag: %d right: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d QFlag: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer, pstNode->iOldL, pstNode->bCanQFlag,
                         pstLf->paXY.first, pstLf->paXY.second,
                         pstLf->iTrussness, pstLf->iLayer, pstLf->iSeSup,
                         pstLf->iKSup, pstLf->iKMSup, pstLf->bCanQFlag,
                         pstRt->paXY.first, pstRt->paXY.second,
                         pstRt->iTrussness, pstRt->iLayer, pstRt->iSeSup,
                         pstRt->iKSup, pstRt->iKMSup, pstRt->bCanQFlag);*/

        int iMinT = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
        if (iMinT == pstNode->iTrussness)
        {
            int iMinL = INT_MAX;
            if (iMinT == pstLf->iTrussness)
            {
                iMinL = COMMON_MIN(iMinL, pstLf->iLayer);
            }
            if (iMinT == pstRt->iTrussness)
            {
                iMinL = COMMON_MIN(iMinL, pstRt->iLayer);
            }
#if 0
            /* case: one edge in queue */
            if ((pstLf->bCanQFlag && (iMinT == pstLf->iTrussness)) && (!pstRt->bCanQFlag))
            {
                /* seSup */
                int iOldMinL = pstNode->iOldL;
                iOldMinL = COMMON_MIN(iOldMinL, pstLf->iOldL);
                int iNewMinL = pstNode->iLayer;
                iNewMinL = COMMON_MIN(iNewMinL, pstLf->iLayer);
                if ((iOldMinL >= pstRt->iLayer) && (iNewMinL < pstRt->iLayer))
                {
                    pstRt->iSeSup--;
                }
                /* KMSup */
                if ((iOldMinL >= pstRt->iLayer - 1) && (iNewMinL < pstRt->iLayer - 1))
                {
                    pstRt->iKMSup--;
                    if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                    {
                        vCdtChgLE.push_back(pstRt->eid);
                        pstRt->bCanQFlag = true;
                    }
                }
            }
            else if ((pstRt->bCanQFlag && (iMinT == pstRt->iTrussness))&& (!pstLf->bCanQFlag))
            {
                /* seSup */
                int iOldMinL = pstNode->iOldL;
                iOldMinL = COMMON_MIN(iOldMinL, pstRt->iOldL);
                int iNewMinL = pstNode->iLayer;
                iNewMinL = COMMON_MIN(iNewMinL, pstRt->iLayer);
                if ((iOldMinL >= pstLf->iLayer) && (iNewMinL < pstLf->iLayer))
                {
                    pstLf->iSeSup--;
                }
                /* KMSup */
                if ((iOldMinL >= pstLf->iLayer - 1) && (iNewMinL < pstLf->iLayer - 1))
                {
                    pstLf->iKMSup--;
                    if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                    {
                        vCdtChgLE.push_back(pstLf->eid);
                        pstLf->bCanQFlag = true;
                    }
                }
            }
            else if ((pstLf->bCanQFlag && (iMinT == pstLf->iTrussness)) &&
                     (pstRt->bCanQFlag && (iMinT == pstRt->iTrussness)))
            {
                continue;
            }
            else
            {
                /* seSup */
                if ((iMinL <= pstNode->iOldL) && (iMinL > pstNode->iLayer))
                {
                    if ((iMinT == pstLf->iTrussness) && (iMinL == pstLf->iLayer))
                    {
                        pstLf->iSeSup--;
                    }
                    if ((iMinT == pstRt->iTrussness) && (iMinL == pstRt->iLayer))
                    {
                        pstRt->iSeSup--;
                    }
                }
                /* KMSup */
                if ((iMinL <= pstNode->iOldL + 1) && (iMinL > pstNode->iLayer))
                {
                    if (iMinT == pstLf->iTrussness)
                    {
                        if (iMinL == pstLf->iLayer - 1)
                        {
                            pstLf->iKMSup--;
                            if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                            {
                                vCdtChgLE.push_back(pstLf->eid);
                                pstLf->bCanQFlag = true;
                            }
                        }
                        else if (iMinL == pstLf->iLayer)
                        {
                            if (pstNode->iLayer < pstLf->iLayer - 1)
                            {
                                pstLf->iKMSup--;
                                if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                                {
                                    vCdtChgLE.push_back(pstLf->eid);
                                    pstLf->bCanQFlag = true;
                                }
                            }
                        }
                    }
                    if (iMinT == pstRt->iTrussness)
                    {
                        if (iMinL == pstRt->iLayer - 1)
                        {
                            pstRt->iKMSup--;
                            if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                            {
                                vCdtChgLE.push_back(pstRt->eid);
                                pstRt->bCanQFlag = true;
                            }
                        }
                        else if (iMinL == pstRt->iLayer)
                        {
                            if (pstNode->iLayer < pstRt->iLayer - 1)
                            {
                                pstRt->iKMSup--;
                                if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                                {
                                    vCdtChgLE.push_back(pstRt->eid);
                                    pstRt->bCanQFlag = true;
                                }
                            }
                        }
                    }
                }
            }
#endif
            /* seSup */
            if ((iMinL <= pstNode->iOldL) && (iMinL > pstNode->iLayer))
            {
                if ((iMinT == pstLf->iTrussness) && (iMinL == pstLf->iLayer))
                {
                    pstLf->iSeSup--;
                }
                if ((iMinT == pstRt->iTrussness) && (iMinL == pstRt->iLayer))
                {
                    pstRt->iSeSup--;
                }
            }
            /* KMSup */
            int iOldTriMinL = COMMON_MIN(iMinL, pstNode->iOldL);
            int iNewTriMinL = COMMON_MIN(iMinL, pstNode->iLayer);
            if (iMinT == pstLf->iTrussness)
            {
                if ((pstLf->iLayer <= iOldTriMinL + 1) && (pstLf->iLayer > iNewTriMinL + 1))
                {
                    pstLf->iKMSup--;
                    if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                    {
                        vCdtChgLE.push_back(pstLf->eid);
                        pstLf->bCanQFlag = true;
                    }
                }
            }
            if (iMinT == pstRt->iTrussness)
            {
                if ((pstRt->iLayer <= iOldTriMinL + 1) && (pstRt->iLayer > iNewTriMinL + 1))
                {
                    pstRt->iKMSup--;
                    if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                    {
                        vCdtChgLE.push_back(pstRt->eid);
                        pstRt->bCanQFlag = true;
                    }
                }
            }
        }

        /*DEBUG_PRINTF("UPDATE_L finish (%d, %d) k: %d L: %d left: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d right: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstLf->paXY.first, pstLf->paXY.second,
                         pstLf->iTrussness, pstLf->iLayer, pstLf->iSeSup,
                         pstLf->iKSup, pstLf->iKMSup,
                         pstRt->paXY.first, pstRt->paXY.second,
                         pstRt->iTrussness, pstRt->iLayer, pstRt->iSeSup,
                         pstRt->iKSup, pstRt->iKMSup);*/
    }
    return 0;
}
/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int delPart::recalL(myG &mpG, int iEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLf = NULL;
    TPST_MAP_BY_EID* pstRt = NULL;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);

    /* build table */
    vector<int> vLfE;
    vector<int> vRtE;
    mpG.findNeb(pstNode->eid, vLfE, vRtE);

    /* estimate L */
    /* L, cnt */
    map <int, int > mpTriCnt;
    map <int, int >::reverse_iterator ritC;
    itLfE = vLfE.begin();
    itRtE = vRtE.begin();
    for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLf = mpG.findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLf);
        pstRt = mpG.findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRt);

        if (!pstLf->bInit)
        {
            mpG.init(pstLf->eid);
        }
        if (!pstRt->bInit)
        {
            mpG.init(pstRt->eid);
        }

        /*DEBUG_PRINTF("CAL_L (%d, %d) k: %d L: %d oldL: %d left: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d QFlag: %d right: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d QFlag: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer, pstNode->iOldL,
                         pstLf->paXY.first, pstLf->paXY.second,
                         pstLf->iTrussness, pstLf->iLayer, pstLf->iSeSup,
                         pstLf->iKSup, pstLf->iKMSup, pstLf->bCanQFlag,
                         pstRt->paXY.first, pstRt->paXY.second,
                         pstRt->iTrussness, pstRt->iLayer, pstRt->iSeSup,
                         pstRt->iKSup, pstRt->iKMSup, pstRt->bCanQFlag);*/

        int iMinT = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
        iMinT = COMMON_MIN(iMinT, pstNode->iTrussness);
        if (iMinT == pstNode->iTrussness)
        {
            int iMinL = pstNode->iLayer;
            if (iMinT == pstLf->iTrussness)
            {
                iMinL = COMMON_MIN(iMinL, pstLf->iLayer);
            }
            if (iMinT == pstRt->iTrussness)
            {
                iMinL = COMMON_MIN(iMinL, pstRt->iLayer);
            }
            if (mpTriCnt.find(iMinL) == mpTriCnt.end())
            {
                mpTriCnt[iMinL] = 1;
            }
            else
            {
                mpTriCnt[iMinL]++;
            }
        }
    }
    int iTriCnt = 0;
    for (ritC = mpTriCnt.rbegin(); ritC != mpTriCnt.rend(); ++ritC)
    {
        if (iTriCnt + ritC->second + 2 > pstNode->iTrussness)
        {
            /* find */
            pstNode->iLayer = ritC->first + 1;
            pstNode->iSeSup = iTriCnt;
            pstNode->iKMSup = iTriCnt + ritC->second;
            break;
        }
        iTriCnt += ritC->second;
    }
    if (ritC == mpTriCnt.rend())
    {
        /* not full */
        DEBUG_ASSERT(iTriCnt + 2 == pstNode->iTrussness);
        pstNode->iLayer = 1;
        pstNode->iSeSup = iTriCnt;
        pstNode->iKMSup = iTriCnt;
    }

    /*DEBUG_PRINTF("CAL_L (%d, %d) old: %d new: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iOldL, pstNode->iLayer);*/
    return 0;
}
/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int delPart::recalL(myG &mpG, vector <int> &vChgKE, vector <int> &vCdtChgLE)
{
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLf = NULL;
    TPST_MAP_BY_EID* pstRt = NULL;

    for (int iEid : vChgKE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        pstNode->bCanQFlag = false;

        /*DEBUG_PRINTF("DEBUG BFS change k (%d, %d) k: %d Layer: %d SeSup: %d degrade: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->bDgdFlag);*/

        recalL(mpG, iEid);
        /* update neighbor */
        vector<int> vLfE;
        vector<int> vRtE;

        mpG.findNeb(pstNode->eid, vLfE, vRtE);

        itLfE = vLfE.begin();
        itRtE = vRtE.begin();
        for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLf = mpG.findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLf);
            pstRt = mpG.findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRt);

            DEBUG_ASSERT(pstLf->bInit);
            DEBUG_ASSERT(pstRt->bInit);

            int iMinT = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
            int iMinL = INT_MAX;
            if (iMinT == pstLf->iTrussness)
            {
                iMinL = COMMON_MIN(iMinL, pstLf->iLayer);
            }
            if (iMinT == pstRt->iTrussness)
            {
                iMinL = COMMON_MIN(iMinL, pstRt->iLayer);
            }
            if (iMinT == pstNode->iTrussness + 1)
            {
                /* seSup */
                if (iMinL <= pstNode->iOldL)
                {
                    if ((iMinT == pstLf->iTrussness) && (iMinL == pstLf->iLayer))
                    {
                        pstLf->iSeSup--;
                    }
                    if ((iMinT == pstRt->iTrussness) && (iMinL == pstRt->iLayer))
                    {
                        pstRt->iSeSup--;
                    }
                }
                /* KMSup */
                iMinL = COMMON_MIN(iMinL, pstNode->iOldL);
                if (iMinT == pstLf->iTrussness)
                {
                    if (iMinL >= pstLf->iLayer - 1)
                    {
                        pstLf->iKMSup--;
                        if (1 == pstLf->iLayer)
                        {
                            DEBUG_ASSERT(pstLf->iKMSup + 2 >= pstLf->iTrussness);
                        }
                        else
                        {
                            if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag) && (!pstLf->bDgdFlag))
                            {
                                vCdtChgLE.push_back(pstLf->eid);
                                pstLf->bCanQFlag = true;
                            }
                        }
                    }
                }
                if (iMinT == pstRt->iTrussness)
                {
                    if (iMinL >= pstRt->iLayer - 1)
                    {
                        pstRt->iKMSup--;
                        if (1 == pstRt->iLayer)
                        {
                            DEBUG_ASSERT(pstRt->iKMSup + 2 >= pstRt->iTrussness);
                        }
                        else
                        {
                            if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag) && (!pstRt->bDgdFlag))
                            {
                                vCdtChgLE.push_back(pstRt->eid);
                                pstRt->bCanQFlag = true;
                            }
                        }
                    }
                }
            }
            else if (iMinT == pstNode->iTrussness)
            {
                /* two edges both decrease k */
                if (INT_MAX == iMinL)
                {
                    /* both k+ */
                    if (pstLf->iTrussness == pstRt->iTrussness)
                    {
                        /* no need to process */
                        continue;
                    }
                    if ((iMinT + 1 == pstLf->iTrussness) && (!pstLf->bDgdFlag))
                    {
                        int iTpMinL = COMMON_MIN(pstNode->iOldL, pstRt->iOldL);
                        if (iTpMinL >= pstLf->iLayer)
                        {
                            pstLf->iSeSup--;
                        }
                        if (iTpMinL >= pstLf->iLayer - 1)
                        {
                            pstLf->iKMSup--;
                            if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                            {
                                vCdtChgLE.push_back(pstLf->eid);
                                pstLf->bCanQFlag = true;
                            }
                        }
                    }
                    if ((iMinT + 1 == pstRt->iTrussness) && (!pstRt->bDgdFlag))
                    {
                        int iTpMinL = COMMON_MIN(pstNode->iOldL, pstLf->iOldL);
                        if (iTpMinL >= pstRt->iLayer)
                        {
                            pstRt->iSeSup--;
                        }
                        if (iTpMinL >= pstRt->iLayer - 1)
                        {
                            pstRt->iKMSup--;
                            if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                            {
                                vCdtChgLE.push_back(pstRt->eid);
                                pstRt->bCanQFlag = true;
                            }
                        }
                    }
                }
                else
                {
                    /* seSup */
                    if (iMinL > pstNode->iLayer)
                    {
                        if ((iMinT == pstLf->iTrussness) && (iMinL == pstLf->iLayer))
                        {
                            pstLf->iSeSup--;
                        }
                        if ((iMinT == pstRt->iTrussness) && (iMinL == pstRt->iLayer))
                        {
                            pstRt->iSeSup--;
                        }
                    }
                    /* KMSup */
                    if (iMinL > pstNode->iLayer)
                    {
                        if (iMinT == pstLf->iTrussness)
                        {
                            if (iMinL == pstLf->iLayer - 1)
                            {
                                pstLf->iKMSup--;
                                if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                                {
                                    vCdtChgLE.push_back(pstLf->eid);
                                    pstLf->bCanQFlag = true;
                                }
                            }
                            else if (iMinL == pstLf->iLayer)
                            {
                                if (pstNode->iLayer < pstLf->iLayer - 1)
                                {
                                    pstLf->iKMSup--;
                                    if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (!pstLf->bCanQFlag))
                                    {
                                        vCdtChgLE.push_back(pstLf->eid);
                                        pstLf->bCanQFlag = true;
                                    }
                                }
                            }
                        }
                        if (iMinT == pstRt->iTrussness)
                        {
                            if (iMinL == pstRt->iLayer - 1)
                            {
                                pstRt->iKMSup--;
                                if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                                {
                                    vCdtChgLE.push_back(pstRt->eid);
                                    pstRt->bCanQFlag = true;
                                }
                            }
                            else if (iMinL == pstRt->iLayer)
                            {
                                if (pstNode->iLayer < pstRt->iLayer - 1)
                                {
                                    pstRt->iKMSup--;
                                    if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (!pstRt->bCanQFlag))
                                    {
                                        vCdtChgLE.push_back(pstRt->eid);
                                        pstRt->bCanQFlag = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            /*DEBUG_PRINTF("CAL_L finish (%d, %d) k: %d L: %d left: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d right: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d\n",
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iTrussness, pstNode->iLayer,
                             pstLf->paXY.first, pstLf->paXY.second,
                             pstLf->iTrussness, pstLf->iLayer, pstLf->iSeSup,
                             pstLf->iKSup, pstLf->iKMSup,
                             pstRt->paXY.first, pstRt->paXY.second,
                             pstRt->iTrussness, pstRt->iLayer, pstRt->iSeSup,
                             pstRt->iKSup, pstRt->iKMSup);*/
        }
    }
    return 0;
}

/*****************
input:
        int iType
description:
        upgrade and save trussness
******************/
int delPart::updateL(myG &mpG, vector <int> &vCdtE, vector <int> &vChgLE)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    /* <k, L>, eid */
    map <pair<int, int>, vector<int> > mpPool;
    for (int iEid : vCdtE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        /*DEBUG_PRINTF("UPDATE_L init (%d, %d) k: %d layer: %d KMSup: %d\n",
               pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness,
               pstNode->iLayer, pstNode->iKMSup);*/

        if (3 > pstNode->iTrussness)
        {
            /* no need to update */
            continue;
        }
        DEBUG_ASSERT(pstNode->bInit);

        pstNode->iOldL = pstNode->iLayer;
        pstNode->bCanQFlag = false;
        recalL(mpG, iEid);
        //DEBUG_PRINTF("UPDATE_L get new layer: %d\n", pstNode->iLayer);
        DEBUG_ASSERT(pstNode->iLayer < pstNode->iOldL);
        vChgLE.push_back(iEid);

        /* update neighbors */
        vector<int> vCdtChgLE;
        updNeibL(mpG, iEid, vCdtChgLE);

        for (int iChgE : vCdtChgLE)
        {
            TPST_MAP_BY_EID* pstTpNode = mpG.findNode(iChgE);
            DEBUG_ASSERT(NULL != pstTpNode);
            DEBUG_ASSERT(pstTpNode->bInit);
            DEBUG_ASSERT((pstTpNode->iKMSup + 2 <= pstTpNode->iTrussness) && (1 < pstTpNode->iLayer));
            pstTpNode->bCanQFlag = true;
            /*DEBUG_PRINTF("UPDATE_L set flag (%d, %d)\n",
                                 pstTpNode->paXY.first, pstTpNode->paXY.second);*/
            mpPool[pair<int, int>(pstTpNode->iTrussness, pstTpNode->iLayer)].push_back(iChgE);
            /*DEBUG_PRINTF("UPDATE_L neighbor (%d, %d) k: %d layer: %d KMSup: %d\n",
                   pstTpNode->paXY.first, pstTpNode->paXY.second, pstTpNode->iTrussness,
                   pstTpNode->iLayer, pstTpNode->iKMSup);*/
        }
    }

    while (!mpPool.empty())
    {
        vector<int> vDesE;
        vDesE.swap(mpPool.begin()->second);
        mpPool.erase(mpPool.begin());

        for (int iEid : vDesE)
        {
            pstNode = mpG.findNode(iEid);
            DEBUG_ASSERT(NULL != pstNode);
            /*DEBUG_PRINTF("UPDATE_L BFS (%d, %d) k: %d layer: %d KMSup: %d\n",
                   pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness,
                   pstNode->iLayer, pstNode->iKMSup);*/

            pstNode->iOldL = pstNode->iLayer;
            pstNode->bCanQFlag = false;
            recalL(mpG, iEid);
            //DEBUG_PRINTF("UPDATE_L get new layer: %d\n", pstNode->iLayer);
            if (pstNode->iLayer >= pstNode->iOldL)
            {
                printf("ERROR (%d, %d) k: %d new layer: %d old layer: %d\n",
                       pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness,
                       pstNode->iLayer, pstNode->iOldL);
                DEBUG_ASSERT(0);
            }
            vChgLE.push_back(iEid);

            /* update neighbors */
            vector<int> vCdtChgLE;
            updNeibL(mpG, iEid, vCdtChgLE);
            /* recalculate layer */
            for (int iChgE : vCdtChgLE)
            {
                TPST_MAP_BY_EID* pstTpNode = mpG.findNode(iChgE);
                DEBUG_ASSERT(NULL != pstTpNode);
                DEBUG_ASSERT(pstTpNode->bInit);
                DEBUG_ASSERT((pstTpNode->iKMSup + 2 <= pstTpNode->iTrussness) && (1 < pstTpNode->iLayer));
                pstTpNode->bCanQFlag = true;

                mpPool[pair<int, int>(pstTpNode->iTrussness, pstTpNode->iLayer)].push_back(iChgE);
                /*DEBUG_PRINTF("UPDATE_L neighbor (%d, %d) k: %d layer: %d KMSup: %d\n",
                       pstTpNode->paXY.first, pstTpNode->paXY.second, pstTpNode->iTrussness,
                       pstTpNode->iLayer, pstTpNode->iKMSup);*/
            }
        }
    }
    return 0;
}

/*****************
input:
        myG &mpG
        int iEid
        int iLfEid
        int iRtEid
        bool *pbLfSup
        bool *pbRtSup
        bool *pbLfCheck
        bool *pbRtCheck
description:
        whether iEid removed affect other edges
******************/
int delPart::triOrder(myG &mpG, int iEid, int iLfEid, int iRtEid, bool *pbLfSup, bool *pbRtSup, bool *pbLfCheck, bool *pbRtCheck)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    int iCurK = 0;
    int iTriT = 0;
    int iTriL = 0;

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    pstLfNode = mpG.findNode(iLfEid);
    DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = mpG.findNode(iRtEid);
    DEBUG_ASSERT(NULL != pstRtNode);

    *pbLfSup = false;
    *pbRtSup = false;
    *pbLfCheck = false;
    *pbRtCheck = false;

    iCurK = pstNode->iTrussness;

    iTriT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
    if (iTriT == iCurK)
    {
        iTriL = COMMON_TRG_MIN(pstNode->iLayer + 1,
                               pstLfNode->iLayer,
                               (pstLfNode->iTrussness > iCurK),
                               pstRtNode->iLayer,
                               (pstRtNode->iTrussness > iCurK));
        if (iTriL > pstNode->iLayer)
        {
            if ((pstLfNode->iTrussness == iCurK) &&
                (pstLfNode->iLayer == pstNode->iLayer + 1))
            {
                *pbLfCheck = true;
            }
            if ((pstRtNode->iTrussness == iCurK) &&
                (pstRtNode->iLayer == pstNode->iLayer + 1))
            {
                *pbRtCheck = true;
            }
        }
        else if (iTriL < pstNode->iLayer)
        {
            if ((pstLfNode->iTrussness == iCurK) &&
                (pstLfNode->iLayer == iTriL))
            {
                *pbLfSup = true;
                if ((pstRtNode->iTrussness == iCurK) &&
                    (pstRtNode->iLayer == iTriL + 1))
                {
                    *pbRtCheck = true;
                }
            }
            if ((pstRtNode->iTrussness == iCurK) &&
                (pstRtNode->iLayer == iTriL))
            {
                *pbRtSup = true;
                if ((pstLfNode->iTrussness == iCurK) &&
                    (pstLfNode->iLayer == iTriL + 1))
                {
                    *pbLfCheck = true;
                }
            }
        }
        else
        {
            if ((pstLfNode->iTrussness == iCurK) &&
                (pstLfNode->iLayer == iTriL))
            {
                *pbLfSup = true;
            }
            if ((pstRtNode->iTrussness == iCurK) &&
                (pstRtNode->iLayer == iTriL))
            {
                *pbRtSup = true;
            }
            if ((pstLfNode->iTrussness == iCurK) &&
                (pstLfNode->iLayer == pstNode->iLayer + 1))
            {
                *pbLfCheck = true;
            }
            if ((pstRtNode->iTrussness == iCurK) &&
                (pstRtNode->iLayer == pstNode->iLayer + 1))
            {
                *pbRtCheck = true;
            }
        }

    }
    else if (iTriT < iCurK)
    {
        if (pstLfNode->iTrussness == pstRtNode->iTrussness)
        {
            iTriL = COMMON_MIN(pstLfNode->iLayer, pstRtNode->iLayer);
            if (pstLfNode->iLayer == iTriL)
            {
                *pbLfSup = true;
                if (pstRtNode->iLayer == iTriL + 1)
                {
                    *pbRtCheck = true;
                }
            }
            if (pstRtNode->iLayer == iTriL)
            {
                *pbRtSup = true;
                if (pstLfNode->iLayer == iTriL + 1)
                {
                    *pbLfCheck = true;
                }
            }
        }
        else if (iTriT == pstRtNode->iTrussness)
        {
            *pbRtSup = true;
        }
        else if (iTriT == pstLfNode->iTrussness)
        {
            *pbLfSup = true;
        }
        else
        {
            DEBUG_ASSERT(0);
        }
    }
    else
    {
        /* nothing */
        return 0;
    }

    return 0;
}
#if 0
/*****************
input:
        myG &mpG
        int iEid
        int iLfEid
        int iRtEid
description:
        2: self > left or right
        1: self = max(left, right)
        0: self in (left, right)
        -1: self = min(left, right)
        -2: self < left and right
******************/
int delPart::checkOrder(myG &mpG, int iEid, int iLfEid, int iRtEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    int iCurK = 0;
    int iCurL = 0;
    int iRes = 0;

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    pstLfNode = mpG.findNode(iLfEid);
    DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = mpG.findNode(iRtEid);
    DEBUG_ASSERT(NULL != pstRtNode);

    iCurK = pstNode->iTrussness;
    iCurL = pstNode->iLayer;

    if (iCurK > pstLfNode->iTrussness)
    {
        if (iCurK > pstRtNode->iTrussness)
        {
            iRes = 2;
        }
        else if (iCurK < pstRtNode->iTrussness)
        {
            iRes = 0;
        }
        else
        {
            if (iCurL > pstRtNode->iLayer)
            {
                iRes = 2;
            }
            else if (iCurL < pstRtNode->iLayer)
            {
                iRes = 0;
            }
            else
            {
                iRes = 1;
            }
        }
    }
    else if (iCurK < pstLfNode->iTrussness)
    {
        if (iCurK > pstRtNode->iTrussness)
        {
            iRes = 0;
        }
        else if (iCurK < pstRtNode->iTrussness)
        {
            iRes = -2;
        }
        else
        {
            if (iCurL > pstRtNode->iLayer)
            {
                iRes = 0;
            }
            else if (iCurL < pstRtNode->iLayer)
            {
                iRes = -2;
            }
            else
            {
                iRes = -1;
            }
        }
    }
    else
    {
        if (iCurK > pstRtNode->iTrussness)
        {
            if (iCurL > pstLfNode->iLayer)
            {
                iRes = 2;
            }
            else if (iCurL < pstLfNode->iLayer)
            {
                iRes = 0;
            }
            else
            {
                iRes = 1;
            }
        }
        else if (iCurK < pstRtNode->iTrussness)
        {
            if (iCurL > pstLfNode->iLayer)
            {
                iRes = 0;
            }
            else if (iCurL < pstLfNode->iLayer)
            {
                iRes = -2;
            }
            else
            {
                iRes = -1;
            }
        }
        else
        {
            /* all are the same k */
            if (iCurL > pstLfNode->iLayer)
            {
                if (iCurL > pstRtNode->iLayer)
                {
                    iRes = 2;
                }
                else if (iCurL < pstRtNode->iLayer)
                {
                    iRes = 0;
                }
                else
                {
                    iRes = 1;
                }
            }
            else if (iCurL < pstLfNode->iLayer)
            {
                if (iCurL > pstRtNode->iLayer)
                {
                    iRes = 0;
                }
                else if (iCurL < pstRtNode->iLayer)
                {
                    iRes = -2;
                }
                else
                {
                    iRes = -1;
                }
            }
            else
            {
                if (iCurL > pstRtNode->iLayer)
                {
                    iRes = 1;
                }
                else if (iCurL < pstRtNode->iLayer)
                {
                    iRes = -1;
                }
                else
                {
                    iRes = 0;
                }
            }
        }
    }

    return iRes;
}

/*****************
input:
        myG &mpG
        int iEid
        int iLfEid
        int iRtEid
description:
        true: sup change
        false: sup not change
******************/
bool delPart::triRmCheck(myG &mpG, int iEid, int iLfEid, int iRtEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    int iCurK = 0;
    int iCurL = 0;
    bool bRes = 0;

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    pstLfNode = mpG.findNode(iLfEid);
    DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = mpG.findNode(iRtEid);
    DEBUG_ASSERT(NULL != pstRtNode);

    iCurK = pstNode->iTrussness;
    iCurL = pstNode->iLayer;

    if (iCurK > pstLfNode->iTrussness)
    {
        bRes = false;
    }
    else if (iCurK < pstLfNode->iTrussness)
    {
        if (iCurK > pstRtNode->iTrussness)
        {
            bRes = false;
        }
        else if (iCurK < pstRtNode->iTrussness)
        {
            bRes = true;
        }
        else
        {
            if (iCurL > pstRtNode->iLayer + 1)
            {
                bRes = false;
            }
            else
            {
                bRes = true;
            }
        }
    }
    else
    {
        if (iCurK > pstRtNode->iTrussness)
        {
            bRes = false;
        }
        else if (iCurK < pstRtNode->iTrussness)
        {
            if (iCurL > pstLfNode->iLayer + 1)
            {
                bRes = false;
            }
            else
            {
                bRes = true;
            }
        }
        else
        {
            /* all are the same k */
            if (iCurL > pstLfNode->iLayer + 1)
            {
                bRes = false;
            }
            else
            {
                if (iCurL > pstRtNode->iLayer + 1)
                {
                    bRes = false;
                }
                else
                {
                    bRes = true;
                }
            }
        }
    }

    return bRes;
}
#endif
/*****************
input:
        myG &mpG
        int x
        int y
description:
        simply add
******************/
int delPart::simpleRm(myG &mpG, int iEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    TPST_MAP_BY_EID* pstTpNode = NULL;
    TPST_MAP_BY_EID* pstTpONode = NULL;
    int iTpEid = 0;
    int iTpOEid = 0;
    int iMinT = 0;
    bool bLfSup = false;
    bool bRtSup = false;
    bool bLfCheck = false;
    bool bRtCheck = false;

	/*struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;*/

    DEBUG_ASSERT(0 != iEid);
    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    if (!pstNode->bInit)
    {
        mpG.init(pstNode->eid);
    }
    //pstNode->bRmFlag = true;

    vector<int> vLfE;
    vector<int> vRtE;
    mpG.findNeb(pstNode->eid, vLfE, vRtE);

    /*gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;*/

    itLfE = vLfE.begin();
    itRtE = vRtE.begin();
    for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
    {
        iTpEid = *itLfE;
        iTpOEid = *itRtE;
        pstTpNode = mpG.findNode(iTpEid);
        DEBUG_ASSERT(NULL != pstTpNode);
        pstTpONode = mpG.findNode(iTpOEid);
        DEBUG_ASSERT(NULL != pstTpONode);

        /*DEBUG_PRINTF("SIMPLE_RM before (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstTpNode->paXY.first, pstTpNode->paXY.second,
                     pstTpNode->iTrussness,
                     pstTpNode->iLayer, pstTpNode->iSeSup,
                     pstTpNode->iKSup, pstTpNode->iKMSup,
                     pstTpNode->mpLfE.size());

        DEBUG_PRINTF("SIMPLE_RM before (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstTpONode->paXY.first, pstTpONode->paXY.second,
                     pstTpONode->iTrussness,
                     pstTpONode->iLayer, pstTpONode->iSeSup,
                     pstTpONode->iKSup, pstTpONode->iKMSup,
                     pstTpONode->mpLfE.size());*/
        if (!pstTpNode->bInit)
        {
            mpG.init(pstTpNode->eid);
        }
        if (!pstTpONode->bInit)
        {
            mpG.init(pstTpONode->eid);
        }

        iMinT = COMMON_MIN(pstTpNode->iTrussness, pstTpONode->iTrussness);
        iMinT = COMMON_MIN(iMinT, pstNode->iTrussness);
        if (iMinT == pstTpNode->iTrussness)
        {
            pstTpNode->iKSup--;
        }
        if (iMinT == pstTpONode->iTrussness)
        {
            pstTpONode->iKSup--;
        }

        triOrder(mpG, iEid, *itLfE, *itRtE, &bLfSup, &bRtSup, &bLfCheck, &bRtCheck);
        if (bLfSup)
        {
            pstTpNode->iSeSup--;
            pstTpNode->iKMSup--;
        }
        if (bRtSup)
        {
            pstTpONode->iSeSup--;
            pstTpONode->iKMSup--;
        }
        if (bLfCheck)
        {
            pstTpNode->iKMSup--;
        }
        if (bRtCheck)
        {
            pstTpONode->iKMSup--;
        }

        pstTpNode->mpLfE.erase(iEid);
        pstTpNode->mpLfE.erase(iTpOEid);

        //pstTpNode->mpRtE.erase(iEid);
        //pstTpNode->mpRtE.erase(iTpOEid);

        pstTpONode->mpLfE.erase(iEid);
        pstTpONode->mpLfE.erase(iTpEid);

        //pstTpONode->mpRtE.erase(iEid);
        //pstTpONode->mpRtE.erase(iTpEid);

        /*DEBUG_PRINTF("SIMPLE_RM after (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstTpNode->paXY.first, pstTpNode->paXY.second,
                     pstTpNode->iTrussness,
                     pstTpNode->iLayer, pstTpNode->iSeSup,
                     pstTpNode->iKSup, pstTpNode->iKMSup,
                     pstTpNode->mpLfE.size());
        DEBUG_PRINTF("SIMPLE_RM after (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstTpONode->paXY.first, pstTpONode->paXY.second,
                     pstTpONode->iTrussness,
                     pstTpONode->iLayer, pstTpONode->iSeSup,
                     pstTpONode->iKSup, pstTpONode->iKMSup,
                     pstTpONode->mpLfE.size());*/

    }
    DEBUG_ASSERT(itRtE == vRtE.end());

    /*gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmCoreTime += lCurTime - lStartTime;*/

    pstNode->mpLfE.clear();
    //pstNode->mpRtE.clear();

    /*gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;*/
    mpG.rm(iEid);
    /*gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmBasicTime += lCurTime - lStartTime;*/

    return 0;
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
int delPart::localDec(myG &mpG, int iNode, map<pair<int, int>, int> &mpLocalG, unordered_map<int, TPSTMP_P_INFO> &mpLocalP, map<pair<int, int>, list<pair<int, int> > > &mpKPLs)
{
    /* k <sesup, pid> */
    map<int, list<pair<int, int> > > mpLowP;
    list<pair<int, int> >::iterator itpaNode;
    map<int, list<pair<int, int> > >::reverse_iterator ritmpP;
    map<pair<int, int>, list<pair<int, int> > >::iterator itmpKPLs;

    unordered_map<int, TPSTMP_P_INFO>::iterator itmpLNode;

    map<int, list<pair<int, int> > >::iterator itmpN;
    list<pair<int, int> >::iterator itpaN;
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
        itmpLNode->second.bInPriQFlag = true;
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

        switch (eCurS)
        {
        case E_RM_NODE:
            iCurP = myIntArray.getTop();
            iMinD = myIntArray.getDyKey(iCurP);
            if (iCurK > iMinD + 2)
            {
                /* rm node, */
                DEBUG_ASSERT(lstWaitRm.empty());
                myIntArray.getTopList(lstWaitRm);
                for (itPid = lstWaitRm.begin(); itPid != lstWaitRm.end(); ++itPid)
                {
                    myIntArray.myDel(*itPid);
                    pstPInfo = &(mpLocalP[*itPid]);
                    pstPInfo->iSeSup = iMinD;
                    pstPInfo->iKSup = iMinD;
                    pstPInfo->iKMSup = iMinD;
                    pstPInfo->iLayer = 1;
                    pstPInfo->bInPriQFlag = false;
                }
                lstWaitRmSla.merge(lstWaitRm);
            }
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
                break;
            }
            plsKE = &(itmpKPLs->second);
            DEBUG_ASSERT(NULL != plsKE);

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
                iLcCurP = paXY.first;
                DEBUG_ASSERT(mpLocalP.find(iLcCurP) != mpLocalP.end());
                pstPInfo = &(mpLocalP[iLcCurP]);
                if (!(pstPInfo->bInPriQFlag))
                {
                    /* in queue */
                    mpLocalP[iLcCurP].iSeSup--;
                }
                else
                {
                    iLcD = pstPInfo->iLocalD;
                    if (iCurK != pstPInfo->iCurK)
                    {
                        pstPInfo->iCurK = iCurK;
                        pstPInfo->iCurLayer = iCurL;
                        pstPInfo->iKSup = iLcD;
                        pstPInfo->iKMSup = iLcD;
                    }
                    else if (iCurL != pstPInfo->iCurLayer)
                    {
                        pstPInfo->iCurLayer = iCurL;
                        pstPInfo->iKMSup = iLcD;
                    }
                    --iLcD;
                    pstPInfo->iLocalD = iLcD;

                    if (iLcD < iCurK - 2)
                    {
                        /* cannot remained */
                        lstWaitRmSla.push_back(iLcCurP);
                        myIntArray.myDel(iLcCurP);
                        pstPInfo->iLayer = iCurL + 1;
                        pstPInfo->iSeSup = iLcD;
                        pstPInfo->bInPriQFlag = false;
                    }
                    else
                    {
                        /* reinsert */
                        myIntArray.insetByOrder(iLcD, iLcCurP);
                    }
                }

                iLcCurP = paXY.second;
                DEBUG_ASSERT(mpLocalP.find(iLcCurP) != mpLocalP.end());
                pstPInfo = &(mpLocalP[iLcCurP]);

                if (!(pstPInfo->bInPriQFlag))
                {
                    /* in queue */
                    pstPInfo->iSeSup--;
                }
                else
                {
                    iLcD = pstPInfo->iLocalD;
                    if (iCurK != pstPInfo->iCurK)
                    {
                        pstPInfo->iCurK = iCurK;
                        pstPInfo->iCurLayer = iCurL;
                        pstPInfo->iKSup = iLcD;
                        pstPInfo->iKMSup = iLcD;
                    }
                    else if (iCurL != pstPInfo->iCurLayer)
                    {
                        pstPInfo->iCurLayer = iCurL;
                        pstPInfo->iKMSup = iLcD;
                    }
                    --iLcD;
                    pstPInfo->iLocalD = iLcD;

                    if (iLcD < iCurK - 2)
                    {
                        /* cannot remain */
                        lstWaitRmSla.push_back(iLcCurP);
                        myIntArray.myDel(iLcCurP);
                        pstPInfo->iLayer = iCurL + 1;
                        pstPInfo->iSeSup = iLcD;
                        pstPInfo->bInPriQFlag = false;
                    }
                    else
                    {
                        /* reinsert */
                        myIntArray.insetByOrder(iLcD, iLcCurP);
                    }
                }
                mpLocalG.erase(paXY);
                mpLocalG.erase(pair<int, int>(paXY.second, paXY.first));
            }
            mpKPLs.erase(mpKPLs.begin());

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
            /* fine, next k */
            myIntArray.getAll(mpLowP[iCurK]);
            /* could be (k+1)-truss */
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

        while(!lstWaitRm.empty() || !lstWaitRmSla.empty())
        {
            int x = 0;
            int y = 0;
            int iRmPid = 0;

            if (lstWaitRm.empty())
            {
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
                for (itPid = lstWaitRm.begin(); itPid != lstWaitRm.end(); )
                {
                    iRmPid = *itPid;
                    if (mpLocalP[iRmPid].iLayer > iCurL)
                    {
                        /* delay rm */
                        lstWaitRmSla.push_back(iRmPid);
                        lstWaitRm.erase(itPid++);
                        continue;
                    }
                    mpLocalP[iRmPid].bLock = true;
                    ++itPid;
                }
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

            vector <pair<int, int> > vTpNeibP;
            vector <pair<int, int> >::iterator itP;
            mpG.findPNebP(iRmPid, vTpNeibP, mpLocalG);
            for (itP = vTpNeibP.begin(); itP != vTpNeibP.end(); ++itP)
            {
                int iLcD = 0;
                int iCurP = itP->first;
                pstPInfo = &(mpLocalP[iCurP]);
                if (pstPInfo->bRmFlag || pstPInfo->bLock)
                {
                    continue;
                }
                if (!(pstPInfo->bInPriQFlag))
                {
                    /* in rm queue */
                    pstPInfo->iSeSup--;
                    continue;
                }
                iLcD = pstPInfo->iLocalD;
                if (iCurK != pstPInfo->iCurK)
                {
                    pstPInfo->iCurK = iCurK;
                    pstPInfo->iCurLayer = iCurL;
                    pstPInfo->iKSup = iLcD;
                    pstPInfo->iKMSup = iLcD;
                }
                else if (iCurL != pstPInfo->iCurLayer)
                {
                    pstPInfo->iCurLayer = iCurL;
                    pstPInfo->iKMSup = iLcD;
                }
                --iLcD;
                pstPInfo->iLocalD = iLcD;

                if (iLcD < iCurK - 2)
                {
                    /* cannot remain */
                    lstWaitRmSla.push_back(iCurP);
                    myIntArray.myDel(iCurP);
                    pstPInfo->iSeSup = iLcD;
                    pstPInfo->iLayer = iCurL + 1;
                    pstPInfo->bInPriQFlag = false;
                }
                else
                {
                    /* reinsert */
                    myIntArray.insetByOrder(iLcD, iCurP);
                }
            } // end for neighbor
        } // end for wait queue

        //DEBUG_PRINTF("DECOMP waited done\n");
    } // end for queue

    for (ritmpP = mpLowP.rbegin(); ritmpP != mpLowP.rend(); ++ritmpP)
    {
        int iTpK = ritmpP->first;
        for (itpaNode = ritmpP->second.begin(); itpaNode != ritmpP->second.end(); ++itpaNode)
        {
            int iTpD = itpaNode->first;
            int iTpPId = itpaNode->second;
            pstPInfo = &(mpLocalP[iTpPId]);
            pstNode = mpG.findNode(pstPInfo->iEid);
            DEBUG_ASSERT(NULL != pstNode);
            if (iTpK >= pstPInfo->iLowB)
            {
                pstPInfo->iLowB = iTpK;
            }
        }
    }
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
int delPart::nodeDec(myG &mpG, int iNode, vector <int> &vCdtE)
{
    list<int> lstNeibP;
    list<int>::iterator itNeibP;
    list<int> lsThirdE;
    list<int>::iterator itE;
    /* <k, layer>, <pid, pid> */
    map<pair<int, int>, list<pair<int, int> > > mpKPLs;
    map<pair<int, int>, int> mpLocalG;

    vector <int> vVisited;
    vector <int>::iterator itvE;
    vector <int> vCheck;

    /* pid, nodeInfo */
    unordered_map<int, TPSTMP_P_INFO> mpLocalP;
    unordered_map<int, TPSTMP_P_INFO>::iterator itmpP;
    TPSTMP_P_INFO *pstPInfo = NULL;

    TPST_MAP_BY_EID* pstNode = NULL;

    bool res = false;

    /* get valuable edges, filter no possible edges (sup+2 < k-1 or trussness > k-1) */
    mpG.findPNebPTrdE(iNode, lstNeibP, lsThirdE);
    DEBUG_PRINTF("DEBUG find done\n");
    for (itNeibP = lstNeibP.begin(); itNeibP != lstNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[*itNeibP]);
        pstPInfo->iPid = *itNeibP;
        pstPInfo->iLocalD = 0;
        pstPInfo->iMaxK = 2;

        pstPInfo->iLowB = 2;
        pstPInfo->iLayer = 1;
        pstPInfo->iSeSup = 0;
        pstPInfo->bRmFlag = false;
        pstPInfo->bLock = false;

        pstNode = mpG.findNode(iNode, *itNeibP);
        DEBUG_ASSERT(NULL != pstNode);
        vVisited.push_back(pstNode->eid);
    }

    DEBUG_PRINTF("DEBUG neigh done\n");
    for (itE = lsThirdE.begin(); itE != lsThirdE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        /* at least 3-truss */
        if (pstNode->iTrussness < 3)
        {
            /* couldn't happen in removal */
            DEBUG_ASSERT(0);
        }
        //mpG.checkSeSup(pstNode->eid);
        vVisited.push_back(pstNode->eid);

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

    localDec(mpG, iNode, mpLocalG, mpLocalP, mpKPLs);

    /* add seed */
    for (itE = lsThirdE.begin(); itE != lsThirdE.end(); ++itE)
    {
        int iTpMinK = 0;
        int iTpMinT = 0;
        int iTpMinL = 0;
        int iTpMinPreL = 0;
        bool bDeSup = false;
        bool bCheck = false;
        TPST_MAP_BY_EID* pstLeftNode = NULL;
        TPST_MAP_BY_EID* pstRightNode = NULL;

        unordered_map<int, TPSTMP_P_INFO>::iterator itLfP;
        unordered_map<int, TPSTMP_P_INFO>::iterator itRtP;

        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        itLfP = mpLocalP.find(pstNode->paXY.first);
        DEBUG_ASSERT(mpLocalP.end() != itLfP);
        itRtP = mpLocalP.find(pstNode->paXY.second);
        DEBUG_ASSERT(mpLocalP.end() != itRtP);

        DEBUG_PRINTF("DEBUB after local third (%d, %d) k: %d Layer: %d SeSup: %d, low: %d %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     itLfP->second.iLowB, itRtP->second.iLowB);

        iTpMinK = COMMON_MIN(itLfP->second.iLowB,
                            itRtP->second.iLowB);
        if (iTpMinK > pstNode->iTrussness)
        {
            /* cannot decrease sup, ignore */
            continue;
        }
        else if (iTpMinK == pstNode->iTrussness)
        {
            iTpMinL = COMMON_MIN_LAYER(itLfP->second.iLowB, itLfP->second.iLayer,
                                       itRtP->second.iLowB, itRtP->second.iLayer);
            if (iTpMinL >= pstNode->iLayer)
            {
                continue;
            }
        }

        pstLeftNode = mpG.findNode(iNode, pstNode->paXY.first);
        DEBUG_ASSERT(NULL != pstLeftNode);
        pstRightNode = mpG.findNode(iNode, pstNode->paXY.second);
        DEBUG_ASSERT(NULL != pstRightNode);

        iTpMinT = COMMON_MIN(pstLeftNode->iTrussness,
                            pstRightNode->iTrussness);
        /* add seed */
        if (iTpMinT > pstNode->iTrussness)
        {
            bDeSup = true;
        }
        else if (iTpMinT < pstNode->iTrussness)
        {
            continue;
        }
        else
        {
            iTpMinPreL = COMMON_MIN_LAYER(pstLeftNode->iTrussness, pstLeftNode->iLayer,
                                       pstRightNode->iTrussness, pstRightNode->iLayer);
            if (iTpMinPreL < pstNode->iLayer)
            {
                if ((iTpMinPreL == pstNode->iLayer - 1) && (iTpMinPreL != iTpMinL))
                {
                    /* may decrease preLayer SeSup */
                    bCheck = true;
                }
            }
            else
            {
                bDeSup = true;
            }
        }

        if (bDeSup)
        {
            pstNode->iSeSup--;
            bCheck = true;
            DEBUG_PRINTF("DEBUB decrease Sesup (%d, %d) k: %d Layer: %d SeSup: %d, check: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup,
                         bCheck);
        }
        if (bCheck)
        {
            vCheck.push_back(pstNode->eid);
        }
    }

    /* set trussness */
    for (itNeibP = lstNeibP.begin(); itNeibP != lstNeibP.end(); ++itNeibP)
    {
        pstPInfo = &(mpLocalP[*itNeibP]);
        pstNode = mpG.findNode(iNode, *itNeibP);
        DEBUG_ASSERT(NULL != pstNode);

        /* set low bound as trussness */
        pstNode->iTrussness = pstPInfo->iLowB;
        pstNode->iLayer = pstPInfo->iLayer;
        pstNode->iSeSup = pstPInfo->iSeSup;
    }

    /* check condition in seed */
    for (itvE = vCheck.begin(); itvE != vCheck.end(); ++itvE)
    {
        pstNode = mpG.findNode(*itvE);
        DEBUG_ASSERT(NULL != pstNode);

        res = checkRmCdt(mpG, pstNode->eid);
        if (!res)
        {
            vCdtE.push_back(pstNode->eid);
            DEBUG_PRINTF("DEBUB candidature (%d, %d) k: %d Layer: %d SeSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup);
        }
    }

    degradeForAllK(mpG, vCdtE);

    return 0;
}
#endif
/*****************
input:
        myG &mpG
        int iNode
description:
        find up/low bound for p neighbor
******************/
int delPart::ballDec(myG &mpG, int iNode, vector <int> &vRmP)
{
    /* pid, eid */
    vector <pair<int, int> > vNeibP;
    map <int, int> mpNeibP;
    vector <int> vThirdE;
    vector <int> vRemainTrdE;
    vector <int>::iterator itE;
    /* <k, layer>, <pid, pid> */
    map<pair<int, int>, list<pair<int, int> > > mpKPLs;
    /* <pid, pid>, k */
    map<pair<int, int>, int> mpLocalG;

    vector <int> vTpChgKE;
    vector <int> vChgKE;
    vector <int> vTpChgLE;
    vector <int> vChgLE;

    /* pid, nodeInfo */
    unordered_map<int, TPSTMP_P_INFO> mpLocalP;
    unordered_map<int, TPSTMP_P_INFO>::iterator itmpP;
    TPSTMP_P_INFO *pstPInfo = NULL;

    TPST_MAP_BY_EID* pstNode = NULL;

    bool res = false;

	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;

    /*DEBUG_PRINTF("BALL center: %d total size: %d\n",
                 iNode, mpG.m_pvPNe->size());*/
    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    for (auto atP : (*(mpG.m_pvPNe))[iNode])
    {
        pstNode = mpG.findNode(atP.second);
        DEBUG_ASSERT(NULL != pstNode);
        /*DEBUG_PRINTF("BALL neighbor edge init (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->iKSup, pstNode->iKMSup,
                     pstNode->mpLfE.size());*/
        if (!pstNode->bInit)
        {
            mpG.init(pstNode->eid);
        }
    }
    mpG.findPNebPTrdE(iNode, vNeibP, vThirdE);
    for (int iEid : vThirdE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        /*DEBUG_PRINTF("BALL third edge init (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->iKSup, pstNode->iKMSup,
                     pstNode->mpLfE.size());*/
        if (!pstNode->bInit)
        {
            mpG.init(pstNode->eid);
        }
    }
    /*DEBUG_PRINTF("BALL center: %d neighbor size: %d third size: %d\n",
                 iNode, vNeibP.size(), vThirdE.size());*/
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreFindTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    /* remove edges */
    for (auto atP : vNeibP)
    {
        int iPid = atP.first;
        int iEid = atP.second;
        mpNeibP[iPid] = iEid;

        /*pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        DEBUG_PRINTF("BALL origin neighbor: %d eid: %d k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                     iPid, iEid,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->iKSup, pstNode->iKMSup);*/
    }
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmLoadTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    for (int iRmP : vRmP)
    {
        map <int, int>::iterator itTpP = mpNeibP.find(iRmP);
        DEBUG_ASSERT(itTpP != mpNeibP.end());
        int iEid = itTpP->second;
        simpleRm(mpG, iEid);
        mpNeibP.erase(itTpP);
        //DEBUG_PRINTF("BALL rm neighbor: %d\n", iRmP);
    }
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    /* load */
    for (auto atP : mpNeibP)
    {
        pstPInfo = &(mpLocalP[atP.first]);
        pstPInfo->iPid = atP.first;
        pstPInfo->iEid = atP.second;
        pstPInfo->iLocalD = 0;
        pstPInfo->iMaxK = 2;

        pstPInfo->iLowB = 2;
        pstPInfo->iLayer = 1;
        pstPInfo->iCurK = 0;
        pstPInfo->iCurLayer = 0;
        pstPInfo->iSeSup = 0;
        pstPInfo->iKSup = 0;
        pstPInfo->iKMSup = 0;
        pstPInfo->bRmFlag = false;
        pstPInfo->bInPriQFlag = false;
        pstPInfo->bLock = false;
    }

    //DEBUG_PRINTF("BALL neigh done\n");
    for (itE = vThirdE.begin(); itE != vThirdE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        /*DEBUG_PRINTF("BALL origin third (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d neighbor size: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->iKSup, pstNode->iKMSup,
                     pstNode->mpLfE.size());*/
#if 0
        /* debug */
        vector<int> vLfE;
        vector<int> vRtE;
        vector<int>::iterator itLfE;
        vector<int>::iterator itRtE;

        mpG.findNeb(pstNode->eid, vLfE, vRtE);
        itLfE = vLfE.begin();
        itRtE = vRtE.begin();
        for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLf = mpG.findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLf);
            pstRt = mpG.findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRt);

            DEBUG_PRINTF("BALL (%d, %d) k: %d L: %d oldL: %d QFlag: %d left: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d QFlag: %d right: (%d, %d) k: %d L: %d seSup: %d, kSup: %d kMSup: %d QFlag: %d\n",
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iTrussness, pstNode->iLayer, pstNode->iOldL, pstNode->bCanQFlag,
                             pstLf->paXY.first, pstLf->paXY.second,
                             pstLf->iTrussness, pstLf->iLayer, pstLf->iSeSup,
                             pstLf->iKSup, pstLf->iKMSup, pstLf->bCanQFlag,
                             pstRt->paXY.first, pstRt->paXY.second,
                             pstRt->iTrussness, pstRt->iLayer, pstRt->iSeSup,
                             pstRt->iKSup, pstRt->iKMSup, pstRt->bCanQFlag);
        }
        /* end debug */
#endif
        /* at least 3-truss */
        if (pstNode->iTrussness < 3)
        {
            /* couldn't happen in removal */
            DEBUG_ASSERT(0);
        }

        if (pstNode->mpLfE.empty())
        {
            /* 2-truss */
            pstNode->iTrussness = 2;
            pstNode->iLayer = 1;
            pstNode->iSeSup = 0;
            pstNode->iKSup = 0;
            pstNode->iKMSup = 0;
            continue;
        }
        /*DEBUG_PRINTF("BALL first neighbor eid: %d %d\n",
                     pstNode->mpLfE.begin()->first, pstNode->mpLfE.begin()->second);*/

        if (mpNeibP.find(pstNode->paXY.first) == mpNeibP.end())
        {
            /* removed */
            continue;
        }
        if (mpNeibP.find(pstNode->paXY.second) == mpNeibP.end())
        {
            /* removed */
            continue;
        }

        vRemainTrdE.push_back(pstNode->eid);

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

    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreLoadTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    localDec(mpG, iNode, mpLocalG, mpLocalP, mpKPLs);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lLocalTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    /* add seed */
    for (itE = vRemainTrdE.begin(); itE != vRemainTrdE.end(); ++itE)
    {
        int iTpMinK = 0;
        int iTpMinT = 0;
        int iTpMinL = 0;
        int iTpMinPreL = 0;
        bool bDeSup = false;
        bool bPreKM = false;
        bool bDesKM = true;
        TPST_MAP_BY_EID* pstLeftNode = NULL;
        TPST_MAP_BY_EID* pstRightNode = NULL;

        unordered_map<int, TPSTMP_P_INFO>::iterator itLfP;
        unordered_map<int, TPSTMP_P_INFO>::iterator itRtP;

        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        itLfP = mpLocalP.find(pstNode->paXY.first);
        DEBUG_ASSERT(mpLocalP.end() != itLfP);
        itRtP = mpLocalP.find(pstNode->paXY.second);
        DEBUG_ASSERT(mpLocalP.end() != itRtP);

        iTpMinK = COMMON_MIN(itLfP->second.iLowB,
                            itRtP->second.iLowB);
        if (iTpMinK > pstNode->iTrussness)
        {
            /* cannot decrease sup, ignore */
            continue;
        }
        else if (iTpMinK == pstNode->iTrussness)
        {
            iTpMinL = COMMON_MIN_LAYER(itLfP->second.iLowB, itLfP->second.iLayer,
                                       itRtP->second.iLowB, itRtP->second.iLayer);
            if (iTpMinL >= pstNode->iLayer)
            {
                continue;
            }
            if (iTpMinL == pstNode->iLayer - 1)
            {
                bDesKM = false;
            }
        }

        pstLeftNode = mpG.findNode(iNode, pstNode->paXY.first);
        DEBUG_ASSERT(NULL != pstLeftNode);
        pstRightNode = mpG.findNode(iNode, pstNode->paXY.second);
        DEBUG_ASSERT(NULL != pstRightNode);

        iTpMinT = COMMON_MIN(pstLeftNode->iTrussness,
                            pstRightNode->iTrussness);
        iTpMinPreL = COMMON_MIN_LAYER(pstLeftNode->iTrussness, pstLeftNode->iLayer,
                                   pstRightNode->iTrussness, pstRightNode->iLayer);
        /* add seed */
        if (iTpMinT > pstNode->iTrussness)
        {
            bDeSup = true;
            bPreKM = true;
        }
        else if (iTpMinT < pstNode->iTrussness)
        {
            continue;
        }
        else
        {
            if (iTpMinPreL >= pstNode->iLayer)
            {
                bDeSup = true;
            }
            if (iTpMinPreL >= pstNode->iLayer - 1)
            {
                bPreKM = true;
            }
        }

        if (bDeSup)
        {
            pstNode->iSeSup--;
            /*DEBUG_PRINTF("BALL decrease Sesup (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup,
                         pstNode->iKSup, pstNode->iKMSup);*/
        }
        if (bPreKM && bDesKM)
        {
            pstNode->iKMSup--;
        }
        if ((iTpMinT >= pstNode->iTrussness) && (iTpMinK < pstNode->iTrussness))
        {
            pstNode->iKSup--;
        }

        /*DEBUG_PRINTF("BALL after local third (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->iKSup, pstNode->iKMSup);*/
    }

    /* set trussness */
    for (auto atP : vNeibP)
    {
        int iPid = atP.first;
        int iEid = atP.second;
        pstPInfo = &(mpLocalP[iPid]);
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);

        if (pstNode->eid != iEid)
        {
            /* removed, ignore */
            continue;
        }

        /* set low bound as trussness */
        pstNode->iTrussness = pstPInfo->iLowB;
        pstNode->iLayer = pstPInfo->iLayer;
        pstNode->iSeSup = pstPInfo->iSeSup;
        pstNode->iKSup = pstPInfo->iKSup;
        pstNode->iKMSup = pstPInfo->iKMSup;

        /*DEBUG_PRINTF("BALL after local neighbor (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness,
                     pstNode->iLayer, pstNode->iSeSup,
                     pstNode->iKSup, pstNode->iKMSup);*/
    }

    /* check condition in seed */
    for (itE = vThirdE.begin(); itE != vThirdE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        if (3 > pstNode->iTrussness)
        {
            continue;
        }

        if (pstNode->iKSup + 2 < pstNode->iTrussness)
        {
            vTpChgKE.push_back(pstNode->eid);
            /*DEBUG_PRINTF("BALL change k (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup,
                         pstNode->iKSup, pstNode->iKMSup);*/
        }
        else if ((pstNode->iKMSup + 2 <= pstNode->iTrussness) && (1 < pstNode->iLayer))
        {
            vTpChgLE.push_back(pstNode->eid);
            /*DEBUG_PRINTF("BALL change layer (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup,
                         pstNode->iKSup, pstNode->iKMSup);*/
        }
    }

    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lAfterLocalTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    updateK(mpG, vTpChgKE, vChgKE);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lUpdateKTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    recalL(mpG, vChgKE, vChgLE);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lRecalLTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    for (int iEid : vTpChgLE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);

        if ((!pstNode->bDgdFlag) && (!pstNode->bCanQFlag))
        {
            vChgLE.push_back(iEid);
            pstNode->bCanQFlag = true;
        }
    }
    vector <int> vDoneLE;
    //DEBUG_PRINTF("BALL start update layer\n");
    updateL(mpG, vChgLE, vDoneLE);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lUpdateLTime += lCurTime - lStartTime;

    /* restore */
    for (int iEid : vChgKE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);

        pstNode->bDgdFlag = false;
    }
    return 0;
}
/*****************
input:
        myG &mpG
        int iNode
description:
        find up/low bound for p neighbor
******************/
int delPart::singleEDec(myG &mpG, int iNode, int iRmNode)
{
    /* pid, eid */
    vector <int> vThirdE;
    vector <int>::iterator itE;

    vector <int> vTpChgKE;
    vector <int> vChgKE;
    vector <int> vTpChgLE;
    vector <int> vChgLE;

    vector<int> vLfE;
    vector<int> vRtE;
    TPST_MAP_BY_EID* pstNode = NULL;

	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;

    pstNode = mpG.findNode(iNode, iRmNode);
    DEBUG_ASSERT(NULL != pstNode);
    if (0 >= pstNode->eid)
    {
        /* removed */
        return 0;
    }
    if (!pstNode->bInit)
    {
        mpG.init(pstNode->eid);
    }

    mpG.findNeb(pstNode->eid, vLfE, vRtE);
    vThirdE = vLfE;
    vThirdE.insert(vThirdE.end(), vRtE.begin(), vRtE.end());
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreFindTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    simpleRm(mpG, pstNode->eid);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    /* check condition in seed */
    for (itE = vThirdE.begin(); itE != vThirdE.end(); ++itE)
    {
        pstNode = mpG.findNode(*itE);
        DEBUG_ASSERT(NULL != pstNode);

        if (3 > pstNode->iTrussness)
        {
            continue;
        }
        if (pstNode->mpLfE.empty())
        {
            /* 2-truss */
            pstNode->iTrussness = 2;
            pstNode->iLayer = 1;
            pstNode->iSeSup = 0;
            pstNode->iKSup = 0;
            pstNode->iKMSup = 0;
            continue;
        }

        if (pstNode->iKSup + 2 < pstNode->iTrussness)
        {
            vTpChgKE.push_back(pstNode->eid);
            /*DEBUG_PRINTF("BALL change k (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup,
                         pstNode->iKSup, pstNode->iKMSup);*/
        }
        else if ((pstNode->iKMSup + 2 <= pstNode->iTrussness) && (1 < pstNode->iLayer))
        {
            vTpChgLE.push_back(pstNode->eid);
            /*DEBUG_PRINTF("BALL change layer (%d, %d) k: %d Layer: %d SeSup: %d, kSup: %d kMSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness,
                         pstNode->iLayer, pstNode->iSeSup,
                         pstNode->iKSup, pstNode->iKMSup);*/
        }
    }

    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lAfterLocalTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    updateK(mpG, vTpChgKE, vChgKE);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lUpdateKTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    recalL(mpG, vChgKE, vChgLE);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lRecalLTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    for (int iEid : vTpChgLE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);

        if ((!pstNode->bDgdFlag) && (!pstNode->bCanQFlag))
        {
            vChgLE.push_back(iEid);
            pstNode->bCanQFlag = true;
        }
    }
    vector <int> vDoneLE;
    //DEBUG_PRINTF("BALL start update layer\n");
    updateL(mpG, vChgLE, vDoneLE);
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lUpdateLTime += lCurTime - lStartTime;

    /* restore */
    for (int iEid : vChgKE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);

        pstNode->bDgdFlag = false;
    }
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
int delPart::delOneNodePart(myG &mpG, map<int, vector <int> > &mpPrivate, int iNode)
{
    map<int, vector <int> >::iterator itmpP;
    vector<int>::iterator itvTpP;

    DEBUG_ASSERT(iNode <= mpG.m_iMaxPId);

    itmpP = mpPrivate.find(iNode);
    if (itmpP != mpPrivate.end())
    {
        itvTpP = itmpP->second.begin();
        if (itvTpP == itmpP->second.end())
        {
            /* empty */
            return 0;
        }
        ++itvTpP;
        if (itvTpP == itmpP->second.end())
        {
            /* single edge */
            singleEDec(mpG, iNode, itmpP->second.front());
        }
        else
        {
            ballDec(mpG, iNode, itmpP->second);
        }
    }
    return 0;
}

