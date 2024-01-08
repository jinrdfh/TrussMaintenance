
#ifndef FILE_I_O_H
#define FILE_I_O_H

#define FILE_NAME_LEN 100
#define ONE_LINE_BUFFER 100

int fileIO_getIndex(char *szFileName, KTrussIndex &oKTIndex, Graph *poG);
int fileIO_saveIndex(char *szFileName, KTrussIndex &oKTIndex);
int fileIO_getEdges(char *szFileName, list<pair<int, int> > &lsE);

#endif //FILE_I_O_H
