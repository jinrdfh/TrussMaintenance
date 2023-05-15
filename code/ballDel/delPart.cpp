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

long g_lRecalLInitTime;
long g_lRecalLCntTime;
long g_lRecalLCalTime;

long g_lBFSCnt;
long g_lDeCnt;
long g_lReCalCnt;
long g_lNeibInitCnt;
long g_lLInitCnt;
long g_lLReuseCnt;
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
    vector<uint32_t>::iterator itLfE;
    vector<uint32_t>::iterator itRtE;
    DEBUG_ASSERT(vChgKE.empty());
    /* k, eid */
    /*map<int, vector<int> > mpOrder;
    map<int, vector<int> >::reverse_iterator ritE;

    for (int iEid : vCdtE)
    {
        pstNode = mpG.findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        if (pstNode->iKSup + 2 < pstNode->iTrussness)
        {
            mpOrder[pstNode->iTrussness].push_back(iEid);
            pstNode->bCanQFlag = true;
        }
        else
        {
            printf("ERROR get (%d, %d) k: %d L: %d Sup: %d %d %d\n",
                   pstNode->paXY.first, pstNode->paXY.second,
                   pstNode->iTrussness, pstNode->iLayer,
                   pstNode->iSeSup, pstNode->iKSup, pstNode->iKMSup);
            DEBUG_ASSERT(0);
        }
    }
    for (ritE = mpOrder.rbegin(); ritE != mpOrder.rend(); ++ritE)
    {
        vStack.insert(vStack.end(), ritE->second.begin(), ritE->second.end());
    }*/
    vStack = vCdtE;
    // decreasing order
    //printf("DEBUG start sort\n");
    vector<TPST_MAP_BY_EID> * pvEInfo = mpG.m_pvG;
    sort(vStack.begin(), vStack.end(),
            [pvEInfo](const int& e1, const int& e2) {
            return (*(pvEInfo))[e1].iTrussness > (*(pvEInfo))[e2].iTrussness;
            });
    //printf("DEBUG end sort\n");
    for (int iEid : vStack)
    {
        pstNode = mpG.findNode(iEid);
        pstNode->bCanQFlag = true;

        //printf("DEBUG after sort k: %d\n", pstNode->iTrussness);
    }
    while (!vStack.empty())
    {
        int iCurEid = vStack.back();
        vStack.pop_back();

        pstNode = mpG.findNode(iCurEid);
        DEBUG_ASSERT(NULL != pstNode);
        DEBUG_ASSERT(pstNode->iKSup + 2 < pstNode->iTrussness);
        /*if (!pstNode->bInit)
        {
            mpG.init(pstNode->eid);
        }*/
        pstNode->bCanQFlag = false;

        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLf = mpG.findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLf);
            pstRt = mpG.findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRt);

            /*DEBUG_PRINTF("UPDATE_K (%d, %d) (%d, %d) k: %d %d layer: %d %d left Sup: %d %d %d right Sup: %d %d %d self: (%d, %d) %d %d %d \n",
                         pstLf->paXY.first, pstLf->paXY.second,
                         pstRt->paXY.first, pstRt->paXY.second,
                         pstLf->iTrussness, pstRt->iTrussness,
                         pstLf->iLayer, pstRt->iLayer,
                         pstLf->iSeSup, pstLf->iKSup, pstLf->iKMSup,
                         pstRt->iSeSup, pstRt->iKSup, pstRt->iKMSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer, pstNode->iKSup);*/

            int iMinT = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
            iMinT = COMMON_MIN(iMinT, pstNode->iTrussness);

            if (iMinT == pstNode->iTrussness)
            {
                /* triangle will decrease 1 */
                if (iMinT == pstLf->iTrussness)
                {
                    /*if (!pstLf->bInit)
                    {
                        mpG.init(pstLf->eid);
                    }*/
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
                    /*if (!pstRt->bInit)
                    {
                        mpG.init(pstRt->eid);
                    }*/
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
        /*DEBUG_PRINTF("UPDATE_K decrease (%d, %d) k: %d oldL: %d kSup: %d \n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer, pstNode->iKSup);*/
        ++g_lDeCnt;
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
    vector<uint32_t>::iterator itLfE;
    vector<uint32_t>::iterator itRtE;

    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    //DEBUG_ASSERT(pstNode->bInit);
    DEBUG_ASSERT(pstNode->bLInit);

    itLfE = pstNode->vKLfE.begin();
    itRtE = pstNode->vKRtE.begin();
    for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
    {
        pstLf = mpG.findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLf);
        pstRt = mpG.findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRt);

        /*DEBUG_PRINTF("UPDATE_L (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) k: %d layer: %d Sup: %d %d %d\n",
                     pstLf->paXY.first, pstLf->paXY.second,
                     pstRt->paXY.first, pstRt->paXY.second,
                     pstLf->iTrussness, pstRt->iTrussness,
                     pstLf->iLayer, pstRt->iLayer,
                     pstLf->iSeSup, pstRt->iSeSup,
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup, pstNode->iKSup, pstNode->iKMSup);*/

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
                    /*if (!pstLf->bInit)
                    {
                        ++g_lNeibInitCnt;
                        mpG.init(pstLf->eid);
                    }
                    else
                    {
                        pstLf->iKMSup--;
                    }*/
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
                    /*if (!pstRt->bInit)
                    {
                        ++g_lNeibInitCnt;
                        mpG.init(pstRt->eid);
                    }
                    else
                    {
                        pstRt->iKMSup--;
                    }*/
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
    TPST_MAP_BY_EID* pstNode = mpG.findNode(iEid);
    //DEBUG_ASSERT(NULL != pstNode);
    /*if (!pstNode->bInit)
    {
        mpG.init(pstNode->eid);
    }*/

    ++g_lReCalCnt;

    if (!pstNode->bLInit)
    {
        ++g_lLInitCnt;
        /*DEBUG_PRINTF("CAL_L (%d, %d) new initL k: %d\n",
                             pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness);*/
        pstNode->bLInit = true;
        int iKPSup = 0;
        int iNeibMaxL = 0;
        int iKNeibCnt = 0;
        //pstNode->vKLfE.reserve(pstNode->vLfE.size());
        //pstNode->vKRtE.reserve(pstNode->vRtE.size());

        vector<uint32_t>::iterator itLfE = pstNode->vLfE.begin();
        vector<uint32_t>::iterator itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            TPST_MAP_BY_EID* pstLf = mpG.findNode(*itLfE);
            //DEBUG_ASSERT(NULL != pstLf);
            TPST_MAP_BY_EID* pstRt = mpG.findNode(*itRtE);
            //DEBUG_ASSERT(NULL != pstRt);

            /*DEBUG_PRINTF("CAL_L (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d self: (%d, %d) k: %d layer: %d Sup: %d %d %d\n",
                         pstLf->paXY.first, pstLf->paXY.second,
                         pstRt->paXY.first, pstRt->paXY.second,
                         pstLf->iTrussness, pstRt->iTrussness,
                         pstLf->iLayer, pstRt->iLayer,
                         pstLf->iSeSup, pstRt->iSeSup,
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup, pstNode->iKSup, pstNode->iKMSup);*/

            int iMinT = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
            if (iMinT == pstNode->iTrussness)
            {
                /*pstNode->vKLfE.push_back(pstLf->eid);
                pstNode->vKRtE.push_back(pstRt->eid);*/

                pstNode->vKLfE[iKNeibCnt] = pstLf->eid;
                pstNode->vKRtE[iKNeibCnt] = pstRt->eid;
                ++iKNeibCnt;

                int iMinL = INT_MAX;
                if (iMinT == pstLf->iTrussness)
                {
                    iMinL = COMMON_MIN(iMinL, pstLf->iLayer);
                }
                if (iMinT == pstRt->iTrussness)
                {
                    iMinL = COMMON_MIN(iMinL, pstRt->iLayer);
                }
                if (INT_MAX != iMinL)
                {
                    iNeibMaxL = COMMON_MAX(iNeibMaxL, iMinL);
                }
            }
            else if (iMinT > pstNode->iTrussness)
            {
                ++iKPSup;
            }
        }
        pstNode->iKPSup = iKPSup;
        pstNode->iNeibMaxL = iNeibMaxL;
        pstNode->vKLfE.resize(iKNeibCnt);
        pstNode->vKRtE.resize(iKNeibCnt);
    }
    else
    {
        ++g_lLReuseCnt;
    }
    /*DEBUG_PRINTF("CAL_L (%d, %d) init done iKPSup: %d iNeibMaxL: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iKPSup, pstNode->iNeibMaxL);*/

    /* build table */
    /* estimate L */
    /* L, cnt */
    int iMaxL = COMMON_MIN(pstNode->iLayer, pstNode->iNeibMaxL);
    vector<int> vLCnt(iMaxL + 2, 0);
    vector<uint32_t>::iterator itLfE = pstNode->vKLfE.begin();
    vector<uint32_t>::iterator itRtE = pstNode->vKRtE.begin();
    for (; itLfE != pstNode->vKLfE.end(); ++itLfE, ++itRtE)
    {
        TPST_MAP_BY_EID* pstLf = mpG.findNode(*itLfE);
        //DEBUG_ASSERT(NULL != pstLf);
        TPST_MAP_BY_EID* pstRt = mpG.findNode(*itRtE);
        //DEBUG_ASSERT(NULL != pstRt);

        int iMinL = pstNode->iLayer;
        if (pstNode->iTrussness == pstLf->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstLf->iLayer);
        }
        if (pstNode->iTrussness == pstRt->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstRt->iLayer);
        }

        if (iMinL > iMaxL)
        {
            iMinL = iMaxL + 1;
        }

        vLCnt[iMinL]++;
    }

    int iTriCnt = pstNode->iKPSup;
    int iCurL = iMaxL + 1;
    for (; iCurL > 0; --iCurL)
    {
        if (iTriCnt + vLCnt[iCurL] + 2 > pstNode->iTrussness)
        {
            /* find */
            pstNode->iLayer = iCurL + 1;
            pstNode->iSeSup = iTriCnt;
            pstNode->iKMSup = iTriCnt + vLCnt[iCurL];
            break;
        }
        iTriCnt += vLCnt[iCurL];
    }
    /*DEBUG_PRINTF("CAL_L (%d, %d) get L: %d sup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         iCurL, iTriCnt);*/
    if (0 == iCurL)
    {
        /* not full */
        iTriCnt += vLCnt[iCurL];
        DEBUG_ASSERT(iTriCnt + 2 == pstNode->iTrussness);
        pstNode->iLayer = 1;
        pstNode->iSeSup = iTriCnt;
        pstNode->iKMSup = iTriCnt;
    }
    /*DEBUG_PRINTF("CAL_L neigh size: %d k size: %d LMax: %d get L: %d\n",
                 pstNode->vLfE.size(), pstNode->vKLfE.size(),
                         iMaxL + 1, pstNode->iLayer);*/
    /*DEBUG_PRINTF("CAL_L (%d, %d) old: %d new: %d, kMSup: %d SeSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iOldL, pstNode->iLayer, pstNode->iKMSup, pstNode->iSeSup);*/

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
    vector<uint32_t>::iterator itLfE;
    vector<uint32_t>::iterator itRtE;
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
        //DEBUG_ASSERT(pstNode->bInit);

        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLf = mpG.findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLf);
            pstRt = mpG.findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRt);

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
                        /*if (!pstLf->bInit)
                        {
                            mpG.init(pstLf->eid);
                        }*/
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
                        /*if (!pstRt->bInit)
                        {
                            mpG.init(pstRt->eid);
                        }*/
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
                            /*if (!pstLf->bInit)
                            {
                                mpG.init(pstLf->eid);
                            }*/
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
                            /*if (!pstRt->bInit)
                            {
                                mpG.init(pstRt->eid);
                            }*/
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
                                /*if (!pstLf->bInit)
                                {
                                    mpG.init(pstLf->eid);
                                }*/
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
                                    /*if (!pstLf->bInit)
                                    {
                                        mpG.init(pstLf->eid);
                                    }*/
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
                                /*if (!pstRt->bInit)
                                {
                                    mpG.init(pstRt->eid);
                                }*/
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
                                    /*if (!pstRt->bInit)
                                    {
                                        mpG.init(pstRt->eid);
                                    }*/
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

	struct timeval tv;
	long lStartTime = 0;
	long lLcStartTime = 0;
	long lCurTime = 0;
	long lInitTime = 0;
	long lUptTime = 0;
	long lReCalTime = 0;
	long lNeibTime = 0;
    /* <k, L>, eid */
    //map <pair<int, int>, vector<int> > mpPool;
    auto cmp = [](TPST_ORDER &left, TPST_ORDER &right) {
        if (left.k == right.k) {
            if (left.L == right.L) return (left.eid > right.eid);
            else return (left.L > right.L);
        } else return (left.k > right.k); };
    std::priority_queue<TPST_ORDER, std::vector<TPST_ORDER>, decltype(cmp)> prCanQ(cmp);

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
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

        pstNode->iOldL = pstNode->iLayer;
        pstNode->bCanQFlag = false;
        recalL(mpG, iEid);
        //DEBUG_PRINTF("UPDATE_L get new layer: %d\n", pstNode->iLayer);
        if (pstNode->iLayer >= pstNode->iOldL)
        {
            /*printf("ERROR UPDATE_L recalculate (%d, %d) k: %d layer: %d %d KMSup: %d\n",
                   pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness,
                   pstNode->iOldL, pstNode->iLayer, pstNode->iKMSup);
            DEBUG_ASSERT(0);*/
            // not changed
            continue;
        }
        vChgLE.push_back(iEid);

        /* update neighbors */
        vector<int> vCdtChgLE;
        updNeibL(mpG, iEid, vCdtChgLE);

        for (int iChgE : vCdtChgLE)
        {
            TPST_MAP_BY_EID* pstTpNode = mpG.findNode(iChgE);
            DEBUG_ASSERT(NULL != pstTpNode);
            DEBUG_ASSERT((pstTpNode->iKMSup + 2 <= pstTpNode->iTrussness) && (1 < pstTpNode->iLayer));
            pstTpNode->bCanQFlag = true;
            /*DEBUG_PRINTF("UPDATE_L set flag (%d, %d)\n",
                                 pstTpNode->paXY.first, pstTpNode->paXY.second);*/
            //mpPool[pair<int, int>(pstTpNode->iTrussness, pstTpNode->iLayer)].push_back(iChgE);
            prCanQ.push({pstTpNode->iTrussness, pstTpNode->iLayer, iChgE});
            /*DEBUG_PRINTF("UPDATE_L neighbor (%d, %d) k: %d layer: %d KMSup: %d\n",
                   pstTpNode->paXY.first, pstTpNode->paXY.second, pstTpNode->iTrussness,
                   pstTpNode->iLayer, pstTpNode->iKMSup);*/
        }
    }

    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    lInitTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    int iDebugRmCnt = 0;
    while (!prCanQ.empty())
    {
        ++iDebugRmCnt;
        int iEid = prCanQ.top().eid;
        prCanQ.pop();

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
            printf("ERROR (%d, %d) k: %d new layer: %d old layer: %d kMSup: %d\n",
                   pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness,
                   pstNode->iLayer, pstNode->iOldL, pstNode->iKMSup);
            DEBUG_ASSERT(0);
        }
        vChgLE.push_back(iEid);

        /* update neighbors */
        vector<int> vCdtChgLE;
        vCdtChgLE.reserve(pstNode->vKLfE.size() * 2);
        updNeibL(mpG, iEid, vCdtChgLE);
        /* recalculate layer */
        for (int iChgE : vCdtChgLE)
        {
            TPST_MAP_BY_EID* pstTpNode = mpG.findNode(iChgE);
            DEBUG_ASSERT(NULL != pstTpNode);
            DEBUG_ASSERT((pstTpNode->iKMSup + 2 <= pstTpNode->iTrussness) && (1 < pstTpNode->iLayer));
            pstTpNode->bCanQFlag = true;

            //mpPool[pair<int, int>(pstTpNode->iTrussness, pstTpNode->iLayer)].push_back(iChgE);
            prCanQ.push({pstTpNode->iTrussness, pstTpNode->iLayer, iChgE});
            /*DEBUG_PRINTF("UPDATE_L neighbor (%d, %d) k: %d layer: %d KMSup: %d\n",
                   pstTpNode->paXY.first, pstTpNode->paXY.second, pstTpNode->iTrussness,
                   pstTpNode->iLayer, pstTpNode->iKMSup);*/
        }
    }

    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    lUptTime += lCurTime - lStartTime;

    /*DEBUG_PRINTF("UPDATE_L remove num: %d reCalL: %d inNeigh: %d\n",
                 iDebugRmCnt, g_lReCalCnt, g_lNeibInitCnt);
    DEBUG_PRINTF("UPDATE_L seed size: %d all: %d\n", vCdtE.size(), vChgLE.size());
    DEBUG_PRINTF("UPDATE_L init L cnt: %d reuse: %d\n", g_lLInitCnt, g_lLReuseCnt);

    DEBUG_PRINTF("UPDATE_L init time %.3f ms\n", (lInitTime) / 1000.0);
    DEBUG_PRINTF("UPDATE_L update time %.3f ms\n", (lUptTime) / 1000.0);
    DEBUG_PRINTF("UPDATE_L reCal time %.3f ms\n", (lReCalTime) / 1000.0);
    DEBUG_PRINTF("UPDATE_L neigh time %.3f ms\n", (lNeibTime) / 1000.0);
    DEBUG_PRINTF("UPDATE_L g_lRecalLInitTime %.3f ms\n", (g_lRecalLInitTime) / 1000.0);
    DEBUG_PRINTF("UPDATE_L g_lRecalLCntTime %.3f ms\n", (g_lRecalLCntTime) / 1000.0);
    DEBUG_PRINTF("UPDATE_L g_lRecalLCalTime %.3f ms\n", (g_lRecalLCalTime) / 1000.0);*/
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
    vector<uint32_t>::iterator itLfE;
    vector<uint32_t>::iterator itRtE;
    TPST_MAP_BY_EID* pstTpNode = NULL;
    TPST_MAP_BY_EID* pstTpONode = NULL;
    int iTpEid = 0;
    int iTpOEid = 0;

	/*struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;*/

    DEBUG_ASSERT(0 != iEid);
    pstNode = mpG.findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    /*if (!pstNode->bInit)
    {
        mpG.init(pstNode->eid);
    }*/

    /*gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;*/

    itLfE = pstNode->vLfE.begin();
    itRtE = pstNode->vRtE.begin();
    for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
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
        // remove the edge from record
        /*if (pstTpNode->bInit)
        {
            for (int iTpPos = 0; iTpPos < pstTpNode->vLfE.size(); ++iTpPos)
            {
                if ((pstTpNode->vLfE[iTpPos] == iEid) || (pstTpNode->vLfE[iTpPos] == iTpOEid))
                {
                    pstTpNode->vLfE.erase(pstTpNode->vLfE.begin() + iTpPos);
                    pstTpNode->vRtE.erase(pstTpNode->vRtE.begin() + iTpPos);
                    break;
                }
            }
        }
        if (pstTpONode->bInit)
        {
            for (int iTpPos = 0; iTpPos < pstTpONode->vLfE.size(); ++iTpPos)
            {
                if ((pstTpONode->vLfE[iTpPos] == iEid) || (pstTpONode->vLfE[iTpPos] == iTpEid))
                {
                    pstTpONode->vLfE.erase(pstTpONode->vLfE.begin() + iTpPos);
                    pstTpONode->vRtE.erase(pstTpONode->vRtE.begin() + iTpPos);
                    break;
                }
            }
        }*/
        for (int iTpPos = 0; iTpPos < pstTpNode->vLfE.size(); ++iTpPos)
        {
            if ((pstTpNode->vLfE[iTpPos] == iEid) || (pstTpNode->vLfE[iTpPos] == iTpOEid))
            {
                pstTpNode->vLfE.erase(pstTpNode->vLfE.begin() + iTpPos);
                pstTpNode->vRtE.erase(pstTpNode->vRtE.begin() + iTpPos);
                break;
            }
        }
        for (int iTpPos = 0; iTpPos < pstTpONode->vLfE.size(); ++iTpPos)
        {
            if ((pstTpONode->vLfE[iTpPos] == iEid) || (pstTpONode->vLfE[iTpPos] == iTpEid))
            {
                pstTpONode->vLfE.erase(pstTpONode->vLfE.begin() + iTpPos);
                pstTpONode->vRtE.erase(pstTpONode->vRtE.begin() + iTpPos);
                break;
            }
        }


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
    DEBUG_ASSERT(itRtE == pstNode->vRtE.end());

    /*gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmCoreTime += lCurTime - lStartTime;*/

    pstNode->vLfE.clear();
    pstNode->vLfE.clear();

    /*gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;*/
    mpG.rm(iEid);
    /*gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreRmBasicTime += lCurTime - lStartTime;*/

    return 0;
}
# if 0
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
bool delPart::cmpOrder(const TPST_ORDER& e1, const TPST_ORDER& e2)
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
        int iNode
        vector <AdjEntry> &vNeibE
        vector <int> &vRmNeb
