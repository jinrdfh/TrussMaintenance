/***************
main cpp
the code for dividing a graph into star motifs, 
which generates inputs for maintenance algorithms in paper 
"Efficient Star-based Truss Maintenance on Dynamic Graphs".
****************/
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <cstdint>

#include <vector>
#include <map>

using namespace std;
// assert function
#define ASSERT(truth) \
    if (!(truth)) { \
      std::cerr << "\x1b[1;31mASSERT\x1b[0m: " \
                << "LINE " << __LINE__ \
                << ", " << __FILE__ \
                << std::endl; \
      std::exit(EXIT_FAILURE); \
    } else

#define ASSERT_MSG(truth, msg) \
    if (!(truth)) { \
      std::cerr << "\x1b[1;31mASSERT\x1b[0m: " \
                << "LINE " << __LINE__ \
                << ", " << __FILE__ << '\n' \
                << "\x1b[1;32mINFO\x1b[0m: " << msg \
                << std::endl; \
      std::exit(EXIT_FAILURE); \
    } else

// adjacency array entry type
typedef struct final
{
    uint32_t vid;
    uint32_t eid;
} ArrayEntry;

/*****************
input:
        graph data file name
        output star motif file
description:
        main function
******************/
int main(int argc, char** argv)
{
    ASSERT_MSG(3 == argc, "wrong parameters");
    vector<pair<uint32_t, uint32_t>> edges_;
    vector<vector<ArrayEntry>> adj_;
    uint32_t n_;  // the # of vertices
    uint32_t m_;  // the # of edges

    ifstream infile(argv[1], ios::in);
    ASSERT_MSG(infile.is_open(), "cannot open the file");
    ASSERT_MSG(!infile.eof(), "invalid graph file");
    // read the edges
    while (true)
    {
        uint32_t v1, v2;
        infile >> v1 >> v2;
        if (infile.eof()) break;
        if (v1 > v2) swap(v1, v2);
        edges_.push_back({v1, v2});
        n_ = (n_ > v2) ? n_ : v2;
        ASSERT_MSG(v1 < v2, "loops exist in the graph");
    }
    infile.close();
    m_ = edges_.size();
    ++n_;

    adj_.resize(n_);
    for (uint32_t eid = 0; eid < m_; ++eid)
    {
        const uint32_t v1 = edges_[eid].first;
        const uint32_t v2 = edges_[eid].second;
        adj_[v1].push_back({v2, eid});
        adj_[v2].push_back({v1, eid});
    }

    vector<bool> removed(n_, false);
    ofstream outfile(argv[2], ios::out);

    vector<int> vDeg(n_, 0);
    int iDMax = 0;

    for (uint32_t pid = 0; pid < n_; ++pid)
    {
        vDeg[pid] = adj_[pid].size();
        iDMax = (iDMax > vDeg[pid])? iDMax : vDeg[pid];
    }
    vector<vector<uint32_t> > bin(iDMax + 1);
    for (uint32_t pid = 0; pid < n_; ++pid)
    {
        bin[vDeg[pid]].push_back(pid);
    }
    int iThre = 2;
    while (1 < bin.size())
    {
        vector<uint32_t> vDesE;
        int iDeg = bin.size() - 1;
        if ((iDeg < iThre) && (!bin[1].empty()))
        {
            /* single edge */
            vDesE.swap(bin[1]);
            iDeg = 1;
        }
        else
        {
            /* star */
            vDesE.swap(bin.back());
            bin.pop_back();
        }
        int iCnt = 0;
        for (uint32_t pid : vDesE)
        {
            if (vDeg[pid] != iDeg)
            {
                /* in other bins */
                continue;
            }
            ++iCnt;
            //printf("find node: %d\nneighbor: ", pid);
            removed[pid] = true;
            vector<uint32_t> vEid;
            for (auto atP : adj_[pid])
            {
                uint32_t neibPid = atP.vid;
                if (!removed[neibPid])
                {
                    uint32_t eid = atP.eid;
                    --vDeg[neibPid];
                    bin[vDeg[neibPid]].push_back(neibPid);
                    vEid.push_back(eid);
                    //printf("%d ", neibPid);
                }
            }
            if (!vEid.empty())
            {
                outfile << "n " << pid << "\n";
                for (uint32_t eid : vEid)
                {
                    outfile << edges_[eid].first << " " << edges_[eid].second << "\n";
                }
            }
        }
        if (0 < iCnt)
        {
            printf("Degree: %d size: %d\n", iDeg, iCnt);
        }
    }
    outfile.close();

}
