/***************
my class G
****************/

#include "common.h"
#include "myG.h"

long g_lInitCnt;
#if 0
/*****************
input:
        myG &oInitG
description:
        copy map
        calculate stage
******************/
bool myG::copyMap(myG &oInitG)
{
    m_mpBasicG = oInitG.m_mpBasicG;
    m_iMaxPId = oInitG.m_iMaxPId;
    m_iMaxEId = oInitG.m_iMaxEId;
    m_iMaxD = oInitG.m_iMaxD;
    m_iMaxK = oInitG.m_iMaxK;

    /* first time */
    debug_assert(NULL != m_pvPNeCnt);
    *m_pvPNeCnt = *(oInitG.m_pvPNeCnt);
    m_pvPNeCnt->reserve(oInitG.m_pvPNeCnt->capacity());

    /* first time */
    debug_assert(NULL != m_pvG);
    *m_pvG = *(oInitG.m_pvG);
    m_pvG->reserve(oInitG.m_pvG->capacity());
    return true;
}
#endif
/*****************
input:
        int x
        int y
        long double ldP
description:
        add new edge
        init stage
******************/
int myG::add(int x, int y)
{
    TPST_MAP_BY_EID stTp = {0};
    TPST_MAP_BY_EID *pstNode = NULL;
    int iPrevMaxPid = 0;

    //DEBUG_PRINTF("MY_G add begin\n");

    //DEBUG_PRINTF("MY_G find (%d, %d) max: %d\n", x, y, m_iMaxPId);
    pstNode = findNode(x, y);
    //DEBUG_PRINTF("MY_G find done\n");
    if (NULL != pstNode)
    {
        /* have joined, ignore */
        //printf("DEBUG (%d, %d) has joined\n", x, y);
        return 0;
    }

    iPrevMaxPid = m_iMaxPId;
    m_iMaxEId++;
    if (m_iMaxPId < x)
    {
        m_iMaxPId = x;
    }
    if (m_iMaxPId < y)
    {
        m_iMaxPId = y;
    }
    if (iPrevMaxPid < m_iMaxPId)
    {
        /* max pid change */
        m_pvPNeCnt->resize(m_iMaxPId + 1);
        m_vAdj.resize(m_iMaxPId + 1);
    }
    //DEBUG_PRINTF("MY_G resize done\n");

    //m_mpBasicG[pair<int, int>(x, y)] = m_iMaxEId;
    //m_mpBasicG[pair<int, int>(y, x)] = m_iMaxEId;
    //DEBUG_PRINTF("MY_G adj (%d, %d) %d\n", x, y, m_iMaxEId);
    adjInsert(x, y, m_iMaxEId);
    //DEBUG_PRINTF("MY_G adj (%d, %d) %d\n", y, x, m_iMaxEId);
    adjInsert(y, x, m_iMaxEId);
    //DEBUG_PRINTF("MY_G adj done\n");
    //m_vAdj[x].push_back({y, m_iMaxEId});
    //m_vAdj[y].push_back({x, m_iMaxEId});

    stTp.eid = m_iMaxEId;
    //stTp.iSup = 0;
    //stTp.iJuSup = 0;
    stTp.iSeSup = 0;
    //stTp.iOutSup = 0;
    stTp.paXY = myG_getPair(x, y);
    stTp.iTrussness = 2;
    stTp.iLayer = 1;
    //stTp.iAlleyType = 0;
    //stTp.bNewFlag = false;
    stTp.bUgdFlag = false;
    //stTp.bPseNewFlag = false;
    //stTp.bBdBallFlag = false;
    //stTp.bNewJuTriFlag = false;
    //stTp.bSeedFlag = false;
    //stTp.bVstFlag = false;
    stTp.bVisQFlag = false;
    //stTp.bHandledFlag = false;
    //stTp.bRmFlag = false;
    //stTp.bLock = false;
    //stTp.bInsLFlag = false;
    stTp.bCanQFlag = false;
    stTp.bDoneQFlag = false;
    stTp.bUsedCanFlag = false;
    stTp.bUsedDoneFlag = false;
    stTp.bUsedVisFlag = false;
    //stTp.bValueFlag = false;

    m_pvG->push_back(stTp);
    //DEBUG_PRINTF("MY_G push end\n");

    /* check right position */
    DEBUG_ASSERT(m_iMaxEId == (*m_pvG)[m_iMaxEId].eid);

    (*m_pvPNeCnt)[x]++;
    (*m_pvPNeCnt)[y]++;

    if ((*m_pvPNeCnt)[x] > m_iMaxD)
    {
        m_iMaxD = (*m_pvPNeCnt)[x];
        m_iDensePId = x;
    }
    if ((*m_pvPNeCnt)[y] > m_iMaxD)
    {
        m_iMaxD = (*m_pvPNeCnt)[y];
        m_iDensePId = y;
    }

    //DEBUG_PRINTF("MY_G add end\n");
    return m_iMaxEId;
}
/*****************
input:
        myG &mpG
        int iEid
description:
        simply add
******************/
int myG::addNeibTri(int iEid)
{
    TPST_MAP_BY_EID* pstENode = NULL;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    int x = 0;
    int y = 0;

    /* add and init */
    DEBUG_ASSERT(0 != iEid);
    pstENode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstENode);
    x = pstENode->paXY.first;
    y = pstENode->paXY.second;

    pstENode->vLfE.clear();
    pstENode->vRtE.clear();
    //pstENode->vKLfE.clear();
    //pstENode->vKRtE.clear();
    findNeb(x, y, pstENode->vLfE, pstENode->vRtE);

    itLfE = pstENode->vLfE.begin();
    itRtE = pstENode->vRtE.begin();
    for (; itLfE != pstENode->vLfE.end(); ++itLfE, ++itRtE)
    {
        TPST_MAP_BY_EID* pstLfNode = NULL;
        TPST_MAP_BY_EID* pstRtNode = NULL;

        pstLfNode = findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRtNode);

        /*int iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        if (iMinT > pstENode->iTrussness)
        {
            pstENode->vKLfE.push_back(pstLfNode->eid);
            pstENode->vKRtE.push_back(pstRtNode->eid);
        }
        else if (iMinT < pstENode->iTrussness)
        {
            if (pstLfNode->iTrussness <= pstRtNode->iTrussness)
            {
                pstLfNode->vKLfE.push_back(pstRtNode->eid);
                pstLfNode->vKRtE.push_back(pstENode->eid);
            }
            if (pstRtNode->iTrussness <= pstLfNode->iTrussness)
            {
                pstRtNode->vKLfE.push_back(pstENode->eid);
                pstRtNode->vKRtE.push_back(pstLfNode->eid);
            }
        }
        else
        {
            pstENode->vKLfE.push_back(pstLfNode->eid);
            pstENode->vKRtE.push_back(pstRtNode->eid);
            if (pstLfNode->iTrussness == pstENode->iTrussness)
            {
                pstLfNode->vKLfE.push_back(pstRtNode->eid);
                pstLfNode->vKRtE.push_back(pstENode->eid);
            }
            if (pstRtNode->iTrussness == pstENode->iTrussness)
            {
                pstRtNode->vKLfE.push_back(pstENode->eid);
                pstRtNode->vKRtE.push_back(pstLfNode->eid);
            }
        }*/

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
    DEBUG_ASSERT(itRtE == pstENode->vRtE.end());

    return iEid;
}
#if 0
/*****************
input:
        int iEid
        int iLfEid
        int iRtEid
description:
        add new edge
        init stage
******************/
int myG::addLsNeib(int iEid, int iLfEid, int iRtEid)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    //TPST_MAP_BY_EID *pstLfNode = NULL;
    //TPST_MAP_BY_EID *pstRtNode = NULL;
    TPST_LS_BY_EID stlsNode = {0};
    //TPST_LS_BY_EID stlsLfNode = {0};
    //TPST_LS_BY_EID stlsRtNode = {0};
    list<TPST_LS_BY_EID>::iterator pstls;
    //list<TPST_LS_BY_EID>::iterator pstlsLf;
    //list<TPST_LS_BY_EID>::iterator pstlsRt;

    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    /*pstLfNode = findNode(iLfEid);
    DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = findNode(iRtEid);
    DEBUG_ASSERT(NULL != pstRtNode);*/

    pstNode->lsLcNebE.push_front(stlsNode);
    pstls = pstNode->lsLcNebE.begin();
    /*pstNode->lsLcNebE.erase(pstls);
    pstNode->lsLcNebE.emplace_front();
    pstls = pstNode->lsLcNebE.begin();*/
    /*pstLfNode->lsLcNebE.push_front(stlsLfNode);
    pstlsLf = pstLfNode->lsLcNebE.begin();
    pstRtNode->lsLcNebE.push_front(stlsRtNode);
    pstlsRt = pstRtNode->lsLcNebE.begin();*/

    pstls->iLfEid = iLfEid;
    pstls->iRtEid = iRtEid;
    //pstls->pstLfN = pstlsLf;
    //pstls->pstRtN = pstlsRt;

    /*pstlsLf->iLfEid = iRtEid;
    pstlsLf->iRtEid = iEid;
    pstlsLf->pstLfN = pstlsRt;
    pstlsLf->pstRtN = pstls;

    pstlsRt->iLfEid = iEid;
    pstlsRt->iRtEid = iLfEid;
    pstlsRt->pstLfN = pstls;
    pstlsRt->pstRtN = pstlsLf;*/

    return 0;
}
/*****************
input:
        int iEid
        TPST_LS_BY_EID *pstls
description:
        add new edge
        init stage
******************/
int myG::rmLsNeib(int iEid, list<TPST_LS_BY_EID>::iterator pstls)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    TPST_MAP_BY_EID *pstLfNode = NULL;
    TPST_MAP_BY_EID *pstRtNode = NULL;

    DEBUG_ASSERT(0);

    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    /*pstLfNode = findNode(pstls->iLfEid);
    DEBUG_ASSERT(NULL != pstLfNode);
    pstRtNode = findNode(pstls->iRtEid);
    DEBUG_ASSERT(NULL != pstRtNode);

    pstLfNode->lsLcNebE.erase(pstls->pstLfN);
    pstRtNode->lsLcNebE.erase(pstls->pstRtN);*/

    pstNode->lsLcNebE.erase(pstls);

    return 0;
}
/*****************
input:
        int iEid
description:
        add new edge
        init stage
******************/
int myG::rmLsNeib(int iEid)
{
    TPST_MAP_BY_EID *pstNode = NULL;
    list <TPST_LS_BY_EID>::iterator itLcE;

    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);

    itLcE = pstNode->lsLcNebE.begin();
    while (itLcE != pstNode->lsLcNebE.end())
    {
        rmLsNeib(iEid, itLcE++);
    }

    return 0;
}
#endif
/*****************
input:
        none
description:
        init object
        init stage
******************/
myG::myG()
{
    TPST_MAP_BY_EID stTp = {0};

    //m_mpBasicG.clear();
    /*m_vG.clear();
    m_vPNeCnt.clear();*/

    m_pvPNeCnt = new vector<int>();
    m_pvG = new vector<TPST_MAP_BY_EID>();

    m_iMaxPId = 0;
    m_iMaxEId = 0;

    m_pvG->push_back(stTp);
}
/*****************
input:
        none
description:
        init after add nodes
******************/
int myG::init()
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    int x = 0;
    int y = 0;

    for (int vid = 1; vid <= m_iMaxPId; ++vid)
    {
        sort(m_vAdj[vid].begin(), m_vAdj[vid].end(),
            [](const TPST_ADJ& e1, const TPST_ADJ& e2) {
            return e1.iPid < e2.iPid;
            });
    }
    /*for (int eid = 1; eid <= m_iMaxEId; ++eid)
    {
        pstNode = findNode(eid);
        DEBUG_ASSERT(NULL != pstNode);
        x = pstNode->paXY.first;
        y = pstNode->paXY.second;

        pstNode->vLfE.clear();
        pstNode->vRtE.clear();
        findNeb(x, y, pstNode->vLfE, pstNode->vRtE);
    }*/
    return 0;
}
/*****************
input:
        none
description:
        init after add nodes
******************/
int myG::init(int iEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    int x = 0;
    int y = 0;
    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    x = pstNode->paXY.first;
    y = pstNode->paXY.second;

    /*DEBUG_PRINTF("INIT (%d, %d) k: %d layer: %d seSup: %d\n",
                 pstNode->paXY.first, pstNode->paXY.second,
                 pstNode->iTrussness, pstNode->iLayer,
                 pstNode->iSeSup);*/

    pstNode->vLfE.clear();
    pstNode->vRtE.clear();
    findNeb(x, y, pstNode->vLfE, pstNode->vRtE);
    pstNode->bInit = true;
    ++g_lInitCnt;
    return 0;
}
/*****************
input:
        myG &oInitG
description:
        copy map
        calculate stage
******************/
myG::~myG()
{
    //m_mpBasicG.clear();
    m_vAdj.clear();

    m_iMaxPId = 0;
    m_iMaxEId = 0;

    m_pvPNeCnt->clear();

    m_pvG->clear();

    delete m_pvPNeCnt;
    delete m_pvG;
}

