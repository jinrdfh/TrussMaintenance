#pragma once
/***************
my list class
add function: insert by order
****************/
#include <stdio.h>
#include <vector>
#include <queue>

using namespace std;

class myPriQueue
{
private:
struct stData
{
    int iDy;
    int iFix;
    stData(int a, int b)
    {
        iDy = a;
        iFix = b;
    }
    bool operator<(const stData& a) const
    {
        return iDy > a.iDy;
    }
};
    priority_queue<stData> m_prQ;

public:
    bool empty()
    {
        return m_prQ.empty();
    }
    /**
    return true: new insert; false: reinsert
    **/
    bool insetByOrder(int DyKey, int FixKey)
    {
        m_prQ.emplace(DyKey, FixKey);
        return true;
    }

    /* merge 2 keys */
    int mergeKey(int iKey1, int iKey2)
    {
        DEBUG_ASSERT(iKey1 <= 0x3fffffff);
        DEBUG_ASSERT(iKey2 <= 0x1);
        return (iKey1 << 1) + iKey2;
    }
    /* get fixed key */
    int getTop()
    {
        int iTpVal = m_prQ.top().iFix;
        return iTpVal;
    }
    /* get dynamic key */
    int getDy()
    {
        int iTpVal = m_prQ.top().iDy;
        return iTpVal;
    }
    bool popAll(vector <int> &vRes)
    {
        DEBUG_ASSERT(vRes.empty());
        while (!m_prQ.empty())
        {
            vRes.push_back(m_prQ.top().iFix);
            m_prQ.pop();
        }
        return true;
    }
    bool pop()
    {
        m_prQ.pop();
        return true;
    }
};


class myPriQueueBig
{
private:
struct stData
{
    int iDy;
    int iFix;
    stData(int a, int b)
    {
        iDy = a;
        iFix = b;
    }
    bool operator<(const stData& a) const
    {
        return iDy < a.iDy;
    }
};
    priority_queue<stData> m_prQ;

public:
    bool empty()
    {
        return m_prQ.empty();
    }
    /**
    return true: new insert; false: reinsert
    **/
    bool insetByOrder(int DyKey, int FixKey)
    {
        m_prQ.emplace(DyKey, FixKey);
        return true;
    }

    /* merge 2 keys */
    int mergeKey(int iKey1, int iKey2)
    {
        DEBUG_ASSERT(iKey1 <= 0x3fffffff);
        DEBUG_ASSERT(iKey2 <= 0x1);
        return (iKey1 << 1) + iKey2;
    }
    /* get fixed key */
    int getTop()
    {
        int iTpVal = m_prQ.top().iFix;
        return iTpVal;
    }
    /* get dynamic key */
    int getDy()
    {
        int iTpVal = m_prQ.top().iDy;
        return iTpVal;
    }
    bool popAll(vector <int> &vRes)
    {
        DEBUG_ASSERT(vRes.empty());
        while (!m_prQ.empty())
        {
            vRes.push_back(m_prQ.top().iFix);
            m_prQ.pop();
        }
        return true;
    }
    bool pop()
    {
        m_prQ.pop();
        return true;
    }
};







