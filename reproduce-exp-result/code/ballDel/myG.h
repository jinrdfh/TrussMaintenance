#pragma once
/***************
my class G
****************/

// edge array entry type
typedef struct tpstMapByEid
{
    int eid;
    int iTpId;
    int iSup;
    int iKSup;
    int iSeSup;
    int iKMSup;
    //int iKPSup; // bLInit
    int iTrussness;
    int iLayer;
    //int iNeibMaxL; // bLInit
    int iOldL;
    // bool bInit; // bInit
    //bool bLInit; // bLInit
    bool bDgdFlag;
    bool bVstFlag;
    bool bRmFlag;
    bool bCanQFlag;
    pair<int, int> paXY;
    /* left eid, right eid , not sorted */
    vector<uint32_t> vLfE; // bInit
    vector<uint32_t> vRtE; // bInit
    /* left eid, right eid , not sorted */
    //vector<uint32_t> vKLfE; // bLInit
    //vector<uint32_t> vKRtE; // bLInit
} TPST_MAP_BY_EID;
// adjacency array entry type
typedef struct stAdjInfo
{
    int pid;
    int eid;
} AdjEntry;

class myG
{
public:

    /* pid, eid */
    vector<vector<AdjEntry> > m_vAdj;

    int m_iMaxPId;
    int m_iMaxEId;
    int m_iMaxD;
    int m_iMaxK;
    int m_iDensePId;
    int m_iInitCnt; // debug
    /* single, x < y */
    vector<TPST_MAP_BY_EID> *m_pvG;

    myG();
    ~myG();
    int init();
    //int init(int iEid);
    TPST_MAP_BY_EID *add(int iEid);
    int rm(int iEid);

    int findNeb(int x, int y, vector<uint32_t> &vLfE, vector<uint32_t> &vRtE);
    //int findNeb(int iEid, vector<int> &vLfE, vector<int> &vRtE);
    //int findHalfNeb(int x, int y, vector<int> &vLfE, vector<int> &vRtE);
    //int findPNebPTrdE(int iNodeId, vector <pair<int, int> > &vNeibP, vector <int> &vThirdE);

    TPST_MAP_BY_EID * findNode(int eid);
    TPST_MAP_BY_EID * findNode(int x, int y);

    pair <int, int> findByEid(int eid)
    {
        return (*m_pvG)[eid].paXY;
    }

};