#if 0
/*****************
input:
        int x
        int y
description:
        delete edge
        calculate stage
******************/
bool myG::simpleRm(int x, int y)
{
    MAP_BASIC_G::iterator itGNode;
    TPST_MAP_BY_EID stTp = {0};
    int iPrevMaxPid = 0;
    int iEid = 0;

    itGNode = m_mpBasicG.find(pair<int, int>(x, y));
    if (itGNode == m_mpBasicG.end())
    {
        /* have removed, ignore */
        printf("DEBUG has removed\n");
        return false;
    }
    iEid = itGNode->second;
    m_mpBasicG.erase(pair<int, int>(x, y));
    m_mpBasicG.erase(pair<int, int>(y, x));
    (*m_pvPNeCnt)[x]--;
    (*m_pvPNeCnt)[y]--;

    return true;
}
/*****************
input:
        int iEid
description:
        update jusup
******************/
int myG::updateJuSup(int iEid)
{
    TPST_MAP_BY_EID* pstENode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;
    int iJuSup = 0;
    int iMinT = 0;

    pstENode = findNode(iEid);
    debug_assert(NULL != pstENode);

    itLfE = pstENode->vLfE.begin();
    itRtE = pstENode->vRtE.begin();
    for (; itLfE != pstENode->vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLfNode = findNode(*itLfE);
        debug_assert(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        debug_assert(NULL != pstRtNode);

        iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        if (pstENode->iTrussness <= iMinT)
        {
            ++iJuSup;
        }
    }
    DEBUG_ASSERT(itRtE == pstENode->vRtE.end());

    pstENode->iJuSup = iJuSup;

    return 0;
}
/*****************
input:
        int iEid
description:
        update SeSup
******************/
int myG::updateSeSup(int iEid)
{
    TPST_MAP_BY_EID* pstENode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;
    int iSeSup = 0;
    int iMinT = 0;
    int iMinLayer = 0;

    pstENode = findNode(iEid);
    debug_assert(NULL != pstENode);

    itLfE = pstENode->vLfE.begin();
    itRtE = pstENode->vRtE.begin();
    for (; itLfE != pstENode->vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLfNode = findNode(*itLfE);
        debug_assert(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        debug_assert(NULL != pstRtNode);

        iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        if (pstENode->iTrussness < iMinT)
        {
            ++iSeSup;
        }
        else if (pstENode->iTrussness == iMinT)
        {
            iMinLayer = pstENode->iLayer;
            if (pstENode->iTrussness == pstLfNode->iTrussness)
            {
                iMinLayer = COMMON_MIN(iMinLayer, pstLfNode->iLayer);
            }
            if (pstENode->iTrussness == pstRtNode->iTrussness)
            {
                iMinLayer = COMMON_MIN(iMinLayer, pstRtNode->iLayer);
            }
            if (pstENode->iLayer <= iMinLayer)
            {
                ++iSeSup;
            }
        }
    }
    DEBUG_ASSERT(itRtE == pstENode->vRtE.end());

    pstENode->iSeSup = iSeSup;

    return 0;
}
/*****************
input:
        int iEid
description:
        update SeSup
******************/
int myG::checkSeSup(int iEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;
    int iSeSup = 0;
    int iMinT = 0;
    int iMinLayer = 0;
    bool bFlag = false;

    pstNode = findNode(iEid);
    debug_assert(NULL != pstNode);

    itLfE = pstNode->vLfE.begin();
    itRtE = pstNode->vRtE.begin();
    for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLfNode = findNode(*itLfE);
        debug_assert(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        debug_assert(NULL != pstRtNode);

        iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        if (pstNode->iTrussness < iMinT)
        {
            ++iSeSup;
        }
        else if (pstNode->iTrussness == iMinT)
        {
            if (pstNode->bLock && (!pstNode->bRmFlag))
            {
                if ((pstLfNode->bLock || pstLfNode->iTrussness > iMinT) &&
                    (pstRtNode->bLock || pstRtNode->iTrussness > iMinT))
                {
                    ++iSeSup;
                }
            }
            else
            {
                iMinLayer = COMMON_TRG_MIN(pstNode->iLayer,
                                pstLfNode->iLayer,
                                (pstLfNode->bLock || pstLfNode->iTrussness > iMinT),
                                pstRtNode->iLayer,
                                (pstRtNode->bLock || pstRtNode->iTrussness > iMinT));
                if (pstNode->iLayer <= iMinLayer)
                {
                    ++iSeSup;
                }
            }
        }
    }
    DEBUG_ASSERT(itRtE == pstNode->vRtE.end());

    if (pstNode->iSeSup != iSeSup)
    {
        printf("CHECK (%d, %d) k: %d layer: %d seSup: %d %d flag: %d han: %d rm: %d ins: %d\n",
                     pstNode->paXY.first, pstNode->paXY.second,
                     pstNode->iTrussness, pstNode->iLayer,
                     pstNode->iSeSup, iSeSup, pstNode->bLock,
                     pstNode->bHandledFlag, pstNode->bRmFlag, pstNode->bInsLFlag);
        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            pstLfNode = findNode(*itLfE);
            debug_assert(NULL != pstLfNode);
            pstRtNode = findNode(*itRtE);
            debug_assert(NULL != pstRtNode);

            bFlag = false;

            iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
            if (pstNode->iTrussness == iMinT)
            {
                if (pstNode->bLock && (!pstNode->bRmFlag))
                {
                    if ((pstLfNode->bLock || pstLfNode->iTrussness > iMinT) &&
                        (pstRtNode->bLock || pstRtNode->iTrussness > iMinT))
                    {
                        DEBUG_PRINTF("ERROR 1: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d lock: %d %d self: (%d, %d) layer: %d flag: %d\n",
                                     pstLfNode->paXY.first, pstLfNode->paXY.second,
                                     pstRtNode->paXY.first, pstRtNode->paXY.second,
                                     pstLfNode->iTrussness, pstRtNode->iTrussness,
                                     pstLfNode->iLayer, pstRtNode->iLayer,
                                     pstLfNode->iSeSup, pstRtNode->iSeSup,
                                     pstLfNode->bLock, pstRtNode->bLock,
                                     pstNode->paXY.first, pstNode->paXY.second,
                                     pstNode->iLayer, pstNode->bLock);
                        bFlag = true;
                    }
                }
                else
                {
                    iMinLayer = COMMON_TRG_MIN(pstNode->iLayer,
                                    pstLfNode->iLayer,
                                    (pstLfNode->bLock || pstLfNode->iTrussness > iMinT),
                                    pstRtNode->iLayer,
                                    (pstRtNode->bLock || pstRtNode->iTrussness > iMinT));
                    if (pstNode->iLayer <= iMinLayer)
                    {
                        DEBUG_PRINTF("ERROR 2: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d lock: %d %d self: (%d, %d) layer: %d flag: %d\n",
                                     pstLfNode->paXY.first, pstLfNode->paXY.second,
                                     pstRtNode->paXY.first, pstRtNode->paXY.second,
                                     pstLfNode->iTrussness, pstRtNode->iTrussness,
                                     pstLfNode->iLayer, pstRtNode->iLayer,
                                     pstLfNode->iSeSup, pstRtNode->iSeSup,
                                     pstLfNode->bLock, pstRtNode->bLock,
                                     pstNode->paXY.first, pstNode->paXY.second,
                                     pstNode->iLayer, pstNode->bLock);
                        bFlag = true;
                    }
                }
            }
            if (!bFlag)
            {
                DEBUG_PRINTF("ERROR 3: (%d, %d) (%d, %d) k: %d %d layer: %d %d seSup: %d %d lock: %d %d self: (%d, %d) layer: %d flag: %d\n",
                             pstLfNode->paXY.first, pstLfNode->paXY.second,
                             pstRtNode->paXY.first, pstRtNode->paXY.second,
                             pstLfNode->iTrussness, pstRtNode->iTrussness,
                             pstLfNode->iLayer, pstRtNode->iLayer,
                             pstLfNode->iSeSup, pstRtNode->iSeSup,
                             pstLfNode->bLock, pstRtNode->bLock,
                             pstNode->paXY.first, pstNode->paXY.second,
                             pstNode->iLayer, pstNode->bLock);
            }
        }
        DEBUG_ASSERT(0);
    }

    return 0;
}
/*****************
input:
        int iEid
description:
        show SeSup
******************/
int myG::showSeSup(int iEid)
{
    TPST_MAP_BY_EID* pstENode = NULL;
    TPST_MAP_BY_EID* pstLfNode = NULL;
    TPST_MAP_BY_EID* pstRtNode = NULL;
    vector <int>::iterator itLfE;
    vector <int>::iterator itRtE;
    int iSeSup = 0;
    int iMinT = 0;
    int iMinLayer = 0;

    pstENode = findNode(iEid);
    debug_assert(NULL != pstENode);

    itLfE = pstENode->vLfE.begin();
    itRtE = pstENode->vRtE.begin();
    for (; itLfE != pstENode->vLfE.end(); ++itLfE, ++itRtE)
    {
        pstLfNode = findNode(*itLfE);
        debug_assert(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        debug_assert(NULL != pstRtNode);

        iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        if (pstENode->iTrussness < iMinT)
        {
            ++iSeSup;
            printf("SHOW k: %d (%d, %d) (%d, %d)\n", iMinT,
                   pstLfNode->paXY.first, pstLfNode->paXY.second,
                   pstRtNode->paXY.first, pstRtNode->paXY.second);
        }
        else if (pstENode->iTrussness == iMinT)
        {
            iMinLayer = pstENode->iLayer;
            if (pstENode->iTrussness == pstLfNode->iTrussness)
            {
                iMinLayer = COMMON_MIN(iMinLayer, pstLfNode->iLayer);
            }
            if (pstENode->iTrussness == pstRtNode->iTrussness)
            {
                iMinLayer = COMMON_MIN(iMinLayer, pstRtNode->iLayer);
            }
            if (pstENode->iLayer <= iMinLayer)
            {
                ++iSeSup;
                printf("SHOW k: %d (%d, %d) (%d, %d)\n", iMinT,
                       pstLfNode->paXY.first, pstLfNode->paXY.second,
                       pstRtNode->paXY.first, pstRtNode->paXY.second);
            }
        }
    }
    DEBUG_ASSERT(itRtE == pstENode->vRtE.end());

    pstENode->iSeSup = iSeSup;

    return 0;
}

/*****************
input:
        int x
        int y
        list<int> &lstNeib
description:
        delete edge
        calculate stage
******************/
int myG::findNeb(int x, int y, vector<int> &vNeib)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
    }

    debug_assert(vNeib.empty());
    //printf("MYG get (%d, %d) u: %d\n", x, y, u);
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {

        w = itLowB->first.second;
        //printf("MYG w: %d\n", w);
        if (m_mpBasicG.find(pair<int, int>(v, w)) != m_mpBasicG.end())
        {
            vNeib.push_back(w);
            iNebCnt++;
        }
        itLowB++;
    }
    return iNebCnt;
}
/*****************
input:
        int x
        int y
        vector<int> &vLfE
        vector<int> &vRtE
description:
        find neighbor eid
******************/
int myG::findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    vector<int> * pvLfE = &vLfE;
    vector<int> * pvRtE = &vRtE;

    DEBUG_ASSERT(x < y);
    DEBUG_ASSERT(vLfE.empty());
    DEBUG_ASSERT(vRtE.empty());

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
        pvLfE = &vRtE;
        pvRtE = &vLfE;
    }

    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        MAP_BASIC_G::iterator itDesE = m_mpBasicG.find(pair<int, int>(v, w));
        if (itDesE != m_mpBasicG.end())
        {
            pvLfE->push_back(itLowB->second);
            pvRtE->push_back(itDesE->second);
            iNebCnt++;
        }
        itLowB++;
    }
    return iNebCnt;
}
#endif
/*****************
input:
        int x
        int y
        vector<int> &vLfE
        vector<int> &vRtE
description:
        find neighbor eid
******************/
int myG::findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE)
{
    size_t p1 = 0, p2 = 0;
    while (p1 < m_vAdj[x].size() && p2 < m_vAdj[y].size())
    {
        if (m_vAdj[x][p1].iPid == m_vAdj[y][p2].iPid)
        {
            vLfE.push_back(m_vAdj[x][p1].iEid);
            vRtE.push_back(m_vAdj[y][p2].iEid);
            ++p1; ++p2;
        }
        else if (m_vAdj[x][p1].iPid < m_vAdj[y][p2].iPid)
        {
            ++p1;
        }
        else
        {
            ++p2;
        }
    }
    return 0;
}
#if 0
/*****************
input:
        int x
        int y
        vector<int> &vLfE
        vector<int> &vRtE
description:
        find neighbor eid
******************/
int myG::findNeb(int x, int y, vector<int> &vNeibP, vector<int> &vLfE, vector<int> &vRtE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    vector<int> * pvLfE = &vLfE;
    vector<int> * pvRtE = &vRtE;

    if (m_iMaxPId < x)
    {
        return 0;
    }
    if (m_iMaxPId < y)
    {
        return 0;
    }

    DEBUG_ASSERT(vLfE.empty());
    DEBUG_ASSERT(vRtE.empty());
    DEBUG_ASSERT(vNeibP.empty());

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
        pvLfE = &vRtE;
        pvRtE = &vLfE;
    }

    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        MAP_BASIC_G::iterator itDesE = m_mpBasicG.find(pair<int, int>(v, w));
        if (itDesE != m_mpBasicG.end())
        {
            vNeibP.push_back(w);
            pvLfE->push_back(itLowB->second);
            pvRtE->push_back(itDesE->second);
            iNebCnt++;
        }
        itLowB++;
    }
    return iNebCnt;
}
/*****************
input:
        int x
        int y
        vector<int> &vLfE
        vector<int> &vRtE
        MAP_BASIC_G &mapCalG
description:
        find neighbor eid
******************/
int myG::findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE, MAP_BASIC_G &mapCalG)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    vector<int> * pvLfE = &vLfE;
    vector<int> * pvRtE = &vRtE;

    DEBUG_ASSERT(x < y);
    DEBUG_ASSERT(vLfE.empty());
    DEBUG_ASSERT(vRtE.empty());

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
        pvLfE = &vRtE;
        pvRtE = &vLfE;
    }

    /* find u's neighbor , note u > 0 */
    itLowB = mapCalG.upper_bound(pair<int, int>(u, 0));
    itHighB = mapCalG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        MAP_BASIC_G::iterator itDesE = mapCalG.find(pair<int, int>(v, w));
        if (itDesE != mapCalG.end())
        {
            pvLfE->push_back(itLowB->second);
            pvRtE->push_back(itDesE->second);
            iNebCnt++;
        }
        itLowB++;
    }
    return iNebCnt;
}
/*****************
input:
        int x
        int y
        list<pair<int, int> > &lspaNeibE
description:
        delete edge
        calculate stage
******************/
int myG::findNebE(int x, int y, list<pair<int, int> > &lspaNeibE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    MAP_BASIC_G::iterator itTrdN;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
    }

    debug_assert(lspaNeibE.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {

        w = itLowB->first.second;
        //printf("MYG w: %d\n", w);
        itTrdN = m_mpBasicG.find(pair<int, int>(v, w));
        if (itTrdN != m_mpBasicG.end())
        {
            lspaNeibE.push_back(pair<int, int>(itLowB->second, itTrdN->second));
            iNebCnt++;
        }
        itLowB++;
    }
    return iNebCnt;
}

