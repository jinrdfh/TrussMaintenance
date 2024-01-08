
using namespace std;

#include <list>
#include <string.h>
#include <sys/time.h>

#undef NDEBUG
#include <assert.h>

#include "KTrussIndex/KTrussIndex.h"
#include "fileIO/fileIO.h"
#include "KTrussUpdate/KTrussUpdate.h"


int main(int argc, char *argv[])
{
    KTrussIndex oKTIndex;
    list<pair<int, int> > lsE;
    Graph *poG = new Graph();
    int iModeEdge = 0;
    int iModeInsert = 0;

	struct timeval tv;
	long lStartTime = 0;
	long lCurTime = 0;
	long lTotalTime = 0;

    if (6 != argc)
    {
        cout << "error! Run as ./dynamic_truss.out G.index [G.increment/G.decrement] G.index_update \
        [-insert/-delete] [node/edge] " << endl;

        //return 1;
    }
    fileIO_getIndex(argv[1], oKTIndex, poG);
    fileIO_getEdges(argv[2], lsE);

    /* start timer */
	gettimeofday(&tv, NULL);
	lStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;

    iModeEdge = strncmp("edge", argv[5], sizeof("edge"));
    iModeInsert = strncmp("-insert", argv[4], sizeof("-insert"));

    if ((0 == iModeEdge) && (0 == iModeInsert))
    {
        /* insert edge */
        for (pair<int, int> &paE : lsE)
        {
            //cout << "now add edge: " << paE.first << ", " << paE.second << endl;
            poG->addEdge(paE.first, paE.second);
            KTrussUpdate::updateEdgeInsertion(Edge(paE.first, paE.second), &oKTIndex);
        }
    }
    else if (0 == iModeInsert)
    {
        /* insert node */
        /* find node v */
        int v = 0;
        map <int, int> mpTp;
        for (pair<int, int> &paE : lsE)
        {
            if (mpTp.find(paE.first) != mpTp.end())
            {
                v = paE.first;
                break;
            }
            mpTp[paE.first];
            if (mpTp.find(paE.second) != mpTp.end())
            {
                v = paE.second;
                break;
            }
            mpTp[paE.second];
        }
        assert(0 != v);
        cout << "find node" << v << endl;

        if (poG->hasNode(v))
        {
            KTrussUpdate::updateNodeDeletion(v, &oKTIndex);
        }
        for (pair<int, int> &paE : lsE)
        {
            poG->addEdge(paE.first, paE.second);
        }

        vector<int> ngV;
        for (Edge e: poG->neighbors(v))
            ngV.push_back(e.other(v));
        KTrussUpdate::updateNodeInsertion(v, ngV, &oKTIndex);
    }
    else
    {
        /* delete node */
        /* find node v */
        int v = 0;
        map <int, int> mpTp;
        for (pair<int, int> &paE : lsE)
        {
            if (mpTp.find(paE.first) != mpTp.end())
            {
                v = paE.first;
                break;
            }
            mpTp[paE.first];
            if (mpTp.find(paE.second) != mpTp.end())
            {
                v = paE.second;
                break;
            }
            mpTp[paE.second];
        }
        assert(0 != v);
        cout << "find node" << v << endl;

        if (poG->hasNode(v))
        {
            KTrussUpdate::updateNodeDeletion(v, &oKTIndex);
        }
    }

	gettimeofday(&tv, NULL);
	lCurTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	lTotalTime = lCurTime - lStartTime;
	//printf("read file Time=%ld ms\n", lTotalTime);

    cout << "total time = " << lTotalTime << " ms" << endl;

    cout << "start save" << endl;
    fileIO_saveIndex(argv[3], oKTIndex);
    cout << "end save" << endl;
    return 0;
}
