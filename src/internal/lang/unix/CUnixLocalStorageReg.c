//
// Created by xy on 7/18/22.
//

#include <xcl/lang/CLocalStorageReg.h>
#include <pthread.h>
#include <assert.h>
#include "xcl/util/CList.h"
#include "xcl/lang/XclErr.h"
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#include <bits/sigthread.h>
#include <errno.h>

typedef CList __RegList;

typedef struct {
    pthread_mutex_t mutex;
    __RegList* regList;
} __CLocalStorageRegQueue;

static __CLocalStorageRegQueue __localStorageRegQueue;

void __initializeRegQueue()
{
    assert(pthread_mutex_init(&__localStorageRegQueue.mutex, NULL) == 0);
    __localStorageRegQueue.regList = List_new();
    assert(__localStorageRegQueue.regList);
}

int32_t __Unix_cmpThreadId(const void* left, const void* right)
{
    return (int32_t)(*(const long*)left - *(const long*)right);
}

void __regLocalStorage(CLocalStorage* localStorage)
{
    pthread_mutex_lock(&__localStorageRegQueue.mutex);
    CListNode* node = malloc(sizeof(CListNode) + sizeof(__RegData));
    if (node)
    {
        __RegData* regData = (__RegData*)(node->data);
        regData->localStorage = localStorage;
        regData->threadId = syscall(__NR_gettid);
        CListIter iter = List_query(__localStorageRegQueue.regList,
                                    &regData->threadId, __Unix_cmpThreadId);
        assert(List_iterEquals(iter, List_end(__localStorageRegQueue.regList)));
        List_push(__localStorageRegQueue.regList, node);
    }
    else
    {
        setErr(XCL_MEMORY_ERR);
        assert(false);
    }
    pthread_mutex_unlock(&__localStorageRegQueue.mutex);
}

void __deregisterLocalStorage(ThreadId tid)
{
    pthread_mutex_lock(&__localStorageRegQueue.mutex);
    CListIter iter = List_begin(__localStorageRegQueue.regList);
    while (!List_iterEquals(iter, List_end(__localStorageRegQueue.regList)))
    {
        if (((__RegData*)iter.cur->data)->threadId == tid)
        {
            List_remove(__localStorageRegQueue.regList, iter);
            break;
        }
        iter = List_next(iter);
    }
    pthread_mutex_unlock(&__localStorageRegQueue.mutex);
}

static bool __isThreadAlive(long threadId)
{
    pthread_t handle = pthread_self();
    return pthread_kill(handle, 0) != ESRCH;
}

void __clearObsoleteStorages()
{
    pthread_mutex_lock(&__localStorageRegQueue.mutex);
    CListIter iter = List_begin(__localStorageRegQueue.regList);
    while (!List_iterEquals(iter, List_end(__localStorageRegQueue.regList)))
    {
        __RegData* data = (__RegData*)iter.cur->data;
        if (!__isThreadAlive(data->threadId))
        {
            CListIter next = List_remove(__localStorageRegQueue.regList, iter);
            LocalStorage_free(data->localStorage);
            free(iter.cur);
            iter = next;
        }
        else { iter = List_next(iter); }
    }
    pthread_mutex_unlock(&__localStorageRegQueue.mutex);
}

bool __hasRegLocalStorage()
{
    pthread_mutex_lock(&__localStorageRegQueue.mutex);
    bool ret = !List_empty(__localStorageRegQueue.regList);
    pthread_mutex_unlock(&__localStorageRegQueue.mutex);
    return ret;
}