/*****************
input:
        int x
        int y
        list<int> &lstNeibE
        MAP_BASIC_G &mapCalG
        MAP_BASIC_G &mapBlackG
        int iTrussness
description:
        delete edge
        calculate stage
******************/
int myG::findNebE(int x, int y, list<int> &lstNeibE, MAP_BASIC_G &mapCalG, MAP_BASIC_G &mapBlackG, int iTrussness)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
    }

    debug_assert(lstNeibE.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        if (mapBlackG.find(itLowB->first) != mapBlackG.end())
        {
            /* in black, ignore */
            itLowB++;
            continue;
        }
        if (m_mpBasicG.find(pair<int, int>(v, w)) != m_mpBasicG.end())
        {
            TPST_MAP_BY_EID* pstENode = NULL;
            TPST_MAP_BY_EID* pstENodeO = NULL;
            int iMinTruss = 0;

            pstENode = this->findNode(u, w);
            debug_assert(NULL != pstENode);
            pstENodeO = this->findNode(v, w);
            debug_assert(NULL != pstENodeO);

            iMinTruss = pstENode->iTrussness<pstENodeO->iTrussness?pstENode->iTrussness:pstENodeO->iTrussness;

            if (iMinTruss != iTrussness)
            {
                /* not in same truss, ignore */
                itLowB++;
                continue;
            }
            if (mapBlackG.find(pair<int, int>(v, w)) != mapBlackG.end())
            {
                /* in black, ignore */
                itLowB++;
                continue;
            }
            /* exists in G, check in calG */
            if (mapCalG.find(pair<int, int>(v, w)) != mapCalG.end())
            {
                lstNeibE.push_back(m_mpBasicG[pair<int, int>(v, w)]);
                iNebCnt++;
            }
            if (mapCalG.find(pair<int, int>(u, w)) != mapCalG.end())
            {
                lstNeibE.push_back(m_mpBasicG[pair<int, int>(u, w)]);
                iNebCnt++;
            }
        }
        itLowB++;
    }

    return iNebCnt;
}

