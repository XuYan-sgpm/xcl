//
// Created by xuyan on 2022/7/19.
//

#include <xcl/lang/CLocalStorageReg.h>
#include <assert.h>
#include <stdlib.h>
#include "xcl/util/CList.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/XclErr.h"
#include "xcl/pool/CPool.h"

typedef CList __RegList;

typedef struct {
    CMutex* mutex;
    __RegList* regList;
} __CLocalStorageRegQueue;

static __CLocalStorageRegQueue __localStorageRegQueue;

void __initializeRegQueue()
{
    __localStorageRegQueue.mutex = Mutex_new();
    assert(__localStorageRegQueue.mutex);
    __localStorageRegQueue.regList = List_new();
    assert(__localStorageRegQueue.regList);
}

int32_t __cmpThreadHandle(const void* left, const void* right)
{
    return (int32_t)(*(const ThreadHandle*)left - *(const ThreadHandle*)right);
}

void __regLocalStorage(CLocalStorage* localStorage)
{
    CListNode* node = malloc(sizeof(CListNode) + sizeof(__RegData));
    Mutex_lock(__localStorageRegQueue.mutex);
    if (node)
    {
        __RegData* regData = (__RegData*)(node->data);
        regData->localStorage = localStorage;
        regData->handle = __Thread_currentHandle();
        CListIter iter = List_query(__localStorageRegQueue.regList,
                                    &regData->handle, __cmpThreadHandle);
        assert(List_iterEquals(iter, List_end(__localStorageRegQueue.regList)));
        List_push(__localStorageRegQueue.regList, node);
    }
    else
    {
        setErr(XCL_MEMORY_ERR);
        assert(false);
    }
    Mutex_unlock(__localStorageRegQueue.mutex);
}

void __deregisterLocalStorage(ThreadHandle handle)
{
    Mutex_lock(__localStorageRegQueue.mutex);
    CListIter iter = List_begin(__localStorageRegQueue.regList);
    while (!List_iterEquals(iter, List_end(__localStorageRegQueue.regList)))
    {
        if (((__RegData*)iter.cur->data)->handle == handle)
        {
            List_remove(__localStorageRegQueue.regList, iter);
            free(iter.cur);
            break;
        }
        iter = List_next(iter);
    }
    Mutex_unlock(__localStorageRegQueue.mutex);
}

void __clearObsoleteStorages()
{
    Mutex_lock(__localStorageRegQueue.mutex);
    CListIter iter = List_begin(__localStorageRegQueue.regList);
    while (!List_iterEquals(iter, List_end(__localStorageRegQueue.regList)))
    {
        __RegData* data = (__RegData*)iter.cur->data;
        if (!__Thread_isAlive(data->handle))
        {
            CListIter next = List_remove(__localStorageRegQueue.regList, iter);
            LocalStorage_delete(data->localStorage);
            free(iter.cur);
            iter = next;
        }
        else
        {
            iter = List_next(iter);
        }
    }
    Mutex_unlock(__localStorageRegQueue.mutex);
}

bool __hasRegLocalStorage()
{
    Mutex_lock(__localStorageRegQueue.mutex);
    bool ret = !List_empty(__localStorageRegQueue.regList);
    Mutex_unlock(__localStorageRegQueue.mutex);
    return ret;
}
