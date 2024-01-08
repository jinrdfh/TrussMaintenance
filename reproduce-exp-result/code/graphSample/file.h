#pragma once
/***************
input file output file function
****************/

#define ONE_LINE_BUFFER 100
#define FILE_NAME_BUFFER 100
/* read file */
int file_fillG(myG &mpG, char *szFileName);

int file_saveGraph(myG &oMpG, vector <pair<int, int> > &vEdge, char *pcFileName);
int file_savePriGraph(myG &oMpG, int iRoot, int iId, vector <pair<int, int> > &vEdge, char *pcPriPath);