/*****************
input:
        int iNode
        list<int> &lsNeibP
        list<int> &liThirdE
        int iTrussness
description:
        delete edge
        calculate stage
******************/
int myG::findThirdE(int iNode, list<int> &lsThirdE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    u = iNode;

    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        /* x: min degree */
        int x = 0;
        int y = 0;
        bool bChgFlag = false;
        MAP_BASIC_G::iterator itLcLowB;
        MAP_BASIC_G::iterator itLcHighB;
        MAP_BASIC_G::iterator itDesE;
        v = itLowB->first.second;

        x = u;
        y = v;

        if ((*m_pvPNeCnt)[x] > (*m_pvPNeCnt)[y])
        {
            x = v;
            y = u;
            bChgFlag = true;
        }
        itLcLowB = m_mpBasicG.upper_bound(pair<int, int>(x, 0));
        itLcHighB = m_mpBasicG.upper_bound(pair<int, int>(x + 1, 0));
        while (itLcLowB != itLcHighB)
        {

            w = itLcLowB->first.second;
            if (v <= w)
            {
                /* ignore half */
                itLcLowB++;
                continue;
            }
            itDesE = m_mpBasicG.find(pair<int, int>(y, w));
            if (itDesE != m_mpBasicG.end())
            {
                int iEid = 0;
                if (bChgFlag)
                {
                    itDesE = itLcLowB;
                }
                /*printf("MYG (v, w): (%d, %d), (x, y): (%d, %d) bool %d\n",
                       v, w, x, y, bChgFlag);*/
                iEid = itDesE->second;
                lsThirdE.push_back(iEid);
                ++iCnt;
            }
            itLcLowB++;
        }
        ++itLowB;
    }

    return iCnt;
}

