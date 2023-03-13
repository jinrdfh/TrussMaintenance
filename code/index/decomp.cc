#include "decomp.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <utility>
#include <chrono>

namespace truss_maint {
namespace decomp {
// for convenience
using std::uint32_t;
// truss decomposition and the corresponding order
Decomp::Decomp(const std::string& file_name)
{
    std::ifstream infile(file_name, std::ios::in);
    ASSERT_MSG(infile.is_open(), "cannot open the file");
    ASSERT_MSG(!infile.eof(), "invalid graph file");
    // read the edges
    //printf("DEBUG init n: %d\n", n_);
    while (true)
    {
        uint32_t v1, v2;
        infile >> v1 >> v2;
        if (infile.eof()) break;
        if (v1 > v2) std::swap(v1, v2);
        edges_.push_back({v1, v2});
        n_ = (n_ > v2) ? n_ : v2;
        ASSERT_MSG(v1 < v2, "loops exist in the graph");
    }
    infile.close();
    m_ = edges_.size();
    //printf("DEBUG max n: %d\n", n_);
    ++n_;
    //printf("DEBUG init last: (%d, %d)\n", edges_[m_ - 1].first, edges_[m_ - 1].second);

  const auto beg = std::chrono::steady_clock::now();
  std::sort(edges_.begin(), edges_.end());
  ASSERT_MSG(std::unique(edges_.begin(), edges_.end()) == edges_.end(),
             "multi-edges exist in the graph");
    //printf("DEBUG after sort last: (%d, %d) n: %d\n", edges_[m_ - 1].first, edges_[m_ - 1].second, n_);
  // initialize adjacency arrays
  adj_.resize(n_);
  for (uint32_t eid = 0; eid < m_; ++eid) {
    const uint32_t v1 = edges_[eid].first;
    const uint32_t v2 = edges_[eid].second;
    adj_[v1].push_back({v2, eid});
    adj_[v2].push_back({v1, eid});
  }
  m_dMax = 0;
  for (uint32_t vid = 0; vid < n_; ++vid) {
    //adj_[vid].shrink_to_fit();
    m_dMax = (m_dMax > adj_[vid].size()) ? m_dMax : adj_[vid].size();
    std::sort(adj_[vid].begin(), adj_[vid].end(),
              [](const ArrayEntry& ae1, const ArrayEntry& ae2) {
                return ae1.vid < ae2.vid;
              });
  }
  // truss decomposition
  // 1. compute the support of each edge by triangle listing
  // 1.1. define a total order over the vertices
  const auto pred = [this](const uint32_t v1, const uint32_t v2) {
    const size_t deg1 = adj_[v1].size();
    const size_t deg2 = adj_[v2].size();
    if (deg1 != deg2) return deg1 > deg2;
    else return v1 > v2;
  };
  // 1.2. sort the vertices in non-ascending order of degree
  std::vector<uint32_t> verts(n_);
  std::iota(verts.begin(), verts.end(), 0);
  std::sort(verts.begin(), verts.end(), pred);
  // 1.3. call the "forward" algorithm to list triangles
  const auto begCnt = std::chrono::steady_clock::now();
  m_Sup.resize(m_, 0);
  std::vector<std::vector<ArrayEntry>> A(n_);
  for (const uint32_t v : verts) {
    for (const auto ae : adj_[v]) {
      const uint32_t u = ae.vid;
      const uint32_t e = ae.eid;
      if (!pred(v, u)) continue;
      size_t pv = 0, pu = 0;
      while (pv < A[v].size() && pu < A[u].size()) {
        if (A[v][pv].vid == A[u][pu].vid) {
          ++m_Sup[A[v][pv].eid]; ++m_Sup[A[u][pu].eid];
          ++m_Sup[e];
          ++pv; ++pu;
        } else if (pred(A[v][pv].vid, A[u][pu].vid)) {
          ++pv;
        } else {
          ++pu;
        }
      }
      A[u].push_back({v, e});
    }
  }
  decltype(A)().swap(A);
  decltype(verts)().swap(verts);

  const auto endCnt = std::chrono::steady_clock::now();
  const auto difCnt = endCnt - begCnt;
  printf("Counting costs \x1b[1;31m%f\x1b[0m ms.\n",
         std::chrono::duration<double, std::milli>(difCnt).count());
  // 2. decomposition
  // 2.1. sort the edges according to their supports
  const uint32_t max_sup = *std::max_element(m_Sup.cbegin(), m_Sup.cend());
  std::vector<std::vector<uint32_t>> bin(max_sup + 1);
  for (uint32_t eid = 0; eid < m_; ++eid) bin[m_Sup[eid]].push_back(eid);

  //printf("start peeling\n");
  m_t.resize(m_, 0);
  m_L.resize(m_, 0);
  m_SeSup.resize(m_, 0);
  // 2.2. peeling
  std::vector<bool> removed(m_, false);
  uint32_t uiK = 2;
  uint32_t uiL = 1;
  uint32_t uiRmCnt = 0;
  std::vector<uint32_t> vWait(m_);
  // 2.2.1. process the edges layer by layer
    while (uiRmCnt < m_)
    {
        vWait.clear();
        //printf("start current k: %d L: %d\n", uiK, uiL);
        for (uint32_t i = 0; i <= uiK - 2; ++i)
        {
          for (auto eid : bin[i])
          {
            /*printf("start current (%d, %d) sup: %d\n",
                   edges_[eid].first, edges_[eid].second, i);*/
                /* avoid repetition */
              if ((!removed[eid]) && (uiK != m_t[eid]))
              {
                  vWait.push_back(eid);
                  m_t[eid] = uiK;
                  m_L[eid] = uiL;
                  m_SeSup[eid] = i;
              }
          }
          bin[i].clear();
        }
        ++uiL;

        if (vWait.empty())
        {
          ++uiK;
          uiL = 1;
          continue;
        }

        for (auto eid : vWait)
        {
        removed[eid] = true;
        ++uiRmCnt;
        //printf("start find triangles\n");
        // find triangles containing the edge with ID eid
        std::vector<std::pair<uint32_t, uint32_t>> tris;
        {
          const uint32_t v1 = edges_[eid].first;
          const uint32_t v2 = edges_[eid].second;
          size_t p1 = 0, p2 = 0;
          while (p1 < adj_[v1].size() && p2 < adj_[v2].size()) {
            if (adj_[v1][p1].vid == adj_[v2][p2].vid) {
              tris.push_back({adj_[v1][p1].eid, adj_[v2][p2].eid});
              ++p1; ++p2;
            } else if (adj_[v1][p1].vid < adj_[v2][p2].vid) {
              ++p1;
            } else {
              ++p2;
            }
          }
        }

        //printf("start rm triangles\n");
        for (const auto tri : tris)
        {
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            if (removed[e1] || removed[e2]) continue;

            for (const uint32_t e : {e1, e2})
            {
                --m_Sup[e];
                bin[m_Sup[e]].push_back(e);
            }
        }
        }

    }
    m_kMax = uiK;

  // clear adj_
  decltype(adj_)().swap(adj_);

  const auto end = std::chrono::steady_clock::now();
  const auto dif = end - beg;
  printf("Indexing costs \x1b[1;31m%f\x1b[0m ms.\n",
         std::chrono::duration<double, std::milli>(dif).count());
    //printf("DEBUG index done last: (%d, %d)\n", edges_[m_ - 1].first, edges_[m_ - 1].second);
}
void Decomp::WriteToFile(char* pcFile) const
{
    char sFirstLine[100] = {0};
    char *pcData = NULL;
    SaveEntry *pstData = NULL;
    uint32_t uMaxBuffer = 100000000;

    //printf("DEBUG before save last: (%d, %d)\n", edges_[m_ - 1].first, edges_[m_ - 1].second);

    /* fill data */
    sprintf(sFirstLine, "c,%d\nP,%d\nE,%d\nD,%d\nK,%d\n", m_ + 1000, n_ - 1, m_, m_dMax, m_kMax);
    pcData = (char *)malloc(m_ * sizeof(SaveEntry));
    ASSERT_MSG(NULL != pcData, "MALLOC error");
    pstData = (SaveEntry *)pcData;
    for (uint32_t eid = 0; eid < m_; ++eid)
    {
        pstData[eid].x = edges_[eid].first;
        pstData[eid].y = edges_[eid].second;
        pstData[eid].k = m_t[eid];
        pstData[eid].L = m_L[eid];
        pstData[eid].SeSup = m_SeSup[eid];
    }

    /* write */
    int fd = open(pcFile, O_CREAT|O_WRONLY|O_TRUNC, 0644);

    int ret = write(fd, sFirstLine, 100 * sizeof(char));
    ASSERT_MSG(100 == ret, "Write error " << ret);

    long long llCurPos = 0;
    long long llTotoalSize = (long long)m_ * sizeof(SaveEntry);
    uint32_t uStep = llTotoalSize;
    while (llCurPos < llTotoalSize)
    {
        if (uStep <= uMaxBuffer)
        {
            ret = write(fd, pcData + llCurPos, uStep * sizeof(char));
            ASSERT_MSG(uStep == ret, "Write error " << ret);
            llCurPos += uStep;

            uStep = uStep < (llTotoalSize - llCurPos)? uStep : (llTotoalSize - llCurPos);
        }
        else
        {
            uStep = uMaxBuffer;
            ASSERT(1 < uStep);
        }
    }
    close(fd);
    free(pcData);
    pcData = NULL;
    pstData = NULL;

    //outfile.close();
    //printf("end save file\n");
}
void Decomp::PrintFile(char* pcFile)
{
    char sFirstLine[100] = {0};
    char *pcData = NULL;
    SaveEntry *pstData = NULL;
    uint32_t uMaxBuffer = 100000000;

    /* read */
    int fd = open(pcFile, O_RDONLY);
    uint32_t ret = read(fd, sFirstLine, 100 * sizeof(char));
    ASSERT_MSG(100 == ret, "Read error " << ret);
    int i_c, i_P, i_m, i_D, i_K;
    sscanf(sFirstLine, "c,%d\nP,%d\nE,%d\nD,%d\nK,%d\n", &i_c, &i_P, &i_m, &i_D, &i_K);
    printf(sFirstLine);

    long long llTotoalSize = ((long long)i_m) * sizeof(SaveEntry);
    //printf("DEBUG signle: %d, totoal size: %lld\n", sizeof(SaveEntry), llTotoalSize);
    /* fill data */
    pcData = (char *)malloc(llTotoalSize);
    ASSERT_MSG(NULL != pcData, "MALLOC error");
    pstData = (SaveEntry *)pcData;

    long long llCurPos = 0;
    uint32_t uStep = llTotoalSize;
    while (llCurPos < llTotoalSize)
    {
        if (uStep <= uMaxBuffer)
        {
            ret = read(fd, pcData + llCurPos, uStep * sizeof(char));
            ASSERT_MSG(uStep == ret, "Read error " << uStep << " get: " << ret);
            llCurPos += uStep;

            /*int iDebugPos = llCurPos / sizeof(SaveEntry);
            printf("DEBUG pos: %lld step: %d last[%d]: (%d, %d)\n", llCurPos, uStep,
                   iDebugPos - 1, pstData[iDebugPos - 1].x, pstData[iDebugPos - 1].y);*/

            uStep = uStep < (llTotoalSize - llCurPos)? uStep : (llTotoalSize - llCurPos);
        }
        else
        {
            uStep = uMaxBuffer;
            ASSERT(1 < uStep);
        }
    }
    close(fd);

    /*printf("DEBUG signle: %d, totoal size: %d print last: (%d, %d)\n", sizeof(SaveEntry), llTotoalSize,
           pstData[i_m - 1].x, pstData[i_m - 1].y);*/

    printf("n,%d\n", 0);
    printf("t,%d\n", 0);
    printf("L,%d\n", 0);
    printf("S,%d\n", 0);
    printf("x,%d\n", 0);
    printf("y,%d\n", 0);
    for (uint32_t eid = 0; eid < i_m; ++eid)
    {
        printf("n,%d\n", eid + 1);
        printf("t,%d\n", pstData[eid].k);
        printf("L,%d\n", pstData[eid].L);
        printf("S,%d\n", pstData[eid].SeSup);
        printf("x,%d\n", pstData[eid].x);
        printf("y,%d\n", pstData[eid].y);
    }
    free(pcData);
    pcData = NULL;
    pstData = NULL;

    //outfile.close();
    //printf("end save file\n");
}
}  // namespace decomp
}  // namespace truss_maint
