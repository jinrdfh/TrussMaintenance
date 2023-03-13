#pragma once
/***************
my class G
****************/

/* base = init - sub */
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

#define myG_getPair(iX, iY) ((iX < iY) ? (pair<int, int>(iX, iY)) : (pair<int, int>(iY, iX)))

typedef map<pair<int, int>, int > MAP_BASIC_G;

typedef struct tpstMapByEid
{
    int eid;
    int iSup;
    int iKSup;
    int iSeSup;
    int iKMSup;
    int iTrussness;
    int iLayer;
    int iOldL;
    bool bInit;
    bool bDgdFlag;
    bool bVstFlag;
    bool bRmFlag;
    bool bCanQFlag;
    //bool bUsedCanFlag;
    pair<int, int> paXY;
    /*vector<int> vLfE;
    vector<int> vRtE;*/
    /* left eid, right eid */
    map<int, int> mpLfE;
    /* right eid, left eid */
    //map<int, int> mpRtE;
}TPST_MAP_BY_EID;

class myG
{
public:
    /* double */
    //MAP_BASIC_G m_mpBasicG;
    /* pid, eid */
    vector<map<int, int> > *m_pvPNe;
    vector<int> *m_pvPNeCnt;

    int m_iMaxPId;
    int m_iMaxEId;
    int m_iMaxD;
    int m_iMaxK;
    int m_iDensePId;
    /* single, x < y */
    vector<TPST_MAP_BY_EID> *m_pvG;

    myG();
    ~myG();
    int init(int iEid);
    //int addNeibTri(int iEid);
    TPST_MAP_BY_EID *add(int iEid);
    //int add(int x, int y);
    int rm(int iEid);

    //int updateSeSup(int iEid);
    //int checkSeSup(int iEid);
    int findNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE);
    int findNeb(int iEid, vector<int> &vLfE, vector<int> &vRtE);
    int findHalfNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE);
    int findPNebPTrdE(int iNodeId, vector <pair<int, int> > &vNeibP, vector <int> &vThirdE);
    int findPNebP(int iNodeId, vector <pair<int, int> > &vNeibP, MAP_BASIC_G &mapCalG);

    /* will not copy map, will copy array */
    TPST_MAP_BY_EID * findNode(int eid);
    TPST_MAP_BY_EID * findNode(int x, int y);

    pair <int, int> findByEid(int eid)
    {
        return (*m_pvG)[eid].paXY;
    }

};
