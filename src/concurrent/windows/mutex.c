//
// Created by xuyan on 2022/7/4.
//

#include "concurrent/mutex.h"
#include "lang/xclerr.h"
#include "lang/system.h"
#include "pool/pool.h"
#include <Windows.h>

struct _CMutex_st {
    CRITICAL_SECTION criticalSection;
};

bool
__Mutex_init(CMutex* mutex)
{
    InitializeCriticalSection(&mutex->criticalSection);
    return true;
}

CMutex*
__Mutex_newByPool(CPool* pool)
{
    CMutex* mutex = (CMutex*)Pool_alloc(pool, sizeof(CMutex));
    if (mutex)
    {
        __Mutex_init(mutex);
    }
    else
    {
        Err_set(XCL_MEMORY_ERR);
    }
    return mutex;
}

XCL_EXPORT CMutex* XCL_API
Mutex_new()
{
    return __Mutex_newByPool(Pool_def());
}

XCL_EXPORT bool XCL_API
Mutex_delete(CMutex* mutex)
{
    if (mutex)
    {
        DeleteCriticalSection(&mutex->criticalSection);
        Pool_dealloc(Pool_def(), mutex, sizeof(CMutex));
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Mutex_lock(CMutex* mutex)
{
    if (mutex)
    {
        EnterCriticalSection(&mutex->criticalSection);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Mutex_unlock(CMutex* mutex)
{
    if (mutex)
    {
        LeaveCriticalSection(&mutex->criticalSection);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Mutex_tryLock(CMutex* mutex)
{
    if (!mutex)
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    else
    {
        return TryEnterCriticalSection(&mutex->criticalSection);
    }
}

XCL_EXPORT bool XCL_API
Mutex_tryLock2(CMutex* mutex, int32_t millis)
{
    if (!mutex)
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    int64_t st = nanos();
    timeBeginPeriod(1);
    int64_t totalWait = 0;
    int64_t nanoTimeout = millis * 1000000;
    bool acquired = false;
    while ((nanoTimeout - totalWait) > 500000)
    {
        if (TryEnterCriticalSection(&mutex->criticalSection))
        {
            acquired = true;
            break;
        }
        Sleep(1);
        totalWait = nanos() - st;
    }
    timeEndPeriod(1);
    if (!acquired)
    {
        acquired = TryEnterCriticalSection(&mutex->criticalSection);
    }
    return acquired;
}
