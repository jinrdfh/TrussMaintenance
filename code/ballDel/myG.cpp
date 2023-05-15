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
    m_iInitCnt = 0;
    for (int vid = 1; vid <= m_iMaxPId; ++vid)
    {
        sort(m_vAdj[vid].begin(), m_vAdj[vid].end(),
            [](const AdjEntry& e1, const AdjEntry& e2) {
            return e1.pid < e2.pid;
            });
    }

    // init iKSup iKMSup
    TPST_MAP_BY_EID* pstNode = NULL;
    vector<uint32_t>::iterator itLfE;
    vector<uint32_t>::iterator itRtE;
    int x = 0;
    int y = 0;
    int iMinT = 0;
    int iMinL = 0;

    for (int iEid = 1; iEid <= m_iMaxEId; ++iEid)
    {
        /* add and init */
        DEBUG_ASSERT(0 != iEid);
        pstNode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        DEBUG_ASSERT(iEid == pstNode->eid);
        x = pstNode->paXY.first;
        y = pstNode->paXY.second;

        pstNode->iKSup = 0;
        pstNode->iKMSup = 0;

        pstNode->vLfE.clear();
        pstNode->vRtE.clear();
        findNeb(x, y, pstNode->vLfE, pstNode->vRtE);

        pstNode->vKLfE.resize(pstNode->vLfE.size());
        pstNode->vKRtE.resize(pstNode->vRtE.size());

        itLfE = pstNode->vLfE.begin();
        itRtE = pstNode->vRtE.begin();
        for (; itLfE != pstNode->vLfE.end(); ++itLfE, ++itRtE)
        {
            TPST_MAP_BY_EID* pstLfNode = NULL;
            TPST_MAP_BY_EID* pstRtNode = NULL;

            pstLfNode = findNode(*itLfE);
            DEBUG_ASSERT(NULL != pstLfNode);
            pstRtNode = findNode(*itRtE);
            DEBUG_ASSERT(NULL != pstRtNode);

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
    }

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
    m_pvG->clear();
    m_vAdj.clear();

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
    //pstNode->bInit = false;
    pstNode->bLInit = false;
    return pstNode;
}

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

    /* remove adj information */
    vector<AdjEntry>::iterator it = lower_bound(m_vAdj[x].begin(), m_vAdj[x].end(), y,
              [](const AdjEntry& e, int tpPid) {
                return e.pid < tpPid;
              });
    if ((m_vAdj[x].end() != it) && (it->pid == y))
    {
        m_vAdj[x].erase(it);
    }
    else
    {
        DEBUG_ASSERT(0);
    }
    it = lower_bound(m_vAdj[y].begin(), m_vAdj[y].end(), x,
              [](const AdjEntry& e, int tpPid) {
                return e.pid < tpPid;
              });
    if ((m_vAdj[y].end() != it) && (it->pid == x))
    {
        m_vAdj[y].erase(it);
    }
    else
    {
        DEBUG_ASSERT(0);
    }

    pstNode->eid = 0;

    return 0;
}
# if 0
/*****************
input:
        myG &mpG
        int iEid
description:
        simply add
******************/
int myG::init(int iEid)
{
    ++m_iInitCnt;

    /* add and init */
    TPST_MAP_BY_EID* pstNode = findNode(iEid);
    DEBUG_ASSERT(NULL != pstNode);
    int x = pstNode->paXY.first;
    int y = pstNode->paXY.second;

    pstNode->bInit = true;

    int iMinD = COMMON_MIN(m_vAdj[x].size(), m_vAdj[y].size());
    pstNode->vLfE.clear();
    pstNode->vRtE.clear();
    pstNode->vLfE.reserve(iMinD);
    pstNode->vRtE.reserve(iMinD);
    findNeb(x, y, pstNode->vLfE, pstNode->vRtE);

    return iEid;
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
/*int myG::findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE)
{
    size_t p1 = 0, p2 = 0;
    while (p1 < m_vAdj[x].size() && p2 < m_vAdj[y].size())
    {
        if (m_vAdj[x][p1].pid == m_vAdj[y][p2].pid)
        {
            vLfE.push_back(m_vAdj[x][p1].eid);
            vRtE.push_back(m_vAdj[y][p2].eid);
            ++p1; ++p2;
        }
        else if (m_vAdj[x][p1].pid < m_vAdj[y][p2].pid)
        {
            ++p1;
        }
        else
        {
            ++p2;
        }
    }

    return vLfE.size();
}*/
int myG::findNeb(int x, int y, vector<uint32_t> &vLfE, vector<uint32_t> &vRtE)
{
    int u = x;
    int v = y;
    if (m_vAdj[x].size() > m_vAdj[y].size())
    {
        u = y;
        v = x;
    }
    vector<AdjEntry>::iterator itNext = m_vAdj[v].begin();
    for (int i = 0; i < m_vAdj[u].size(); ++i)
    {
        int w = m_vAdj[u][i].pid;

        auto prc_info = std::lower_bound(itNext, m_vAdj[v].end(), w,
            [](const AdjEntry& info, int value)
            {
                return info.pid < value;
            });
        if (prc_info == m_vAdj[v].end())
        {
            // end
            break;
        }
        else if (w == prc_info->pid)
        {
            // found
            vLfE.push_back(m_vAdj[u][i].eid);
            vRtE.push_back(prc_info->eid);
            itNext = ++prc_info;
        }
        else
        {
            // not found
            itNext = prc_info;
        }
    }

    return vLfE.size();
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
#endif
/*****************
input:
        int x
        int y
description:
        find node
******************/
TPST_MAP_BY_EID * myG::findNode(int x, int y)
{
    int iEid = 0;
    if (m_vAdj[x].empty())
    {
        return NULL;
    }
    if (m_vAdj[y].empty())
    {
        return NULL;
    }
    /* the minimum degree */
    vector<AdjEntry> *pv = &(m_vAdj[x]);
    int v = y;
    if (m_vAdj[x].size() > m_vAdj[y].size())
    {
        pv = &(m_vAdj[y]);
        v = x;
    }

    vector<AdjEntry>::iterator it = lower_bound(pv->begin(), pv->end(), v,
              [](const AdjEntry& e, int tpPid) {
                return e.pid < tpPid;
              });
    if ((it != pv->end()) && (it->pid == v))
    {
        iEid = it->eid;
        return findNode(iEid);
    }
    return NULL;
}







