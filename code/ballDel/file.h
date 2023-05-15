#pragma once
/***************
input file output file function
****************/

#define DATA_PATH ("../data/test.txt")
#define TRUSS_FILE ("truss.txt")
#define ONE_LINE_BUFFER 100
#define FILE_NAME_BUFFER 100

typedef struct save_entry
{
    uint32_t x;
    uint32_t y;
    uint32_t k;
    uint32_t L;
    uint32_t SeSup;
} SaveEntry;
/* read file and fill the list g_lstFindEdgeByP */
//int file_fillG(char *szFileName, myG &mpG);
int file_readPrivate(char *szFileName, map<int, vector <int> > &mpPrivate, int *piMaxPid);
int file_readQuery(char *szFileName, vector <pair<int, int> > &lstQuery);

int file_saveDeTruss(myG &oMpG, map<int, vector<int> > &mpDeG, char *szTrussPath);
int file_saveTrussness(myG &oMpG, char *szTrussPath);
//int file_saveResult(myG &oMpG, list<int> &lstDG, int iNode, int iDk, char *szResultPath);

//int file_saveG(myG &oMpG, char *szGPath, bool bFinal);
int file_saveBitG(myG &oMpG, char *pcIndexFile);
//int file_saveAlley(alley &oAlley, char *szAlleyPath);
//int file_readG(myG &oMpG, char *szGPath);
int file_readBitG(myG &oMpG, char *pcIndexFile);
//int file_readAlley(alley &oAlley, char *szAlleyPath);
