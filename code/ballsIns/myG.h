#pragma once
/***************
my class G
****************/

/* base = init - sub */
#if 0
template <typename mpTA, typename mpTB>
bool myG_mapSub(mpTA &mpBase, mpTB &mpInitG, mpTA &mpSub)
{
    typename mpTB::iterator itmapT;

    for (itmapT = mpInitG.begin(); itmapT != mpInitG.end(); itmapT++)
    {
        if (mpSub.find(itmapT->first) == mpSub.end())
        {
            /* doesn't have */
            mpBase[itmapT->first];
        }
    }
    return 0;
}
#endif

#define myG_getPair(iX, iY) ((iX < iY) ? (pair<int, int>(iX, iY)) : (pair<int, int>(iY, iX)))

//typedef map<pair<int, int>, int > MAP_BASIC_G;
/*typedef struct tpstlsByEid
{
    int iLfEid;
    int iRtEid;
}TPST_LS_BY_EID;*/

typedef struct tpstAdj
{
    int iPid;
    int iEid;
}TPST_ADJ;

typedef struct tpstMapByEid
{
    int eid;
    int iTpId;
    //int iSup;
    //int iJuSup;
    int iSeSup;
    //int iOutSup;
    //int iTrigger;
    int iTrussness;
    //int iLowB;
    int iLayer;
    //bool bNewFlag;
    //bool bPseNewFlag;
    bool bInit;
    bool bUgdFlag;
    //bool bBdBallFlag;
    //bool bNewJuTriFlag;
    //bool bSeedFlag;
    bool bVstFlag;
    bool bVisQFlag;
    //bool bHandledFlag;
    //bool bRmFlag;
    //bool bLock;
    bool bInsLFlag;
    bool bCanQFlag;
    bool bDoneQFlag;
    bool bUsedCanFlag;
    bool bUsedDoneFlag;
    bool bUsedVisFlag;
    pair<int, int> paXY;
    vector<int> vLfE;
    vector<int> vRtE;
    //vector<int> vKLfE;
    //vector<int> vKRtE;
}TPST_MAP_BY_EID;

class myG
{
public:
    /* double */
    //MAP_BASIC_G m_mpBasicG;
    vector<vector<TPST_ADJ> > m_vAdj;
    vector<int> *m_pvPNeCnt;

    int m_iMaxPId;
    int m_iMaxEId;
    int m_iMaxD;
    int m_iMaxK;
    int m_iDensePId;
    /* single, x < y */
    vector<TPST_MAP_BY_EID> *m_pvG;

    /* eid, <pid> > */
    //map<int, vector<int> > mpNeibPPool;

    myG();
    ~myG();
    int init();
    int init(int iEid);
    int add(int x, int y);
    int addNeibTri(int iEid);
    //int addLsNeib(int iEid, int iLfEid, int iRtEid);
    //int rmLsNeib(int iEid, list<TPST_LS_BY_EID>::iterator pstls);
    //int rmLsNeib(int iEid);
    //bool copyMap(myG &oInitG);
    //int updateJuSup(int iEid);
    //int updateSeSup(int iEid);
    //int checkSeSup(int iEid);
    //int showSeSup(int iEid);
    //int findNeb(int x, int y, vector<int> &vNeib);
    int findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE);
    //int findNeb(int x, int y, vector<int> &vNeibP, vector<int> &vLfE, vector<int> &vRtE);
    //int findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE, MAP_BASIC_G &mapCalG);

    //int findNebE(int x, int y, list<pair<int, int> > &lspaNeibE);

    /* find eid in calG, at least trussness */
    //int findNebE(int x, int y, list<int> &lstNeibE, MAP_BASIC_G &mapCalG, int iTrussness);

    /* find neighbor edge in calG, not in BlackG, mintruss == truss */
    //int findNebE(int x, int y, list<int> &lstNeibE, MAP_BASIC_G &mapCalG, MAP_BASIC_G &mapBlackG, int iTrussness);

    /* find neighbor edge in calG, not in BlackG, mintruss == truss */
    //int findThirdE(int iNode, list<int> &lsThirdE);

    //int findPNebP(int iNodeId, list<int> &lstP);
    //int findPNebP(int iNodeId, vector <pair<int, int> > &vNeibP);
    //int findPNebP(int iNodeId, list<int> &lstP, MAP_BASIC_G &mapCalG);
    //int findPNebPTrdE(int iNodeId, vector <int> &vNeibP, vector <int> &vThirdE);
    //int findPNebPTrdE(int iNodeId, vector <TPST_ADJ> &vNeibP, vector <int> &vThirdE);

    //bool simpleRm(int x, int y);
    /* will not copy map, will copy array */
    int adjInsert(int x, int y, int eid);
    TPST_MAP_BY_EID * findNode(int eid);
    TPST_MAP_BY_EID * findNode(int x, int y);

    /*pair <int, int> findByEid(int eid)
    {
        return (*m_pvG)[eid].paXY;
    }*/

};