/*****************
input:
        int iNode
        vector <int> &vNeibP
        vector <int> &vThirdE
description:
        delete edge
        calculate stage
******************/
int myG::findPNebPTrdE(int iNodeId, vector <int> &vNeibP, vector <int> &vThirdE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    /* eid, none */
    vector<int> * pvNebE = NULL;
    vector<int>::iterator itE;

    u = iNodeId;

    DEBUG_ASSERT(vNeibP.empty());
    DEBUG_ASSERT(vThirdE.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        /* x: min degree */
        int iEid = 0;
        TPST_MAP_BY_EID* pstENode = NULL;
        v = itLowB->first.second;
        iEid = itLowB->second;
        vNeibP.push_back(v);
        pstENode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstENode);

        if (u < v)
        {
            pvNebE = &(pstENode->vRtE);
        }
        else
        {
            pvNebE = &(pstENode->vLfE);
        }
        for (itE = pvNebE->begin(); itE != pvNebE->end(); ++itE)
        {
            pstENode = findNode(*itE);
            DEBUG_ASSERT(NULL != pstENode);
            if (v != pstENode->paXY.first)
            {
                /* ignore half */
                continue;
            }
            vThirdE.push_back(*itE);
            ++iCnt;
        }
        ++itLowB;
    }

    return iCnt;
}