output:
        vector <int> &vRmE
        vector <int> &vKSeed
        vector <int> &vLSeed
description:
        local decomposition
        neighbor edges maintain: k, layer, seSup, kSup, kMSup
        third edges maintain: seSup, kSup, kMSup, seed
******************/
int delPart::localDec(myG &mpG, int iNode, vector <AdjEntry> &vNeibE, vector <int> &vRmNeb, vector <int> &vRmE, vector <int> &vKSeed, vector <int> &vLSeed)
{
    /* all are temp id */
    vector<vector<AdjEntry> > adj;
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID *pstLf = NULL;
    TPST_MAP_BY_EID *pstRt = NULL;
    TPST_MAP_BY_EID *pstDes = NULL;
    TPST_MAP_BY_EID *pstTrd = NULL;
    vector <uint32_t>::iterator itLfE;
    vector <uint32_t>::iterator itRtE;
    vector <int> vEdges;
    vector <TPST_ORDER> vTrdOrder;
    vector <pair<int, int> > vPair;
    int iSupMax = 0;

    int i_N = vNeibE.size();
    int i_M = 0;

    sort(vRmNeb.begin(), vRmNeb.end());

    //DEBUG_PRINTF("LOCAL start degree: %d\n", i_N);

    //DEBUG_PRINTF("LOCAL reserve done\n");
    /* rename id */
    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        pstNode = mpG.findNode(vNeibE[iTpId].eid);
        //DEBUG_ASSERT(NULL != pstNode);
        pstNode->iTpId = iTpId;
        /*if (!pstNode->bInit)
        {
            mpG.init(pstNode->eid);
        }*/
    }

    //DEBUG_PRINTF("LOCAL adj start\n");
    // initialize adjacency arrays
    adj.resize(i_N);
    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        int iEid = vNeibE[iTpId].eid;

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
        int iEid = vNeibE[iTpId].eid;
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
            vEdges.push_back(pstTrd->eid);
            vPair.push_back(pair<int, int>(pstNode->iTpId, pstDes->iTpId));
            int iTpEid = vEdges.size() - 1;
            pstTrd->iTpId = iTpEid;
            adj[pstNode->iTpId].push_back({pstDes->iTpId, iTpEid});
            adj[pstDes->iTpId].push_back({pstNode->iTpId, iTpEid});

            vTrdOrder.push_back({pstTrd->iTrussness, pstTrd->iLayer, iTpEid});
            /*DEBUG_PRINTF("LOCAL adj (%d, %d) k: %d L:%d\n",
                         pstTrd->paXY.first, pstTrd->paXY.second,
                         pstTrd->iTrussness, pstTrd->iLayer);*/
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
    vector<int> v_kSup(i_N, 0);
    vector<int> v_LMSup(i_N, 0);
    vector<int> v_LMK(i_N, 0);
    vector<int> v_LML(i_N, 0);
    vector<bool> vNRmd(i_N, false);
    vector<bool> vNWaitRmd(i_N, false);
    vector<bool> vNChgD(i_N, false);
    vector<bool> vERmd(i_M, false);
    vector<int> vChgDQ;
    vChgDQ.reserve(i_N);

    auto udtSup = [&v_LMK, &v_LML, &v_kSup, &v_LMSup, &v_D, &vNWaitRmd](const int iNode, const int iCurK, const int iPreL) {
        if (vNWaitRmd[iNode])
        {
            /* will be removed */
            return;
        }
        if (iCurK == v_LMK[iNode]){
            if (iPreL == v_LML[iNode]){
                /* nothing */
                return;
            }
        } else {
            /* kSup */
            v_kSup[iNode] = v_D[iNode];
            v_LMK[iNode] = iCurK;
        }
        /* LMSup */
        v_LMSup[iNode] = v_D[iNode];
        v_LML[iNode] = iPreL;
        return;
    };

    int iCurK = 1;
    int iCurL = 1;
    int iRmCnt = 0;
    vector<int> vWait;
    vWait.reserve(i_N);

    // push edges removed in to queue
    vector<int>::iterator itRmPos = vRmNeb.begin();
    for (int pid = 0; pid < i_N; ++pid)
    {
        if (vNeibE[pid].pid == *itRmPos)
        {
            /* should be removed */
            v_t[pid] = 1;
            v_L[pid] = 1;
            v_SeSup[pid] = 0;
            vWait.push_back(pid);
            vNWaitRmd[pid] = true;
            vRmE.push_back(vNeibE[pid].eid);
            ++itRmPos;
            if (itRmPos == vRmNeb.end())
            {
                break;
            }
        }
    }

    // 2.2.1. process the edges layer by layer
    while (iRmCnt < i_N)
    {
        //DEBUG_PRINTF("LOCAL current k: %d L: %d\n", iCurK, iCurL);
        while ((!vTrdOrder.empty()) && cmpOrder({iCurK, iCurL, 0}, vTrdOrder.back()))
        {
            /* rm edge */
            int iTpEid = vTrdOrder.back().eid;
            /*DEBUG_PRINTF("LOCAL rm edges (%d, %d) start len: %d\n",
                         vNeibE[vPair[iTpEid].first].pid,
                         vNeibE[vPair[iTpEid].second].pid, vTrdOrder.size());*/
            vTrdOrder.pop_back();
            if (!vERmd[iTpEid])
            {
                vERmd[iTpEid] = true;
                //DEBUG_ASSERT(0 < v_D[vPair[iTpEid].first]);
                int iTpNode = vPair[iTpEid].first;
                udtSup(iTpNode, iCurK, iCurL - 1);
                --v_D[iTpNode];
                //DEBUG_PRINTF("LOCAL decrease node: %d D: %d\n", vNeibE[iTpNode].pid, v_D[iTpNode]);
                if (!vNChgD[iTpNode])
                {
                    vNChgD[iTpNode] = true;
                    vChgDQ.push_back(iTpNode);
                }

                iTpNode = vPair[iTpEid].second;
                udtSup(iTpNode, iCurK, iCurL - 1);
                --v_D[iTpNode];
                //DEBUG_PRINTF("LOCAL decrease node: %d D: %d\n", vNeibE[iTpNode].pid, v_D[iTpNode]);
                if (!vNChgD[iTpNode])
                {
                    vNChgD[iTpNode] = true;
                    vChgDQ.push_back(iTpNode);
                }
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
                    vNWaitRmd[pid] = true;
                    v_t[pid] = iCurK;
                    v_L[pid] = iCurL;
                    v_SeSup[pid] = i;

                    if (1 == iCurL)
                    {
                        /* new k */
                        v_kSup[pid] = v_D[pid];
                        v_LMSup[pid] = v_D[pid];
                        v_LMK[pid] = iCurK;
                        v_LML[pid] = iCurL;
                    }
                }
            }
            bin[i].clear();
        }

        if (vWait.empty())
        {
            if (!vTrdOrder.empty())
            {
                if ((cmpOrder({iCurK + 1, 1, 0}, vTrdOrder.back())))
                {
                    ++iCurL;
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
            //DEBUG_PRINTF("LOCAL rm node: %d\n", vNeibE[pid].pid);
            vNRmd[pid] = true;
            vNWaitRmd[pid] = false;
            ++iRmCnt;

            for (const auto neiP : adj[pid])
            {
                if (vNRmd[neiP.pid]) continue;
                if (vERmd[neiP.eid]) continue;

                udtSup(neiP.pid, iCurK, iCurL);
                --v_D[neiP.pid];
                //DEBUG_PRINTF("LOCAL decrease node: %d D: %d\n", vNeibE[neiP.pid].pid, v_D[neiP.pid]);
                if (!vNChgD[neiP.pid])
                {
                    vNChgD[neiP.pid] = true;
                    vChgDQ.push_back(neiP.pid);
                }
                vERmd[neiP.eid] = true;
            }
        }
        vWait.clear();
        for (int iTpPid : vChgDQ)
        {
            DEBUG_ASSERT(-1 < v_D[iTpPid]);
            bin[v_D[iTpPid]].push_back(iTpPid);
            vNChgD[iTpPid] = false;
        }
        vChgDQ.clear();
        ++iCurL;
    }

    //DEBUG_PRINTF("LOCAL save start\n");
    /* save result */
    for (int iTpId = 0; iTpId < i_M; ++iTpId)
    {
        int iEid = vEdges[iTpId];
        int iLfTpPid = vPair[iTpId].first;
        int iRtTpPid = vPair[iTpId].second;
        int iLfEid = vNeibE[iLfTpPid].eid;
        int iRtEid = vNeibE[iRtTpPid].eid;

        bool bChg = false;

        pstNode = mpG.findNode(iEid);
        //DEBUG_ASSERT(NULL != pstNode);
        pstLf = mpG.findNode(iLfEid);
        //DEBUG_ASSERT(NULL != pstLf);
        pstRt = mpG.findNode(iRtEid);
        //DEBUG_ASSERT(NULL != pstRt);

        /*DEBUG_PRINTF("LOCAL (%d, %d) (%d, %d) k: %d %d %d %d layer: %d %d %d %d seSup: %d %d self: (%d, %d) k: %d layer: %d Sup: %d %d %d\n",
                     pstLf->paXY.first, pstLf->paXY.second,
                     pstRt->paXY.first, pstRt->paXY.second,
                     pstLf->iTrussness, pstRt->iTrussness,
                     v_t[iLfTpPid], v_t[iRtTpPid],
                     pstLf->iLayer, pstRt->iLayer,
                     v_L[iLfTpPid], v_L[iRtTpPid],
                     pstLf->iSeSup, pstRt->iSeSup,
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup, pstNode->iKSup, pstNode->iKMSup);*/

        int iOldMinK = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
        iOldMinK = COMMON_MIN(iOldMinK, pstNode->iTrussness);
        int iNewMinK = COMMON_MIN(v_t[iLfTpPid], v_t[iRtTpPid]);
        iNewMinK = COMMON_MIN(iNewMinK, pstNode->iTrussness);

        int iOldMinL = INT_MAX;
        if (iOldMinK == pstLf->iTrussness)
        {
            iOldMinL = COMMON_MIN(iOldMinL, pstLf->iLayer);
        }
        if (iOldMinK == pstRt->iTrussness)
        {
            iOldMinL = COMMON_MIN(iOldMinL, pstRt->iLayer);
        }
        if (iOldMinK == pstNode->iTrussness)
        {
            iOldMinL = COMMON_MIN(iOldMinL, pstNode->iLayer);
        }
        int iNewMinL = INT_MAX;
        if (iNewMinK == v_t[iLfTpPid])
        {
            iNewMinL = COMMON_MIN(iNewMinL, v_L[iLfTpPid]);
        }
        if (iNewMinK == v_t[iRtTpPid])
        {
            iNewMinL = COMMON_MIN(iNewMinL, v_L[iRtTpPid]);
        }
        if (iNewMinK == pstNode->iTrussness)
        {
            iNewMinL = COMMON_MIN(iNewMinL, pstNode->iLayer);
        }

        if (iOldMinK == pstNode->iTrussness)
        {
            if (iNewMinK == pstNode->iTrussness)
            {
                /* kSup not change */
                if (iOldMinL == pstNode->iLayer)
                {
                    if (iNewMinL == pstNode->iLayer)
                    {
                        /* all not change */
                    }
                    else if (iNewMinL == pstNode->iLayer - 1)
                    {
                        /* LMSup not change, but SeSup change */
                        pstNode->iSeSup--;
                    }
                    else
                    {
                        /* both LMSup and SeSup change */
                        /*if (!pstNode->bInit)
                        {
                            mpG.init(pstNode->eid);
                        }*/
                        bChg = true;
                        pstNode->iSeSup--;
                        pstNode->iKMSup--;
                    }
                }
                else if (iOldMinL == pstNode->iLayer - 1)
                {
                    /* no SeSup triangle */
                    if (iNewMinL < pstNode->iLayer - 1)
                    {
                        /*if (!pstNode->bInit)
                        {
                            mpG.init(pstNode->eid);
                        }*/
                        pstNode->iKMSup--;
                        bChg = true;
                    }
                }
                else
                {
                    /* no SeSup and LMSup triangle */
                }
            }
            else
            {
                /* kSup change */
                /*if (!pstNode->bInit)
                {
                    mpG.init(pstNode->eid);
                }*/
                bChg = true;
                pstNode->iKSup--;

                if (iOldMinL == pstNode->iLayer)
                {
                    /* both LMSup and SeSup change */
                    pstNode->iSeSup--;
                    pstNode->iKMSup--;
                }
                else if (iOldMinL == pstNode->iLayer - 1)
                {
                    /* no SeSup triangle */
                    pstNode->iKMSup--;
                }
                else
                {
                    /* no SeSup and LMSup triangle */
                }
            }
        }
        else
        {
            /* kSup not change */
        }
        if (bChg)
        {
            if (pstNode->iKSup + 2 < pstNode->iTrussness)
            {
                //pstNode->bCanQFlag = true;
                vKSeed.push_back(pstNode->eid);
            }
            else if ((pstNode->iKMSup + 2 <= pstNode->iTrussness) && (1 < pstNode->iLayer))
            {
                //pstNode->bCanQFlag = true;
                vLSeed.push_back(pstNode->eid);
            }
        }

        /*DEBUG_PRINTF("LOCAL (%d, %d) k: %d layer: %d Sup: %d %d %d bool: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup, pstNode->iKSup, pstNode->iKMSup, pstNode->bCanQFlag);*/
    }
    for (int iTpId = 0; iTpId < i_N; ++iTpId)
    {
        int iEid = vNeibE[iTpId].eid;

        pstNode = mpG.findNode(iEid);
        //DEBUG_ASSERT(NULL != pstNode);
        pstNode->bCanQFlag = false;
        if (pstNode->iTrussness < v_t[iTpId])
        {
            printf("ERROR self: %d local: %d\n", pstNode->iTrussness, v_t[iTpId]);
            printf("LOCAL neighbor error (%d, %d) k: %d layer: %d seSup: %d\n",
                         pstNode->paXY.first, pstNode->paXY.second,
                         pstNode->iTrussness, pstNode->iLayer,
                         pstNode->iSeSup);
            DEBUG_ASSERT(0);
        }
        /*DEBUG_PRINTF("LOCAL neighbor (%d, %d) k: %d %d layer: %d %d old Sup: %d %d %d new Sup: %d %d %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, v_t[iTpId],
                     pstNode->iLayer, v_L[iTpId],
                     pstNode->iSeSup, pstNode->iKSup, pstNode->iKMSup,
                     v_SeSup[iTpId], v_kSup[iTpId], v_LMSup[iTpId]);*/
        if (pstNode->iTrussness > v_t[iTpId])
        {
            pstNode->iTrussness = v_t[iTpId];
            ++g_lDeCnt;

            //DEBUG_PRINTF("LOCAL_DEC decrease (%d, %d)\n", pstNode->paXY.first, pstNode->paXY.second);
        }
        pstNode->iLayer = v_L[iTpId];
        pstNode->iSeSup = v_SeSup[iTpId];
        pstNode->iKSup = v_kSup[iTpId];
        pstNode->iKMSup = v_LMSup[iTpId];

        if (1 < pstNode->iLayer)
        {
            if (pstNode->iKMSup + 2 <= pstNode->iTrussness)
            {
                printf("ERROR record k: %d L: %d\n", v_LMK[iTpId], v_LML[iTpId]);
                DEBUG_ASSERT(0);
            }
        }

    }

    //DEBUG_PRINTF("LOCAL save end\n");
    return 0;
}

