#pragma once
/***************
my list class
add function: insert by order
****************/
#include <stdio.h>
#include <list>

#define MY_INT_A_USED_FLAG 1
using namespace std;

class myIntA
{
    typedef struct pairNode{
        int  DyKey;
        /* 0 not used; 1 used */
        int iUsed;
        list <int>::iterator itDyList;
    }PAIR_NODE;
    private:
        int m_iDyKeyLen;
        int m_iFixKeyLen;
        int m_iDyTop;
        int m_iEleCnt;
        list <int> ** m_aDyList;
        PAIR_NODE* m_aFixPair;

    public:
        myIntA(int iDyKeyLen, int iFixKeyLen)
        {
            m_iDyKeyLen = iDyKeyLen;
            m_iFixKeyLen = iFixKeyLen;
            m_iEleCnt = 0;

            m_aDyList = (list <int> **)malloc(iDyKeyLen * sizeof(list <int> *));
            debug_assert(NULL != m_aDyList);
            memset(m_aDyList, 0, iDyKeyLen * sizeof(list <int> *));

            m_aFixPair = (PAIR_NODE *)malloc(iFixKeyLen * sizeof(PAIR_NODE));
            debug_assert(NULL != m_aFixPair);
            memset(m_aFixPair, 0, iFixKeyLen * sizeof(PAIR_NODE));
        }
        ~myIntA()
        {
            free(m_aFixPair);
            m_aFixPair = NULL;

            for (int i = 0; i < m_iDyKeyLen; i++)
            {
                if (0 != m_aDyList[i])
                {
                    m_aDyList[i]->clear();
                    delete m_aDyList[i];
                }
            }
            free(m_aDyList);
            m_aDyList = NULL;
        }
        bool empty()
        {
            if (0 < m_iEleCnt)
            {
                return false;
            }
            return true;
        }
        /**
        return true: new insert; false: reinsert
        **/
        bool insetByOrder(int DyKey, int FixKey)
        {
            bool bNewFlag = true;
            list <int>::iterator itTpDyNode;
            list <int> lsTp;
            if (DyKey >= m_iDyKeyLen)
            {
                printf("ERROR get dyKey: %d max: %d FixKey: %d\n",
                       DyKey, m_iDyKeyLen, FixKey);
                debug_assert(0);
            }
            debug_assert(FixKey < m_iFixKeyLen);

            m_iDyTop = m_iDyTop < DyKey?m_iDyTop:DyKey;

            if (0 == m_aFixPair[FixKey].iUsed)
            {
                /* new node */
                bNewFlag = true;
                m_aFixPair[FixKey].iUsed = MY_INT_A_USED_FLAG;
                ++m_iEleCnt;
            }
            else
            {
                /* reinsert */
                bNewFlag = false;
                if (DyKey == m_aFixPair[FixKey].DyKey)
                {
                    /* same ignore */
                    return bNewFlag;
                }
                else
                {
                    /* erase old node */
                    m_aDyList[m_aFixPair[FixKey].DyKey]->erase(m_aFixPair[FixKey].itDyList);
                }
            }
            m_aFixPair[FixKey].DyKey = DyKey;

            if (0 == m_aDyList[DyKey])
            {
                /* new dynamic node */
                m_aDyList[DyKey] = new list <int>;
            }
            m_aDyList[DyKey]->push_front(FixKey);
            m_aFixPair[FixKey].itDyList = m_aDyList[DyKey]->begin();

            return bNewFlag;
        }

        /* get fixed key */
        int getTop()
        {
            int FixTpVal = 0;
            for (int i = m_iDyTop; i < m_iDyKeyLen; ++i)
            {
                if (0 == m_aDyList[i])
                {
                    /* blank, ignore */
                    continue;
                }
                if (m_aDyList[i]->empty())
                {
                    /* empty, ignore */
                    continue;
                }
                FixTpVal = *(m_aDyList[i]->begin());

                m_iDyTop = i;
                break;
            }
            return FixTpVal;
        }
        /* get dynamic key */
        int getDyKey(int FixKey)
        {
            debug_assert(FixKey < m_iFixKeyLen);

            return m_aFixPair[FixKey].DyKey;
        }

        bool getTopList(list <int> &lsDes)
        {
            list <int> *plsFix = NULL;
            list <int>::iterator itlst;
            debug_assert(lsDes.empty());
            for (int i = m_iDyTop; i < m_iDyKeyLen; ++i)
            {
                if (0 == m_aDyList[i])
                {
                    /* blank, ignore */
                    continue;
                }
                if (m_aDyList[i]->empty())
                {
                    /* empty, ignore */
                    continue;
                }
                m_iDyTop = i;
                plsFix = m_aDyList[i];
                for (itlst = plsFix->begin(); itlst != plsFix->end(); ++itlst)
                {
                    lsDes.push_back(*itlst);
                }
                break;
            }
            return true;
        }

        bool myDel(int FixKey)
        {
            int DyKey;
            list <int> * plsDyList = NULL;

            debug_assert(FixKey < m_iFixKeyLen);
            if (0 == m_aFixPair[FixKey].iUsed)
            {
                /* blank, error */
                return false;
            }
            DyKey = m_aFixPair[FixKey].DyKey;
            plsDyList = m_aDyList[DyKey];
            if (m_aFixPair[FixKey].itDyList == plsDyList->end())
            {
                /* already removed, error */
                return false;
            }
            plsDyList->erase(m_aFixPair[FixKey].itDyList);
            --m_iEleCnt;
            m_aFixPair[FixKey].itDyList = plsDyList->end();
            m_aFixPair[FixKey].iUsed = 0;
            return true;
        }
};









