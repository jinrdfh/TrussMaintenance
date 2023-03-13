/***************
my class G
****************/

#include "common.h"
#include "myG.h"

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

    m_pvPNe = new vector<map<int, int> >();
    m_pvPNeCnt = new vector<int>();
    m_pvG = new vector<TPST_MAP_BY_EID>();

    m_iMaxPId = 0;
    m_iMaxEId = 0;

    m_pvG->push_back(stTp);
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

    m_iMaxPId = 0;
    m_iMaxEId = 0;

    m_pvPNe->clear();
    m_pvPNeCnt->clear();

    m_pvG->clear();

    delete m_pvPNe;
    delete m_pvPNeCnt;
    delete m_pvG;
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
/*****************
input:
        int iEid
description:
        rm an edge
******************/
TPST_MAP_BY_EID *myG::add(int iEid)
{
    TPST_MAP_BY_EID stTpNode = {0};
    TPST_MAP_BY_EID *pstNode = NULL;
    stTpNode.eid = iEid;
    m_pvG->push_back(stTpNode);
    pstNode = &((*(m_pvG))[iEid]);
    DEBUG_ASSERT(iEid == pstNode->eid);

    pstNode->bDgdFlag = false;
    pstNode->bInit = false;
    return pstNode;
}
#if 0
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
    int u = 0;
    int v = 0;
    int iPrevMaxPid = m_iMaxPId;
    TPST_MAP_BY_EID *pstNode = NULL;
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
        m_pvPNe->resize(m_iMaxPId + 1);
        m_pvPNeCnt->resize(m_iMaxPId + 1);
    }

    u = x;
    v = y;
    if ((*m_pvPNeCnt)[x] > (*m_pvPNeCnt)[y])
    {
        u = y;
        v = x;
    }

    if ((*m_pvPNe)[u].find(v) != (*m_pvPNe)[u].end())
    {
        /* record */
        return 0;
    }

    m_iMaxEId++;
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
    (*m_pvPNe)[u][v] = m_iMaxEId;
    (*m_pvPNe)[v][u] = m_iMaxEId;

    TPST_MAP_BY_EID stTp = {0};

    /*MAP_BASIC_G::iterator itGNode;

    itGNode = m_mpBasicG.find(pair<int, int>(x, y));
    if (itGNode != m_mpBasicG.end())
    {
        printf("DEBUG has joined\n");
        return 0;
    }

    m_mpBasicG[pair<int, int>(x, y)] = m_iMaxEId;
    m_mpBasicG[pair<int, int>(y, x)] = m_iMaxEId;*/

    stTp.eid = m_iMaxEId;
    stTp.paXY = myG_getPair(x, y);

    m_pvG->push_back(stTp);

    /* check right position */
    DEBUG_ASSERT(m_iMaxEId == (*m_pvG)[m_iMaxEId].eid);

    pstNode = &((*(m_pvG))[m_iMaxEId]);

    pstNode->iSup = 0;

    return m_iMaxEId;
}
#endif
/*****************
input:
        int iEid
description:
        rm an edge
******************/
int myG::rm(int iEid)
{
    int x = 0;
    int y = 0;
    TPST_MAP_BY_EID* pstNode = NULL;

    DEBUG_ASSERT(0 != iEid);
    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);

    x = pstNode->paXY.first;
    y = pstNode->paXY.second;

    /*m_mpBasicG.erase(pair<int, int>(x, y));
    m_mpBasicG.erase(pair<int, int>(y, x));*/
    (*m_pvPNe)[x].erase(y);
    (*m_pvPNe)[y].erase(x);

    pstNode->eid = 0;

    (*m_pvPNeCnt)[x]--;
    (*m_pvPNeCnt)[y]--;

    return 0;
}
#if 0
/*****************
input:
        myG &mpG
        int iEid
description:
        simply add
******************/
int myG::addNeibTri(int iEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    vector<int> vLfE;
    vector<int> vRtE;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    int x = 0;
    int y = 0;
    int iMinT = 0;
    int iMinL = 0;

    /* add and init */
    DEBUG_ASSERT(0 != iEid);
    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    x = pstNode->paXY.first;
    y = pstNode->paXY.second;

    (*m_pvPNeCnt)[x]++;
    (*m_pvPNeCnt)[y]++;

    (*m_pvPNe)[x][y] = iEid;
    (*m_pvPNe)[y][x] = iEid;

    pstNode->iKSup = 0;

    findNeb(x, y, vLfE, vRtE);

    itLfE = vLfE.begin();
    itRtE = vRtE.begin();
    for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
    {
        TPST_MAP_BY_EID* pstLfNode = NULL;
        TPST_MAP_BY_EID* pstRtNode = NULL;

        pstLfNode = findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRtNode);

        pstNode->mpLfE[pstLfNode->eid] = pstRtNode->eid;
        pstNode->mpRtE[pstRtNode->eid] = pstLfNode->eid;

        if (pstLfNode->paXY.first == x)
        {
            /* z > x */
            /*pstLfNode->vLfE.push_back(iEid);
            pstLfNode->vRtE.push_back(*itRtE);*/

            pstLfNode->mpLfE[iEid] = pstRtNode->eid;
            pstLfNode->mpRtE[pstRtNode->eid] = iEid;
        }
        else
        {
            /* z < x */
            /*pstLfNode->vLfE.push_back(*itRtE);
            pstLfNode->vRtE.push_back(iEid);*/

            pstLfNode->mpRtE[iEid] = pstRtNode->eid;
            pstLfNode->mpLfE[pstRtNode->eid] = iEid;
        }
        if (pstRtNode->paXY.first == y)
        {
            /* z > y */
            /*pstRtNode->vLfE.push_back(iEid);
            pstRtNode->vRtE.push_back(*itLfE);*/

            pstRtNode->mpLfE[iEid] = pstLfNode->eid;
            pstRtNode->mpRtE[pstLfNode->eid] = iEid;
        }
        else
        {
            /* z < x */
            /*pstRtNode->vLfE.push_back(*itLfE);
            pstRtNode->vRtE.push_back(iEid);*/

            pstRtNode->mpRtE[iEid] = pstLfNode->eid;
            pstRtNode->mpLfE[pstLfNode->eid] = iEid;
        }
        iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        iMinT = COMMON_MIN(iMinT, pstNode->iTrussness);
        iMinL = INT_MAX;
        if (iMinT == pstNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstNode->iLayer);
        }
        if (iMinT == pstLfNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstLfNode->iLayer);
        }
        if (iMinT == pstRtNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstRtNode->iLayer);
        }

        if (iMinT == pstNode->iTrussness)
        {
            pstNode->iKSup++;
            if (iMinL == pstNode->iLayer)
            {
                pstNode->iKMSup++;
            }
            else if (iMinL == pstNode->iLayer - 1)
            {
                pstNode->iKMSup++;
            }
        }

        if (iMinT == pstLfNode->iTrussness)
        {
            pstLfNode->iKSup++;
            if (iMinL == pstLfNode->iLayer)
            {
                pstLfNode->iKMSup++;
            }
            else if (iMinL == pstLfNode->iLayer - 1)
            {
                pstLfNode->iKMSup++;
            }
        }

        if (iMinT == pstRtNode->iTrussness)
        {
            pstRtNode->iKSup++;
            if (iMinL == pstRtNode->iLayer)
            {
                pstRtNode->iKMSup++;
            }
            else if (iMinL == pstRtNode->iLayer - 1)
            {
                pstRtNode->iKMSup++;
            }
        }
    }
    DEBUG_ASSERT(itRtE == vRtE.end());

    return iEid;
}
#endif
/*****************
input:
        myG &mpG
        int iEid
description:
        simply add
******************/
int myG::init(int iEid)
{
    TPST_MAP_BY_EID* pstNode = NULL;
    vector<int> vLfE;
    vector<int> vRtE;
    vector<int>::iterator itLfE;
    vector<int>::iterator itRtE;
    int x = 0;
    int y = 0;
    int iMinT = 0;
    int iMinL = 0;

    /* add and init */
    DEBUG_ASSERT(0 != iEid);
    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    x = pstNode->paXY.first;
    y = pstNode->paXY.second;

    pstNode->iKSup = 0;
    pstNode->iKMSup = 0;
    pstNode->bInit = true;

    findNeb(x, y, vLfE, vRtE);

    itLfE = vLfE.begin();
    itRtE = vRtE.begin();
    for (; itLfE != vLfE.end(); ++itLfE, ++itRtE)
    {
        TPST_MAP_BY_EID* pstLfNode = NULL;
        TPST_MAP_BY_EID* pstRtNode = NULL;

        pstLfNode = findNode(*itLfE);
        DEBUG_ASSERT(NULL != pstLfNode);
        pstRtNode = findNode(*itRtE);
        DEBUG_ASSERT(NULL != pstRtNode);

        pstNode->mpLfE[pstLfNode->eid] = pstRtNode->eid;
        //pstNode->mpRtE[pstRtNode->eid] = pstLfNode->eid;

        iMinT = COMMON_MIN(pstLfNode->iTrussness, pstRtNode->iTrussness);
        iMinT = COMMON_MIN(iMinT, pstNode->iTrussness);
        iMinL = INT_MAX;
        if (iMinT == pstNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstNode->iLayer);
        }
        if (iMinT == pstLfNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstLfNode->iLayer);
        }
        if (iMinT == pstRtNode->iTrussness)
        {
            iMinL = COMMON_MIN(iMinL, pstRtNode->iLayer);
        }

        if (iMinT == pstNode->iTrussness)
        {
            pstNode->iKSup++;
            if (iMinL == pstNode->iLayer)
            {
                pstNode->iKMSup++;
            }
            else if (iMinL == pstNode->iLayer - 1)
            {
                pstNode->iKMSup++;
            }
        }
    }
    //DEBUG_ASSERT(itRtE == vRtE.end());
    /*DEBUG_PRINTF("INIT (%d, %d) k: %d layer: %d KMSup: %d\n",
           pstNode->paXY.first, pstNode->paXY.second, pstNode->iTrussness,
           pstNode->iLayer, pstNode->iKMSup);*/

    return iEid;
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
    map<int, int>::iterator itP;
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

    for (itP = (*m_pvPNe)[u].begin(); itP != (*m_pvPNe)[u].end(); ++itP)
    {
        w = itP->first;
        map<int, int>::iterator itDesP = (*m_pvPNe)[v].find(w);
        if (itDesP != (*m_pvPNe)[v].end())
        {
            pvLfE->push_back(itP->second);
            pvRtE->push_back(itDesP->second);
            iNebCnt++;
        }
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
int myG::findHalfNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE)
{
    map<int, int>::iterator itP;
    map<int, int>::iterator itDesP;
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
    itP = (*m_pvPNe)[u].upper_bound(y);
    for (; itP != (*m_pvPNe)[u].end(); ++itP)
    {
        w = itP->first;

        itDesP = (*m_pvPNe)[v].find(w);
        if (itDesP != (*m_pvPNe)[v].end())
        {
            pvLfE->push_back(itP->second);
            pvRtE->push_back(itDesP->second);
        }
    }
    return 0;
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
int myG::findNeb(int iEid, vector<int> &vLfE, vector<int> &vRtE)
{
    int iNebCnt = 0;
    TPST_MAP_BY_EID* pstNode = NULL;

    DEBUG_ASSERT(vLfE.empty());
    DEBUG_ASSERT(vRtE.empty());

    pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);

    for (auto atE : pstNode->mpLfE)
    {
        vLfE.push_back(atE.first);
        vRtE.push_back(atE.second);
        ++iNebCnt;
    }
    return iNebCnt;
}