/*****************
input:
        int iNode
        vector <int> &vNeibP
        vector <int> &vThirdE
description:
        delete edge
        calculate stage
******************/
int myG::findPNebPTrdE(int iNodeId, vector <TPST_ADJ> &vNeibP, vector <int> &vThirdE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    /* eid, none */
    vector<int> * pvNebE = NULL;
    vector<int>::iterator itE;
    TPST_ADJ stPair = {0};

    u = iNodeId;

    DEBUG_ASSERT(vNeibP.empty());
    DEBUG_ASSERT(vThirdE.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        /* x: min degree */
        int iEid = 0;
        TPST_MAP_BY_EID* pstENode = NULL;
        v = itLowB->first.second;
        iEid = itLowB->second;
        stPair.iPid = v;
        stPair.iEid = iEid;
        vNeibP.push_back(stPair);
        pstENode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstENode);

        if (u < v)
        {
            pvNebE = &(pstENode->vRtE);
        }
        else
        {
            pvNebE = &(pstENode->vLfE);
        }
        for (itE = pvNebE->begin(); itE != pvNebE->end(); ++itE)
        {
            pstENode = findNode(*itE);
            DEBUG_ASSERT(NULL != pstENode);
            if (v != pstENode->paXY.first)
            {
                /* ignore half */
                continue;
            }
            vThirdE.push_back(*itE);
            ++iCnt;
        }
        ++itLowB;
    }

    return iCnt;
}
/*****************
input:
        int x
        int y
        list<int> &lstNeibE
        MAP_BASIC_G &mapCalG
description:
        find eid in calG, at least trussness
******************/
int myG::findNebE(int x, int y, list<int> &lstNeibE, MAP_BASIC_G &mapCalG, int iTrussness)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    MAP_BASIC_G::iterator itO;
    int iNebCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;
    int w = 0;

    if ((*m_pvPNeCnt)[x] < (*m_pvPNeCnt)[y])
    {
        u = x;
        v = y;
    }
    else
    {
        u = y;
        v = x;
    }

    debug_assert(lstNeibE.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        itO = m_mpBasicG.find(pair<int, int>(v, w));
        if (itO != m_mpBasicG.end())
        {
            TPST_MAP_BY_EID* pstENode = NULL;
            TPST_MAP_BY_EID* pstENodeO = NULL;
            int iMinTruss = 0;
            pstENode = this->findNode(itLowB->second);
            debug_assert(NULL != pstENode);
            pstENodeO = this->findNode(itO->second);
            debug_assert(NULL != pstENodeO);

            iMinTruss = pstENode->iTrussness<pstENodeO->iTrussness?pstENode->iTrussness:pstENodeO->iTrussness;

            if (iMinTruss < iTrussness)
            {
                /* ignore */
                itLowB++;
                continue;
            }
            /* exists in G, check in calG */
            if (mapCalG.find(pair<int, int>(v, w)) != mapCalG.end())
            {
                lstNeibE.push_back(m_mpBasicG[pair<int, int>(v, w)]);
                iNebCnt++;
            }
            if (mapCalG.find(pair<int, int>(u, w)) != mapCalG.end())
            {
                lstNeibE.push_back(m_mpBasicG[pair<int, int>(u, w)]);
                iNebCnt++;
            }
        }
        itLowB++;
    }

    return iNebCnt;
}

