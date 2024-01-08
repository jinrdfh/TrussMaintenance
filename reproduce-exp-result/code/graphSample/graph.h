#pragma once
/***************
graph structure define
****************/

typedef struct tpstDGNode
{
    int iEid;
    int iLayer;
}TPST_DG_NODE;
typedef struct tpstDecompG
{
    int iTrussness;
    list<TPST_DG_NODE> lsENode;
}TPST_DECOMP_G;

typedef list<TPST_DECOMP_G> LIST_DECOMP_G;