/*****************
input:
        int iNode
        vector <pair<int, int> > &vNeibP
        vector <int> &vThirdE
description:
        delete edge
        calculate stage
******************/
int myG::findPNebPTrdE(int iNodeId, vector <pair<int, int> > &vNeibP, vector <int> &vThirdE)
{
    map<int, int>::iterator itP;
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
    for (itP = (*m_pvPNe)[u].begin(); itP != (*m_pvPNe)[u].end(); ++itP)
    {
        /* x: min degree */
        int iEid = 0;
        TPST_MAP_BY_EID* pstNode = NULL;
        v = itP->first;
        iEid = itP->second;
        vNeibP.push_back(pair<int, int>(v, iEid));
        pstNode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);

        vector<int> vLfE;
        vector<int> vRtE;
        findNeb(iEid, vLfE, vRtE);

        if (u < v)
        {
            pvNebE = &(vRtE);
        }
        else
        {
            pvNebE = &(vLfE);
        }
        for (itE = pvNebE->begin(); itE != pvNebE->end(); ++itE)
        {
            pstNode = findNode(*itE);
            DEBUG_ASSERT(NULL != pstNode);
            if (v != pstNode->paXY.first)
            {
                /* ignore half */
                continue;
            }
            vThirdE.push_back(*itE);
            ++iCnt;
        }
    }
    return iCnt;
}

/*****************
input:
        int iNodeId
        list<int> &lstP
        MAP_BASIC_G &mapCalG
description:
        find node's neighbor node
******************/
int myG::findPNebP(int iNodeId, vector <pair<int, int> > &vNeibP, MAP_BASIC_G &mapCalG)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iCnt = 0;
    int u = 0;
    int w = 0;

    u = iNodeId;

    DEBUG_ASSERT(vNeibP.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = mapCalG.upper_bound(pair<int, int>(u, 0));
    itHighB = mapCalG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        w = itLowB->first.second;
        int iCurK = itLowB->second;

        vNeibP.push_back(pair<int, int>(w, iCurK));
        ++iCnt;

        ++itLowB;
    }

    return iCnt;
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
    map<int, int>::iterator itP;
    /* nei(u) < nei(v) */
    int u = 0;
    int v = 0;

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
    int eid = 0;

    itP = (*m_pvPNe)[u].find(v);
    if (itP == (*m_pvPNe)[u].end())
    {
        printf("ERROR MYG no edge (%d, %d)\n", x, y);
        DEBUG_ASSERT(0);
        return NULL;
    }
    eid = itP->second;
    //printf("MYG (%d, %d), eid: %d\n", x, y, eid);
    return findNode(eid);
}







