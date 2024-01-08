#pragma once

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define DEBUG_BUFFER_SIZE 100
#define MY_DEBUG

#ifdef DEBUG_PRINT_BTN
    //��־��ʾ(�ļ������к�}+����
    #define DEBUG_LOG(s) printf("[%s:%d] %s\n", __FILE__, __LINE__, s)
	//������Ϣλ�ã�����+�ļ���+������+�к�+������Ϣ)
    #define DEBUG_ERROR(s)                                                                      \
	do{																					  \
	fprintf(stderr, "[ERROR  at:]%s %s(Line %d): %s\n",__FILE__,__FUNCTION__,__LINE__,s); \
	}while(0)

	#define DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)

#else
    //��־����ʾ
    #define DEBUG_LOG(s)  NULL
    //������Ϣ����ʾ
    #define DEBUG_ERROR(s)          NULL
    #define DEBUG_PRINTF(fmt, ...)          NULL
#endif

#ifdef MY_DEBUG
#ifdef DEBUG_ASSERT_BTN
    #define DEBUG_ASSERT(bTure)     \
    {     \
        int nptrs;     \
        void *buffer[DEBUG_BUFFER_SIZE];     \
        if (!(bTure))     \
        {     \
            printf("file: %s---line: %d\n",__FILE__, __LINE__);           \
            nptrs = backtrace(buffer, DEBUG_BUFFER_SIZE);     \
            printf("backtrace() returned %d addresses\n", nptrs);     \
            backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);     \
            exit(0);\
        }     \
    }
    #define debug_assert(bTure)     \
    {     \
        int nptrs;     \
        void *buffer[DEBUG_BUFFER_SIZE];     \
        if (!(bTure))     \
        {     \
            printf("file: %s---line: %d\n",__FILE__, __LINE__);           \
            nptrs = backtrace(buffer, DEBUG_BUFFER_SIZE);     \
            printf("backtrace() returned %d addresses\n", nptrs);     \
            backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);     \
            exit(0);\
        }     \
    }

#else
    #define DEBUG_ASSERT(s)          NULL
    #define debug_assert(s)          NULL
#endif
#else

#define DEBUG_ASSERT(s)          assert(s)
#define debug_assert(s)          assert(s)

#ifndef DEBUG_ASSERT_BTN
    #define NDEBUG
#endif
#endif