/*****************
input:
        int iNodeId
        list<int> &lstP
        MAP_BASIC_G &mapCalG
description:
        find node's neighbor node
******************/
int myG::findPNebP(int iNodeId, list<int> &lstP, MAP_BASIC_G &mapCalG)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int w = 0;
    TPST_MAP_BY_EID* pstENode = NULL;

    u = iNodeId;

    debug_assert(lstP.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = mapCalG.upper_bound(pair<int, int>(u, 0));
    itHighB = mapCalG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;

        lstP.push_back(w);
        ++iCnt;

        ++itLowB;
    }

    return iCnt;
}
/*****************
input:
        int iNodeId
        list<int> &lstP
description:
        find eid
******************/
int myG::findPNebP(int iNodeId, list<int> &lstP)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int w = 0;
    TPST_MAP_BY_EID* pstENode = NULL;

    u = iNodeId;

    debug_assert(lstP.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;

        lstP.push_back(w);
        ++iCnt;

        ++itLowB;
    }

    return iCnt;
}
/*****************
input:
        int iNodeId
        list<int> &lstP
description:
        find eid
******************/
int myG::findPNebP(int iNodeId, vector <pair<int, int> > &vNeibP)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    /* nei(u) < nei(v) */
    int u = 0;
    int w = 0;
    TPST_MAP_BY_EID* pstENode = NULL;

    u = iNodeId;

    DEBUG_ASSERT(vNeibP.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        int iEid = itLowB->second;

        vNeibP.push_back(pair<int, int>(w, iEid));
        ++iCnt;

        ++itLowB;
    }

    return iCnt;
}

