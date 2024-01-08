/***************
my class G
****************/

#include <cstdlib>
#include <time.h>

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

    m_mpBasicG.clear();
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
        myG &oInitG
description:
        copy map
        calculate stage
******************/
myG::~myG()
{
    m_mpBasicG.clear();

    m_iMaxPId = 0;
    m_iMaxEId = 0;

    m_pvPNeCnt->clear();

    m_pvG->clear();

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
        debug_assert(0);
    }
    return &((*m_pvG)[eid]);
}


/*****************
input:
        int x
        int y
description:
        add node
******************/
bool myG::add(int x, int y)
{
    MAP_BASIC_G::iterator itGNode;
    TPST_MAP_BY_EID stTp = {0};
    int iPrevMaxPid = 0;

    itGNode = m_mpBasicG.find(pair<int, int>(x, y));
    if (itGNode != m_mpBasicG.end())
    {
        /* have joined, ignore */
        printf("DEBUG has joined\n");
        return false;
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
    }

    m_mpBasicG[pair<int, int>(x, y)] = m_iMaxEId;
    m_mpBasicG[pair<int, int>(y, x)] = m_iMaxEId;

    stTp.eid = m_iMaxEId;
    stTp.paXY = myG_getPair(x, y);
    stTp.bSampled = false;

    m_pvG->push_back(stTp);

    /* check right position */
    DEBUG_ASSERT(m_iMaxEId == (*m_pvG)[m_iMaxEId].eid);

    (*m_pvPNeCnt)[x]++;
    (*m_pvPNeCnt)[y]++;

    if ((*m_pvPNeCnt)[x] > m_iMaxD)
    {
        m_iMaxD = (*m_pvPNeCnt)[x];
    }
    if ((*m_pvPNeCnt)[y] > m_iMaxD)
    {
        m_iMaxD = (*m_pvPNeCnt)[y];
    }

    return true;
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
int myG::findNearNode(int iNId)
{
    MAP_BASIC_G::iterator itLowB;
    int iNearN = 0;

    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(iNId, 0));
    if (m_mpBasicG.end() != itLowB)
    {
        iNearN = itLowB->first.first;
    }
    return iNearN;
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
int myG::findNeb(int iNId, vector<int> &vNeib, vector<int> &vNeibE)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int iNebCnt = 0;
    int u = iNId;
    int v = 0;

    DEBUG_ASSERT(vNeib.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    while (itLowB != itHighB)
    {
        v = itLowB->first.second;
        vNeib.push_back(v);
        vNeibE.push_back(itLowB->second);
        iNebCnt++;
        itLowB++;
    }
    return iNebCnt;
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
int myG::findNewNeb(int iNId, vector<int> &vNewNeib)
{
    MAP_BASIC_G::iterator itLowB;
    MAP_BASIC_G::iterator itHighB;
    int u = iNId;
    int v = 0;


    DEBUG_ASSERT(vNewNeib.empty());
    /* find u's neighbor , note u > 0 */
    itLowB = m_mpBasicG.upper_bound(pair<int, int>(u, 0));
    itHighB = m_mpBasicG.upper_bound(pair<int, int>(u + 1, 0));
    if (itLowB != itHighB)
    {
        v = itLowB->first.second;
    }
    else
    {
        v = m_iMaxPId + 1;
    }
    for (int iPid = 1; iPid <= m_iMaxPId; ++iPid)
    {
        if (iPid == iNId)
        {
            /* ignore self */
            continue;
        }
        if (iPid < v)
        {
            vNewNeib.push_back(iPid);
            continue;
        }
        else if (iPid == v)
        {
            /* ignore neighbor */
            itLowB++;
            if (itLowB != itHighB)
            {
                v = itLowB->first.second;
            }
            else
            {
                v = m_iMaxPId + 1;
            }
        }
    }
    return vNewNeib.size();
}
/*****************
input:
        long iMax
output:
        none
description:
        random value
******************/
int myG::random(int iMax)
{
    int iBit = 0;
    int lTpV = iMax;
    int lRanV = 0;
    int iTpRanV = 0;
    static unsigned uSeed = 0;

    if (0 == uSeed)
    {
        char * pcTp = (char *)malloc(sizeof(char) * 10);
        uSeed = (unsigned)time(NULL) & (unsigned)((long long)pcTp % 0xffff);
        free(pcTp);
    }
    srand(uSeed);
    uSeed = rand();

    while (0 < lTpV)
    {
        /* 12 bits at each time */
        iBit += 12;
        lTpV >>= 12;

        iTpRanV = (rand() & 0xfff);
        lRanV = (lRanV << 12) | (iTpRanV);
        //printf("RANDOM tpV: %d total: %d\n", iTpRanV, lRanV);
    }
    //printf("RANDOM value: %d bits: %d\n", lRanV, iBit);
    return lRanV % iMax;
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
int myG::sample(vector <pair<int, int> > &vPubE, vector <pair<int, int> > &vPriE, float fRatio, int iSamNum)
{
    MAP_BASIC_G::iterator itE;
    vector<int>::iterator itN;
    vector<int>::iterator itvE;
    int iNCnt = 0;
    int iEid = 0;
    int lRanNId = 0;
    int lRanV = 0;
    vector <int> vSamN;
    map <int, int> mpSamN;
    map <int, int>::iterator itmpN;
    vector<int> vNeib;
    vector<int> vNeibE;
    TPST_MAP_BY_EID * pstNode = NULL;

    /* sample nodes */
    while (iNCnt < iSamNum)
    {
        lRanNId = random(m_iMaxPId);
        lRanNId = findNearNode(lRanNId);
        //printf("SAMPLE get node: %d neib: %d\n", lRanNId, (*m_pvPNeCnt)[lRanNId]);
        if (lRanNId < 1)
        {
            continue;
        }

        /* in case node removed */
        if ((*m_pvPNeCnt)[lRanNId] < 2)
        {
            continue;
        }
        if (mpSamN.find(lRanNId) == mpSamN.end())
        {
            mpSamN[lRanNId];
            vSamN.push_back(lRanNId);
            printf("SAMPLE sample node: %d\n", lRanNId);
        }
        ++iNCnt;
    }
    printf("SAMPLE sample nodes done\n");
    /* sample edges */
    for (itN = vSamN.begin(); itN != vSamN.end(); ++itN)
    {
        lRanNId = *itN;
        vNeib.clear();
        vNeibE.clear();
        findNeb(lRanNId, vNeib, vNeibE);
        for (itvE = vNeibE.begin(); itvE != vNeibE.end(); ++itvE)
        {
            lRanV = random(100);
            if (lRanV < 100 * fRatio)
            {
                /* sampled */
                pstNode = findNode(*itvE);
                DEBUG_ASSERT(NULL != pstNode);
                if (!pstNode->bSampled)
                {
                    if ((*m_pvPNeCnt)[pstNode->paXY.first] < 2)
                    {
                        continue;
                    }
                    if ((*m_pvPNeCnt)[pstNode->paXY.second] < 2)
                    {
                        continue;
                    }
                    pstNode->bSampled = true;
                    vPriE.push_back(pair<int, int>(pstNode->paXY.first, pstNode->paXY.second));
                    printf("SAMPLE sample edge: (%d, %d)\n",
                           pstNode->paXY.first, pstNode->paXY.second);
                    (*m_pvPNeCnt)[pstNode->paXY.first]--;
                    (*m_pvPNeCnt)[pstNode->paXY.second]--;
                }
            }
        }
    }
    printf("SAMPLE sample edges done\n");
    /* store edges */
    for (itE = m_mpBasicG.begin(); itE != m_mpBasicG.end(); ++itE)
    {
        if (itE->first.first > itE->first.second)
        {
            continue;
        }
        iEid = itE->second;
        pstNode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        if (!pstNode->bSampled)
        {
            vPubE.push_back(pair<int, int>(itE->first.first, itE->first.second));
        }
    }
    printf("SAMPLE store edges done\n");
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
int myG::sampleConnected(vector <pair<int, int> > &vPubE, vector <pair<int, int> > &vPriE, float fRatio, int iSamNum)
{
    MAP_BASIC_G::iterator itE;
    vector<int>::iterator itN;
    vector<int>::iterator itvE;
    int iNCnt = 0;
    int iEid = 0;
    int lRanNId = 0;
    int lRanV = 0;
    //vector <int> vSamN;
    //list <int> lsSamN;
    myPriQueueBig myCanQ;
    map <int, int> mpSamN;
    map <int, int>::iterator itmpN;
    vector<int> vNeib;
    vector<int> vNeibE;
    TPST_MAP_BY_EID * pstNode = NULL;

    /* sample nodes */
    while (iNCnt < iSamNum)
    {
        lRanNId = random(m_iMaxPId);
        lRanNId = findNearNode(lRanNId);
        //printf("SAMPLE get node: %d neib: %d\n", lRanNId, (*m_pvPNeCnt)[lRanNId]);
        if (lRanNId < 1)
        {
            continue;
        }
        ++iNCnt;

        /* in case node removed */
        if ((*m_pvPNeCnt)[lRanNId] < 2)
        {
            continue;
        }
        if (mpSamN.find(lRanNId) == mpSamN.end())
        {
            mpSamN[lRanNId];
            //lsSamN.push_front(lRanNId);
            myCanQ.insetByOrder((*m_pvPNeCnt)[lRanNId], lRanNId);
            printf("SAMPLE sample node: %d\n", lRanNId);
            //while ((!lsSamN.empty()) && (iNCnt < iSamNum))
            while ((!myCanQ.empty()) && (iNCnt < iSamNum))
            {
                //lRanNId = lsSamN.front();
                //lsSamN.pop_front();
                lRanNId = myCanQ.getTop();
                myCanQ.pop();
                printf("SAMPLE current: %d\n", lRanNId);
                /* sample edges */
                vNeib.clear();
                vNeibE.clear();
                findNeb(lRanNId, vNeib, vNeibE);
                itN = vNeib.begin();
                itvE = vNeibE.begin();
                for (; itvE != vNeibE.end(); ++itN, ++itvE)
                {
                    lRanV = random(100);
                    if (lRanV < 100 * fRatio)
                    {
                        /* sampled */
                        if (mpSamN.find(*itN) == mpSamN.end())
                        {
                            mpSamN[*itN];
                            //lsSamN.push_front(*itN);
                            myCanQ.insetByOrder((*m_pvPNeCnt)[*itN], *itN);
                            ++iNCnt;
                            printf("SAMPLE find node: %d\n", *itN);
                        }
                        pstNode = findNode(*itvE);
                        DEBUG_ASSERT(NULL != pstNode);
                        if (!pstNode->bSampled)
                        {
                            if ((*m_pvPNeCnt)[pstNode->paXY.first] < 2)
                            {
                                continue;
                            }
                            if ((*m_pvPNeCnt)[pstNode->paXY.second] < 2)
                            {
                                continue;
                            }
                            pstNode->bSampled = true;
                            vPriE.push_back(pair<int, int>(pstNode->paXY.first, pstNode->paXY.second));
                            printf("SAMPLE sample edge: (%d, %d)\n",
                                   pstNode->paXY.first, pstNode->paXY.second);
                            (*m_pvPNeCnt)[pstNode->paXY.first]--;
                            (*m_pvPNeCnt)[pstNode->paXY.second]--;
                        }
                    }
                }
            }
        }

    }
    printf("SAMPLE sample nodes done\n");

    /* store edges */
    for (itE = m_mpBasicG.begin(); itE != m_mpBasicG.end(); ++itE)
    {
        if (itE->first.first > itE->first.second)
        {
            continue;
        }
        iEid = itE->second;
        pstNode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        if (!pstNode->bSampled)
        {
            vPubE.push_back(pair<int, int>(itE->first.first, itE->first.second));
        }
    }
    printf("SAMPLE store edges done\n");
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
int myG::sampleBFS(vector <pair<int, int> > &vPubE, vector <pair<int, int> > &vPriE, float fRatio, int iSamNum)
{
    MAP_BASIC_G::iterator itE;
    vector<int>::iterator itN;
    vector<int>::iterator itvE;
    int iNCnt = 0;
    int iEid = 0;
    int lRanNId = 0;
    int lRanV = 0;
    //vector <int> vSamN;
    list <int> lsSamN;
    //myPriQueueBig myCanQ;
    map <int, int> mpSamN;
    map <int, int>::iterator itmpN;
    vector<int> vNeib;
    vector<int> vNeibE;
    TPST_MAP_BY_EID * pstNode = NULL;

    /* sample nodes */
    while (iNCnt < iSamNum)
    {
        lRanNId = random(m_iMaxPId);
        lRanNId = findNearNode(lRanNId);
        //printf("SAMPLE get node: %d neib: %d\n", lRanNId, (*m_pvPNeCnt)[lRanNId]);
        if (lRanNId < 1)
        {
            continue;
        }
        ++iNCnt;

        /* in case node removed */
        if ((*m_pvPNeCnt)[lRanNId] < 2)
        {
            continue;
        }
        if (mpSamN.find(lRanNId) == mpSamN.end())
        {
            mpSamN[lRanNId];
            lsSamN.push_back(lRanNId);
            //myCanQ.insetByOrder((*m_pvPNeCnt)[lRanNId], lRanNId);
            printf("SAMPLE sample node: %d\n", lRanNId);
            while ((!lsSamN.empty()) && (iNCnt < iSamNum))
            //while ((!myCanQ.empty()) && (iNCnt < iSamNum))
            {
                lRanNId = lsSamN.front();
                lsSamN.pop_front();
                //lRanNId = myCanQ.getTop();
                //myCanQ.pop();
                printf("SAMPLE current: %d\n", lRanNId);
                /* sample edges */
                vNeib.clear();
                vNeibE.clear();
                findNeb(lRanNId, vNeib, vNeibE);
                itN = vNeib.begin();
                itvE = vNeibE.begin();
                for (; itvE != vNeibE.end(); ++itN, ++itvE)
                {
                    lRanV = random(100);
                    if (lRanV < 100 * fRatio)
                    {
                        /* sampled */
                        if (mpSamN.find(*itN) == mpSamN.end())
                        {
                            mpSamN[*itN];
                            lsSamN.push_back(*itN);
                            //myCanQ.insetByOrder((*m_pvPNeCnt)[*itN], *itN);
                            ++iNCnt;
                            printf("SAMPLE find node: %d\n", *itN);
                        }
                        pstNode = findNode(*itvE);
                        DEBUG_ASSERT(NULL != pstNode);
                        if (!pstNode->bSampled)
                        {
                            if ((*m_pvPNeCnt)[pstNode->paXY.first] < 2)
                            {
                                continue;
                            }
                            if ((*m_pvPNeCnt)[pstNode->paXY.second] < 2)
                            {
                                continue;
                            }
                            pstNode->bSampled = true;
                            vPriE.push_back(pair<int, int>(pstNode->paXY.first, pstNode->paXY.second));
                            printf("SAMPLE sample edge: (%d, %d)\n",
                                   pstNode->paXY.first, pstNode->paXY.second);
                            (*m_pvPNeCnt)[pstNode->paXY.first]--;
                            (*m_pvPNeCnt)[pstNode->paXY.second]--;
                        }
                    }
                }
            }
        }

    }
    printf("SAMPLE sample nodes done\n");

    /* store edges */
    for (itE = m_mpBasicG.begin(); itE != m_mpBasicG.end(); ++itE)
    {
        if (itE->first.first > itE->first.second)
        {
            continue;
        }
        iEid = itE->second;
        pstNode = findNode(iEid);
        DEBUG_ASSERT(NULL != pstNode);
        if (!pstNode->bSampled)
        {
            vPubE.push_back(pair<int, int>(itE->first.first, itE->first.second));
        }
    }
    printf("SAMPLE store edges done\n");
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
int myG::sampleNew(vector <pair<int, int> > &vPriE, int iSamNum)
{
    int iRootNode = 0;

    static unsigned uiSeed = time(NULL);

    srand(uiSeed);
    uiSeed = random(m_iMaxPId) + 1;
    /* sample root node */
    while (1)
    {
        iRootNode = random(m_iMaxPId);
        iRootNode = findNearNode(iRootNode);
        if (iRootNode < 2)
        {
            continue;
        }
        break;
    }
    printf("SAMPLE sample root: %d\n", iRootNode);

    vector<int> vNeib;
    vNeib.reserve(m_iMaxPId);
    findNewNeb(iRootNode, vNeib);
    vector<int> vSampleNeib;
    random_shuffle(vNeib.begin(), vNeib.end());

    for (int i = 0; i < iSamNum; ++i)
    {
        if (i >= vNeib.size())
        {
            /* no enough nodes */
            break;
        }
        int iPid = vNeib[i];
        if (iPid < iRootNode)
        {
            vPriE.push_back(pair<int, int> (iPid, iRootNode));
        }
        else
        {
            vPriE.push_back(pair<int, int> (iRootNode, iPid));
        }
    }
    printf("SAMPLE sample nodes: %d\n", vPriE.size());

    return iRootNode;
}

/*****************
input:
        myG &oMpG
        list <pair<int, int> > &lsE
        char *pcFileName
output:
        none
description:
        get one hop nodes
******************/
int myG::getOneHop(vector <int> &vSrc, vector <int> &vNeib)
{
    vector <int>::iterator itvN;

    for (itvN = vSrc.begin(); itvN != vSrc.end(); ++itvN)
    {
        vector <int> vNeibN;
        vector <int> vNeibE;

        findNeb(*itvN, vNeibN, vNeibE);
        vNeib.insert(vNeib.end(), vNeibN.begin(), vNeibN.end());
    }
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
int myG::sample2Hop(vector <pair<int, int> > &vPriE, int iSamNum)
{
    MAP_BASIC_G::iterator itE;
    vector<int>::iterator itN;
    vector<int>::iterator itvE;
    int iNCnt = 0;
    int iEid = 0;
    int lRanNId = 0;
    int lRanV = 0;
    int iRootNode = 0;
    map <int, int> mpSamN;
    /* id, none */
    map <int, int> mpNeib;
    map <int, int>::iterator itmpN;
    vector<int> vNeib;
    vector<int> vNeibE;
    vector<int> v2HopNodes;
    vector<int> v1HopNodes;
    TPST_MAP_BY_EID * pstNode = NULL;
    bool bFullFlag = false;
    int iNodeMax = 0;

    /* sample root node */
    while (1)
    {
        iRootNode = random(m_iMaxPId);
        iRootNode = findNearNode(iRootNode);
        if (iRootNode < 1)
        {
            continue;
        }
        break;
    }
    printf("SAMPLE sample root: %d\n", iRootNode);

    v2HopNodes.push_back(iRootNode);
    /* get one hop */
    getOneHop(v2HopNodes, v1HopNodes);
    /* get two hop */
    v2HopNodes.clear();
    getOneHop(v1HopNodes, v2HopNodes);

    /* keep only one */
    for (itN = v2HopNodes.begin(); itN != v2HopNodes.end(); ++itN)
    {
        mpNeib[*itN] = 1;
    }
    if (mpNeib.find(iRootNode) != mpNeib.end())
    {
        mpNeib[iRootNode] = 0;
    }
    for (itN = v1HopNodes.begin(); itN != v1HopNodes.end(); ++itN)
    {
        if (mpNeib.find(*itN) != mpNeib.end())
        {
            mpNeib[*itN] = 0;
        }
    }
    v2HopNodes.clear();
    for (itmpN = mpNeib.begin(); itmpN != mpNeib.end(); ++itmpN)
    {
        if (0 < itmpN->second)
        {
            v2HopNodes.push_back(itmpN->first);
        }
    }

    iNodeMax = v2HopNodes.size();
    printf("SAMPLE total 2-hop nodes: %d\n", iNodeMax);
    if (iNodeMax < 1)
    {
        /* no nodes can sample */
        return 0;
    }
    bFullFlag = iNodeMax > iSamNum;

    /* sample nodes */
    while (iNCnt < iSamNum)
    {
        lRanNId = random(iNodeMax);
        DEBUG_ASSERT(lRanNId < iNodeMax);
        lRanNId = v2HopNodes[lRanNId];
        if (lRanNId < 1)
        {
            continue;
        }
        if (lRanNId == iRootNode)
        {
            /* no loop */
            continue;
        }
        if (!bFullFlag)
        {
            ++iNCnt;
        }
        if (mpSamN.find(lRanNId) != mpSamN.end())
        {
            /* sampled */
            continue;
        }

        if (m_mpBasicG.find(pair<int, int>(lRanNId, iRootNode)) != m_mpBasicG.end())
        {
            /* exist */
            DEBUG_ASSERT(0);
            continue;
        }

        mpSamN[lRanNId];
        printf("SAMPLE sample node: %d\n", lRanNId);
        if (bFullFlag)
        {
            ++iNCnt;
        }
        if (lRanNId < iRootNode)
        {
            vPriE.push_back(pair<int, int> (lRanNId, iRootNode));
        }
        else
        {
            vPriE.push_back(pair<int, int> (iRootNode, lRanNId));
        }

    }
    printf("SAMPLE sample nodes done\n");

    return iRootNode;
}




