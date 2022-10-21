//
// Created by xuyan on 2022/7/4.
//

#include "concurrent/mutex.h"
#include "lang/xcl_err.h"
#include "lang/system.h"
#include "pool/pool.h"
#include <Windows.h>

struct Mutex {
    CRITICAL_SECTION critical_section;
};

bool
__Mutex_init(Mutex* mutex)
{
    InitializeCriticalSection(&mutex->critical_section);
    return true;
}

Mutex*
__Mutex_newByPool(struct Pool* pool)
{
    Mutex* mutex = (Mutex*)Pool_alloc(pool, sizeof(Mutex));
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

XCL_EXPORT Mutex* XCL_API
Mutex_new()
{
    return __Mutex_newByPool(Pool_def());
}

XCL_EXPORT bool XCL_API
Mutex_delete(Mutex* mutex)
{
    if (mutex)
    {
        DeleteCriticalSection(&mutex->critical_section);
        Pool_dealloc(Pool_def(), mutex, sizeof(Mutex));
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Mutex_lock(Mutex* mutex)
{
    if (mutex)
    {
        EnterCriticalSection(&mutex->critical_section);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Mutex_unlock(Mutex* mutex)
{
    if (mutex)
    {
        LeaveCriticalSection(&mutex->critical_section);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Mutex_tryLock(Mutex* mutex)
{
    if (!mutex)
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    else
    {
        return TryEnterCriticalSection(&mutex->critical_section);
    }
}

XCL_EXPORT bool XCL_API
Mutex_tryLock2(Mutex* mutex, int32_t millis)
{
    if (!mutex)
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    int64_t st = nanos();
    timeBeginPeriod(1);
    int64_t total_wait = 0;
    int64_t nano_timeout = millis * 1000000;
    bool acquired = false;
    while ((nano_timeout - total_wait) > 500000)
    {
        if (TryEnterCriticalSection(&mutex->critical_section))
        {
            acquired = true;
            break;
        }
        Sleep(1);
        total_wait = nanos() - st;
    }
    timeEndPeriod(1);
    if (!acquired)
    {
        acquired = TryEnterCriticalSection(&mutex->critical_section);
    }
    return acquired;
}