#endif
/*****************
input:
        int x
        int y
description:
        insert y into m_vAdj[x]
        return position
******************/
int myG::adjInsert(int x, int y, int eid)
{
    /* binary search position */
    int iSize = m_vAdj[x].size();
    int iMin = 0;
    int iMax = iSize;
    int iPos = iSize / 2;
    while (iMin < iMax)
    {
        if (m_vAdj[x][iPos].iPid > y)
        {
            iMax = iPos;
            iPos = (iMin + iMax) / 2;
        }
        else if (m_vAdj[x][iPos].iPid < y)
        {
            iMin = iPos;
            iPos = (iMin + iMax) / 2;
            if (iPos <= iMin)
            {
                /* not found */
                ++iPos;
                break;
            }
        }
        else
        {
            /* found */
            break;
        }
    }

    if (iPos < iSize)
    {
        m_vAdj[x].insert(m_vAdj[x].begin() + iPos, {y, eid});
    }
    else
    {
        m_vAdj[x].push_back({y, eid});
    }
    return iPos;
}
/*****************
input:
        int x
        int y
description:
        find node
******************/
TPST_MAP_BY_EID * myG::findNode(int x, int y)
{
    if (y > m_iMaxPId)
    {
        /* not found */
        return NULL;
    }
    //DEBUG_PRINTF("MY_G size: %d max: %d\n", m_pvPNeCnt->size(), m_iMaxPId);
    DEBUG_ASSERT(m_iMaxPId < m_pvPNeCnt->size());
    int u = x;
    int v = y;
    if (m_pvPNeCnt->at(u) > m_pvPNeCnt->at(v))
    {
        u = y;
        v = x;
    }
    int iSize = m_pvPNeCnt->at(u);
    int iMin = 0;
    int iMax = iSize;
    int iPos = iSize / 2;
    int iEid = 0;
    while (iMin < iMax)
    {
        if (m_vAdj[u][iPos].iPid > v)
        {
            iMax = iPos;
            iPos = (iMin + iMax) / 2;
        }
        else if (m_vAdj[u][iPos].iPid < v)
        {
            iMin = iPos;
            iPos = (iMin + iMax) / 2;
            if (iPos <= iMin)
            {
                /* not found */
                break;
            }
        }
        else
        {
            /* found */
            iEid = m_vAdj[u][iPos].iEid;
            break;
        }
    }
    if (0 == iEid)
    {
        /* not found */
        return NULL;
    }
    return findNode(iEid);
}
/*****************
input:
        int eid
description:
        find node
******************/
TPST_MAP_BY_EID * myG::findNode(int eid)
{
    if (!((0 < eid) && (eid <= m_iMaxEId)))
    {
        printf("MYG ERROR eid: %d maxEid: %d\n",
               eid, m_iMaxEId);
        DEBUG_ASSERT(0);
    }
    return &((*m_pvG)[eid]);
}













