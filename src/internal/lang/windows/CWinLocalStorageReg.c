//
// Created by xuyan on 2022/7/18.
//

#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CLocalStorageReg.h"
#include "xcl/lang/XclErr.h"
#include "xcl/util/CList.h"
#include <assert.h>
#include <windows.h>

typedef CList __RegList;

typedef struct {
    CRITICAL_SECTION mutex;
    __RegList* regList;
} __CLocalStorageRegQueue;

static __CLocalStorageRegQueue __localStorageRegQueue;

void __initializeRegQueue()
{
    InitializeCriticalSection(&__localStorageRegQueue.mutex);
    __localStorageRegQueue.regList = List_new();
    assert(__localStorageRegQueue.regList);
}

int32_t __Win32_cmpThreadId(const void* left, const void* right)
{
    return (int32_t)(*(const DWORD*)left - *(const DWORD*)right);
}

void __regLocalStorage(CLocalStorage* localStorage)
{
    EnterCriticalSection(&__localStorageRegQueue.mutex);
    CListNode* node = malloc(sizeof(CListNode) + sizeof(__RegData));
    if (node)
    {
        __RegData* regData = (__RegData*)(node->data);
        regData->localStorage = localStorage;
        regData->threadId = GetCurrentThreadId();
        CListIter iter = List_query(__localStorageRegQueue.regList,
                                    &regData->threadId, __Win32_cmpThreadId);
        assert(List_iterEquals(iter, List_end(__localStorageRegQueue.regList)));
        List_push(__localStorageRegQueue.regList, node);
    }
    else
    {
        setErr(XCL_MEMORY_ERR);
        assert(false);
    }
    LeaveCriticalSection(&__localStorageRegQueue.mutex);
}

void __deregisterLocalStorage(ThreadId tid)
{
    EnterCriticalSection(&__localStorageRegQueue.mutex);
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
    LeaveCriticalSection(&__localStorageRegQueue.mutex);
}

static bool __isThreadAlive(DWORD threadId)
{
    HANDLE handle = OpenThread(THREAD_QUERY_INFORMATION, false, threadId);
    if (!handle)
    {
        setErr(GetLastError());
        return false;
    }
    DWORD exit;
    if (!GetExitCodeThread(handle, &exit))
    {
        setErr(GetLastError());
        return false;
    }
    return exit == STILL_ACTIVE;
}

void __clearObsoleteStorages()
{
    EnterCriticalSection(&__localStorageRegQueue.mutex);
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
    LeaveCriticalSection(&__localStorageRegQueue.mutex);
}

bool __hasRegLocalStorage()
{
    EnterCriticalSection(&__localStorageRegQueue.mutex);
    bool ret = !List_empty(__localStorageRegQueue.regList);
    LeaveCriticalSection(&__localStorageRegQueue.mutex);
    return ret;
}