/*****************
input:
        myG &mpG
        int iNode
description:
        find up/low bound for p neighbor
******************/
int delPart::singleEDec(myG &mpG, int iNode, int iRmNode, int *piRmEid, vector <int> &vKSeed, vector <int> &vLSeed)
{
    /* pid, eid */
    vector <uint32_t> vThirdE;
    vector <int>::iterator itE;

    vector <int> vTpChgKE;
    vector <int> vChgKE;
    vector <int> vTpChgLE;
    vector <int> vChgLE;

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
    *piRmEid = pstNode->eid;
    /*if (!pstNode->bInit)
    {
        mpG.init(pstNode->eid);
    }*/

    vThirdE = pstNode->vLfE;
    vThirdE.insert(vThirdE.end(), pstNode->vRtE.begin(), pstNode->vRtE.end());
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lPreFindTime += lCurTime - lStartTime;

    gettimeofday(&tv, NULL);
    lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
    /* check condition in seed */

    vector <uint32_t>::iterator itLfE = pstNode->vLfE.begin();
    vector <uint32_t>::iterator itRtE = pstNode->vRtE.begin();
    for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
    {
        TPST_MAP_BY_EID *pstLf = mpG.findNode(*itLfE);
        //DEBUG_ASSERT(NULL != pstLf);
        TPST_MAP_BY_EID *pstRt = mpG.findNode(*itRtE);
        //DEBUG_ASSERT(NULL != pstRt);

        /*DEBUG_PRINTF("SINGLE (%d, %d) (%d, %d) k: %d %d layer: %d %d left Sup: %d %d %d right Sup: %d %d %d self: (%d, %d) %d %d \n",
                     pstLf->paXY.first, pstLf->paXY.second,
                     pstRt->paXY.first, pstRt->paXY.second,
                     pstLf->iTrussness, pstRt->iTrussness,
                     pstLf->iLayer, pstRt->iLayer,
                     pstLf->iSeSup, pstLf->iKSup, pstLf->iKMSup,
                     pstRt->iSeSup, pstRt->iKSup, pstRt->iKMSup,
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer);*/

        int iMinK = COMMON_MIN(pstLf->iTrussness, pstRt->iTrussness);
        iMinK = COMMON_MIN(iMinK, pstNode->iTrussness);
        int iMinL = INT_MAX;
        if (iMinK == pstLf->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstLf->iLayer);
        }
        if (iMinK == pstRt->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstRt->iLayer);
        }
        if (iMinK == pstNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstNode->iLayer);
        }

        if (pstLf->iTrussness == iMinK)
        {
            /*if (!pstLf->bInit)
            {
                mpG.init(pstLf->eid);
            }*/
            pstLf->iKSup--;
            if (pstLf->iLayer == iMinL)
            {
                pstLf->iKMSup--;
                pstLf->iSeSup--;
            }
            else if (pstLf->iLayer - 1 == iMinL)
            {
                pstLf->iKMSup--;
            }
            if (pstLf->iKSup + 2 < pstLf->iTrussness)
            {
                vKSeed.push_back(pstLf->eid);
            }
            else if ((pstLf->iKMSup + 2 <= pstLf->iTrussness) && (1 < pstLf->iLayer))
            {
                vLSeed.push_back(pstLf->eid);
            }
        }
        if (pstRt->iTrussness == iMinK)
        {
            /*if (!pstRt->bInit)
            {
                mpG.init(pstRt->eid);
            }*/
            pstRt->iKSup--;
            if (pstRt->iLayer == iMinL)
            {
                pstRt->iKMSup--;
                pstRt->iSeSup--;
            }
            else if (pstRt->iLayer - 1 == iMinL)
            {
                pstRt->iKMSup--;
            }
            if (pstRt->iKSup + 2 < pstRt->iTrussness)
            {
                vKSeed.push_back(pstRt->eid);
            }
            else if ((pstRt->iKMSup + 2 <= pstRt->iTrussness) && (1 < pstRt->iLayer))
            {
                vLSeed.push_back(pstRt->eid);
            }
        }
        /*DEBUG_PRINTF("SINGLE after (%d, %d) (%d, %d) k: %d %d layer: %d %d left Sup: %d %d %d right Sup: %d %d %d self: (%d, %d) %d %d \n",
                     pstLf->paXY.first, pstLf->paXY.second,
                     pstRt->paXY.first, pstRt->paXY.second,
                     pstLf->iTrussness, pstRt->iTrussness,
                     pstLf->iLayer, pstRt->iLayer,
                     pstLf->iSeSup, pstLf->iKSup, pstLf->iKMSup,
                     pstRt->iSeSup, pstRt->iKSup, pstRt->iKMSup,
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer);*/
    }
    gettimeofday(&tv, NULL);
    lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
    g_lAfterLocalTime += lCurTime - lStartTime;

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
    TPST_MAP_BY_EID* pstNode = NULL;
    map<int, vector <int> >::iterator itmpP;
    vector<int>::iterator itvTpP;

    vector <int> vRmE;
    vector <int> vKSeed;
    vector <int> vLSeed;

	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;

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
        gettimeofday(&tv, NULL);
        lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
        if (itvTpP == itmpP->second.end())
        {
            /* single edge */
            int iEid = 0;
            singleEDec(mpG, iNode, itmpP->second.front(), &iEid, vKSeed, vLSeed);
            vRmE.push_back(iEid);
        }
        else
        {
            localDec(mpG, iNode, mpG.m_vAdj[iNode], itmpP->second, vRmE, vKSeed, vLSeed);
        }
        gettimeofday(&tv, NULL);
        lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
        g_lLocalTime += lCurTime - lStartTime;

        gettimeofday(&tv, NULL);
        lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
        for (int iEid : vRmE)
        {
            simpleRm(mpG, iEid);
        }
        gettimeofday(&tv, NULL);
        lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
        g_lPreRmTime += lCurTime - lStartTime;

        gettimeofday(&tv, NULL);
        lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
        vector <int> vChgKE;
        updateK(mpG, vKSeed, vChgKE);
        gettimeofday(&tv, NULL);
        lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
        g_lUpdateKTime += lCurTime - lStartTime;

        //DEBUG_PRINTF("BALL change K size: %d\n", vChgKE.size());

        gettimeofday(&tv, NULL);
        lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
        vector <int> vLSeedByK;
        recalL(mpG, vChgKE, vLSeedByK);
        gettimeofday(&tv, NULL);
        lCurTime = tv.tv_sec * 1000000 + tv.tv_usec;
        g_lRecalLTime += lCurTime - lStartTime;

        gettimeofday(&tv, NULL);
        lStartTime = tv.tv_sec * 1000000 + tv.tv_usec;
        for (int iEid : vLSeed)
        {
            pstNode = mpG.findNode(iEid);
            DEBUG_ASSERT(NULL != pstNode);

            if ((!pstNode->bDgdFlag) && (!pstNode->bCanQFlag))
            {
                vLSeedByK.push_back(iEid);
                pstNode->bCanQFlag = true;
            }
        }

        //DEBUG_PRINTF("BALL L seed size: %d\n", vLSeedByK.size());
        vector <int> vDoneLE;
        //DEBUG_PRINTF("BALL start update layer\n");
        updateL(mpG, vLSeedByK, vDoneLE);
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
    }
    return 0;
}

