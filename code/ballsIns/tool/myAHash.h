#pragma once
/***************
my list class
add function: insert by order
****************/
#include <stdio.h>
#include <list>
#include <map>
#include <utility>
#include <unordered_map>

#define MY_INT_A_USED_FLAG 1

using namespace std;

template<typename _Tp>
class myA
{
    typedef struct pairNode{
        _Tp  DyKey;
        list <int>::iterator itDyList;
    }PAIR_NODE;
    private:
        int m_iFixKeyLen;
        int m_iEleCnt;
        map<_Tp, list <int> *> *m_pmpDyMap;
        unordered_map<int, PAIR_NODE> *m_pumpFixUMap;

    public:
        myA(int iFixKeyLen)
        {
            m_iFixKeyLen = iFixKeyLen;
            m_iEleCnt = 0;
            m_pmpDyMap = new map<_Tp, list <int> *>();
            m_pumpFixUMap = new unordered_map<int, PAIR_NODE>();
        }
        ~myA()
        {
            typename map<_Tp, list <int> *>::iterator itmpDy;

            for (itmpDy = m_pmpDyMap->begin(); itmpDy != m_pmpDyMap->end(); ++itmpDy)
            {
                itmpDy->second->clear();
                delete itmpDy->second;
            }
            m_pmpDyMap->clear();
            m_pumpFixUMap->clear();

            delete m_pmpDyMap;
            delete m_pumpFixUMap;
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
        bool insetByOrder(_Tp DyKey, int FixKey)
        {
            bool bNewFlag = true;
            typename map<_Tp, list <int> *>::iterator itmpDy;
            typename unordered_map<int, PAIR_NODE>::iterator itmpFix;
            PAIR_NODE stNode;

            debug_assert(FixKey < m_iFixKeyLen);

            itmpFix = m_pumpFixUMap->find(FixKey);
            if (itmpFix == m_pumpFixUMap->end())
            {
                /* new node */
                bNewFlag = true;
                ++m_iEleCnt;
                stNode.DyKey = DyKey;

                m_pumpFixUMap->insert(pair<int, PAIR_NODE>(FixKey, stNode));

                itmpFix = m_pumpFixUMap->find(FixKey);
                debug_assert(itmpFix != m_pumpFixUMap->end());
            }
            else
            {
                /* reinsert */
                bNewFlag = false;
                if (DyKey == itmpFix->second.DyKey)
                {
                    /* same ignore */
                    return bNewFlag;
                }
                else
                {
                    /* erase old node */
                    itmpDy = m_pmpDyMap->find(itmpFix->second.DyKey);
                    debug_assert(itmpDy != m_pmpDyMap->end());
                    itmpDy->second->erase(itmpFix->second.itDyList);
                    if (itmpDy->second->empty())
                    {
                        delete itmpDy->second;
                        m_pmpDyMap->erase(itmpDy);
                    }
                    itmpFix->second.DyKey = DyKey;
                }
            }

            itmpDy = m_pmpDyMap->find(DyKey);
            if (itmpDy == m_pmpDyMap->end())
            {
                /* new dynamic node */
                list <int> * plsNewLs = new list <int>();
                m_pmpDyMap->insert(pair<_Tp, list <int> *>(DyKey, plsNewLs));
                itmpDy = m_pmpDyMap->find(DyKey);
                debug_assert(itmpDy != m_pmpDyMap->end());
            }
            itmpDy->second->push_front(FixKey);
            itmpFix->second.itDyList = itmpDy->second->begin();

            return bNewFlag;
        }

        /* get fixed key */
        int getTop()
        {
            int FixTpVal = 0;
            typename map<_Tp, list <int> *>::iterator itmpDy;
            for (itmpDy = m_pmpDyMap->begin(); itmpDy != m_pmpDyMap->end(); ++itmpDy)
            {
                if (itmpDy->second->empty())
                {
                    /* empty, ignore */
                    continue;
                }
                FixTpVal = itmpDy->second->front();

                break;
            }
            return FixTpVal;
        }
        /* get dynamic key */
        int getDyKey(int FixKey)
        {
            typename unordered_map<int, PAIR_NODE>::iterator itmpFix;
            debug_assert(FixKey < m_iFixKeyLen);
            itmpFix = m_pumpFixUMap->find(FixKey);
            if (itmpFix == m_pumpFixUMap->end())
            {
                printf("TOOL ERROR fixed key: %d size: %d\n",
                       FixKey, m_pumpFixUMap->size());
                debug_assert(0);
            }

            return itmpFix->second.DyKey;
        }
        /* get dynamic key */
        bool inIt(int FixKey)
        {
            typename unordered_map<int, PAIR_NODE>::iterator itmpFix;
            debug_assert(FixKey < m_iFixKeyLen);
            itmpFix = m_pumpFixUMap->find(FixKey);
            if (itmpFix == m_pumpFixUMap->end())
            {
                return false;
            }

            return true;
        }
        bool onlyOne()
        {
            typename map<_Tp, list <int> *>::iterator itmpDy = m_pmpDyMap->begin();
            ++itmpDy;
            return (itmpDy == m_pmpDyMap->end());
        }
        bool getTopList(list <int> &lsDes)
        {
            list <int> *plsFix = NULL;
            list <int>::iterator itlst;
            typename map<_Tp, list <int> *>::iterator itmpDy;
            debug_assert(lsDes.empty());
            for (itmpDy = m_pmpDyMap->begin(); itmpDy != m_pmpDyMap->end(); ++itmpDy)
            {
                if (itmpDy->second->empty())
                {
                    /* empty, ignore */
                    continue;
                }
                plsFix = itmpDy->second;
                for (itlst = plsFix->begin(); itlst != plsFix->end(); ++itlst)
                {
                    lsDes.push_back(*itlst);
                }
                break;
            }
            return true;
        }
        bool getAll(list <pair<_Tp, int> > &lsRes)
        {
            typename unordered_map<int, PAIR_NODE>::iterator itFixNode;
            debug_assert(lsRes.empty());
            for (itFixNode = m_pumpFixUMap->begin(); itFixNode != m_pumpFixUMap->end(); ++itFixNode)
            {
                lsRes.push_back(pair<_Tp, int>(itFixNode->second.DyKey, itFixNode->first));
            }
            return true;
        }
        bool myDel(int FixKey)
        {
            _Tp DyKey;
            list <int> * plsDyList = NULL;
            typename map<_Tp, list <int> *>::iterator itmpDy;
            typename unordered_map<int, PAIR_NODE>::iterator itmpFix;

            debug_assert(FixKey < m_iFixKeyLen);
            itmpFix = m_pumpFixUMap->find(FixKey);
            if (itmpFix == m_pumpFixUMap->end())
            {
                /* blank, error */
                return false;
            }
            DyKey = itmpFix->second.DyKey;
            itmpDy = m_pmpDyMap->find(DyKey);
            debug_assert(itmpDy != m_pmpDyMap->end());
            plsDyList = itmpDy->second;
            if (itmpFix->second.itDyList == plsDyList->end())
            {
                /* already removed, error */
                return false;
            }
            plsDyList->erase(itmpFix->second.itDyList);
            --m_iEleCnt;
            itmpFix->second.itDyList = plsDyList->end();
            m_pumpFixUMap->erase(FixKey);
            if (plsDyList->empty())
            {
                delete plsDyList;
                m_pmpDyMap->erase(DyKey);
            }
            return true;
        }
};









