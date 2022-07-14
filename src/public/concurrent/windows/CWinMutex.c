//
// Created by xuyan on 2022/7/4.
//

#include <Windows.h>
#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/system.h"
#include "xcl/lang/XclErr.h"

typedef struct
{
    CRITICAL_SECTION criticalSection;
} CWinMutex;

XCL_PUBLIC(void*)
Mutex_new()
{
    CWinMutex*mutex = (CWinMutex*)malloc(sizeof(CWinMutex));
    if (mutex)
    {
        InitializeCriticalSection(&mutex->criticalSection);
    }
    else
    {
        setErr(XCL_MEMORY_ERR);
    }
    return mutex;
}

XCL_PUBLIC(bool)
Mutex_delete(void*mutex)
{
    if (mutex)
    {
        DeleteCriticalSection(&((CWinMutex*)mutex)->criticalSection);
        return true;
    }
    else
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_PUBLIC(bool)
Mutex_lock(void*mutex)
{
    if (mutex)
    {
        EnterCriticalSection(&((CWinMutex*)mutex)->criticalSection);
        return true;
    }
    else
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_PUBLIC(bool)
Mutex_unlock(void*mutex)
{
    if (mutex)
    {
        LeaveCriticalSection(&((CWinMutex*)mutex)->criticalSection);
        return true;
    }
    else
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_PUBLIC(bool)
Mutex_tryLock(void*mutex)
{
    if (!mutex)
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
    else
    {
        return TryEnterCriticalSection(&((CWinMutex*)mutex)->criticalSection);
    }
}

XCL_PUBLIC(bool)
Mutex_tryLock2(void*mutex, int32_t millis)
{
    if (!mutex)
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
    int64_t st = nanos();
    timeBeginPeriod(1);
    int64_t totalWait = 0;
    int64_t nanoTimeout = millis * 1000000;
    bool acquired = false;
    while ((nanoTimeout - totalWait) > 500000)
    {
        if (TryEnterCriticalSection(&((CWinMutex*)mutex)->criticalSection))
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
        acquired =
            TryEnterCriticalSection(&((CWinMutex*)mutex)->criticalSection);
    }
    return acquired;
}
