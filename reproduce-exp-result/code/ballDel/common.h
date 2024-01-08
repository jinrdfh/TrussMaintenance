#pragma once

using namespace std;

#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <list>
#include <map>
#include <utility>
#include <algorithm>
#include <vector>
#include <memory.h>
#include <unordered_map>
#include <climits>

#define DEBUG_PRINT_BTN
#define DEBUG_ASSERT_BTN

#include "tool/debug.h"
#include "tool/myIntAHash.h"
#include "tool/myAHash.h"
#include "tool/myPriQueue.h"


#define common_max(valA, valB) ((valA>valB)?valA:valB)
#define common_min(valA, valB) ((valA<valB)?valA:valB)
#define COMMON_MAX(valA, valB) ((valA>valB)?valA:valB)
#define COMMON_MIN(valA, valB) ((valA<valB)?valA:valB)

#define COMMON_INF(infFlag, valA, valB) ((infFlag)?valA:valB)
//#define COMMON_TRG_MIN(self, valA, infA, valB, infB) ((infA)?((infB)?(self+1):valB):((infB)?valA:COMMON_MIN(valA, valB)))
#define COMMON_TRG_MIN(self, valA, infA, valB, infB) COMMON_INF(infA, COMMON_INF(infB, self+1, valB), COMMON_INF(infB, valA, COMMON_MIN(valA, valB)))

#define COMMON_TRI_PAIR(pairA, pairB, pairC) (pair<pair<int, int>, int>(pair<int, int>(pairA, pairB), pairC))

