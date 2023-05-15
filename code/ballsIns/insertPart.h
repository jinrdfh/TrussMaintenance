#pragma once
/***************
alley function
****************/

typedef struct tpstmpPInfo
{
    int iPid;
    int iEid;
    int iLocalD;
    int iMaxK;
    int iLayer;
    int iLowB;
    int iSup;
    int iJuSup;
    int iSeSup;
    bool bRmFlag;
    bool bLock;
}TPSTMP_P_INFO;

typedef struct tpstOrder
{
    int k;
    int L;
    int eid;
}TPST_ORDER;

#define INSERT_BALL_I 1
#define INSERT_BALL_II 2

typedef struct tpstBallInfo
{
    int iPid;
    int iType;
    bool bUsed;

    /* I-ball */
    int iNeibP;
    int iEid;
    //vector<int> vLfE;
    //vector<int> vRtE;

    /* II-ball */
    /* pid, eid : (center, node) */
    //map <int, int> mpNeibP;
    vector<int> vNewP;
    /* pid */
    //vector<int> vThirdE;
}TPST_BALL_INFO;

class insertPart
{
private:
    static int rmEInVisQEasy(myG &mpG, int iCurK, int iCurL, myPriQueue &myVstQ);
    static int insKByBFSEasy(myG &mpG, int iCurT, vector <int> &vSeed, vector <int> &vUpGE, vector <int> &vUpdSeed);
    static int upgradeForAllK(myG &mpG, vector <int> &vCdtE);
    static int simpleAdd(myG &mpG, int x, int y);

    static int localDec(myG &mpG, int iNode, vector <TPST_ADJ> &vNeibE, vector <int> &vSeed);


    static bool checkInsSeSup(myG &mpG, int iSelfId, int iLtId, int iRtId);
    static bool checkDesSeSup(myG &mpG, int iSelfId, int iLtId, int iRtId);

    static int fillInfo(myG &mpG, map<int, vector <int> > &mpPrivate, list<pair<int, int> > &lstQuery, map<int, TPST_BALL_INFO > &mpBallInfo);
    static int disjoint(myG &mpG, map<int, TPST_BALL_INFO > &mpBallInfo, vector< vector<int> > &vInsScheme);

    static int nodeDec(myG &mpG, int iNode, vector <int> &vSeed);
    static int edgeDec(myG &mpG, int iEid, vector <int> &vSeed);
    static int batchIns(myG &mpG, map<int, TPST_BALL_INFO > &mpBallInfo, vector<int> &vNodes);

public:

    static bool cmpOrder(const TPST_ORDER& e1, const TPST_ORDER& e2);
    static int insertAll(myG &mpG, map<int, vector <int> > &mpPrivate, list<pair<int, int> > &lstQuery);

};
