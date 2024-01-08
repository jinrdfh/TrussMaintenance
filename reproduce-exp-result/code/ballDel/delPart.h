#pragma once
/***************
alley function
****************/

/*typedef struct tpstmpPInfo
{
    int iPid;
    int iEid;
    int iLocalD;
    int iMaxK;
    int iLowB;
    int iLayer;
    int iSeSup;
    int iCurK;
    int iCurLayer;
    int iKSup;
    int iKMSup;
    bool bRmFlag;
    bool bInPriQFlag;
    bool bLock;
}TPSTMP_P_INFO;*/

typedef struct tpstOrder
{
    int k;
    int L;
    int eid;
}TPST_ORDER;

class delPart
{
private:
    //static int lcDecomp(myG &mpG, int iCurT, int iCurL, vector <int> &vUpSeed);
    //static int checkOrder(myG &mpG, int iEid, int iLfEid, int iRtEid);
    //static bool triRmCheck(myG &mpG, int iEid, int iLfEid, int iRtEid);
    static int triOrder(myG &mpG, int iEid, int iLfEid, int iRtEid, bool *pbLfSup, bool *pbRtSup, bool *pbLfCheck, bool *pbRtCheck);

    //static int deKByBFSEasy(myG &mpG, int iCurT, vector <int> &vSeed, vector <int> &vUpGE, vector <int> &vUpdSeed);
    //static int degradeForAllK(myG &mpG, vector <int> &vCdtE);
    static int simpleRm(myG &mpG, int iEid);

    //static int localDec(myG &mpG, int iNode, map<pair<int, int>, int> &mpLocalG, unordered_map<int, TPSTMP_P_INFO> &mpLocalP, map<pair<int, int>, list<pair<int, int> > > &mpKPLs);
    //static int nodeDec(myG &mpG, int iNode, vector <int> &vCdtE);

    static int updateK(myG &mpG, vector <int> &vCdtE, vector <int> &vChgKE);
    static int updNeibL(myG &mpG, int iEid, vector <int> &vCdtChgLE);
    static int recalL(myG &mpG, int iEid);
    static int recalL(myG &mpG, vector <int> &vChgKE, vector <int> &vCdtChgLE);
    static int updateL(myG &mpG, vector <int> &vCdtE, vector <int> &vChgLE);

    static int singleEDec(myG &mpG, int iNode, int iRmNode, int *piRmEid, vector <int> &vKSeed, vector <int> &vLSeed);
    static int localDec(myG &mpG, int iNode, vector <AdjEntry> &vNeibE, vector <int> &vRmNeb, vector <int> &vRmE, vector <int> &vKSeed, vector <int> &vLSeed);
    //static int ballDec(myG &mpG, int iNode, vector <int> &vRmP);
public:
    static bool cmpOrder(const TPST_ORDER& e1, const TPST_ORDER& e2);
    static int delOneNodePart(myG &mpG, map<int, vector <int> > &mpPrivate, int iNode);

};
