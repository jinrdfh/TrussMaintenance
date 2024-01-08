#pragma once
/***************
my class G
****************/

#define myG_getPair(iX, iY) ((iX < iY) ? (pair<int, int>(iX, iY)) : (pair<int, int>(iY, iX)))

typedef map<pair<int, int>, int > MAP_BASIC_G;

typedef struct tpstMapByEid
{
    int eid;
    pair<int, int> paXY;
    bool bSampled;
}TPST_MAP_BY_EID;

class myG
{
public:
    /* double */
    MAP_BASIC_G m_mpBasicG;
    vector<int> *m_pvPNeCnt;

    int m_iMaxPId;
    int m_iMaxEId;
    int m_iMaxD;
    /* single, x < y */
    vector<TPST_MAP_BY_EID> *m_pvG;

    myG();
    ~myG();
    TPST_MAP_BY_EID *findNode(int eid);
    int findNearNode(int iNId);
    int findNeb(int iNId, vector<int> &vNeib, vector<int> &vNeibE);
    int findNewNeb(int iNId, vector<int> &vNewNeib);
    int getOneHop(vector <int> &vSrc, vector <int> &vNeib);
    bool add(int x, int y);
    int random(int iMax);
    int sample(vector <pair<int, int> > &vPubE, vector <pair<int, int> > &vPriE, float fRatio, int iSamNum);
    int sampleBFS(vector <pair<int, int> > &vPubE, vector <pair<int, int> > &vPriE, float fRatio, int iSamNum);
    int sampleConnected(vector <pair<int, int> > &vPubE, vector <pair<int, int> > &vPriE, float fRatio, int iSamNum);

    int sampleNew(vector <pair<int, int> > &vPriE, int iSamNum);
    int sample2Hop(vector <pair<int, int> > &vPriE, int iSamNum);

    pair <int, int> findByEid(int eid)
    {
        return (*m_pvG)[eid].paXY;
    }

};
