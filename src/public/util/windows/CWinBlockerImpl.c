//
// Created by xuyan on 2022/7/18.
//

#include <windows.h>
#include <xcl/util/CBlockerInternalApi.h>
#include <synchapi.h>
#include <stdlib.h>
#include "xcl/lang/XclErr.h"

typedef struct {
    CBlocker base;
    CRITICAL_SECTION* criticalSection;
    CONDITION_VARIABLE cond;
} CWinBlocker;

static inline CRITICAL_SECTION* __WinBlocker_mutex(CBlocker* blocker)
{
    return ((CWinBlocker*)blocker)->criticalSection;
}

static inline CONDITION_VARIABLE* __WinBlocker_cond(CBlocker* blocker)
{
    return &((CWinBlocker*)blocker)->cond;
}

CBlocker* __Blocker_alloc() { return malloc(sizeof(CWinBlocker)); }

void __Blocker_dealloc(CBlocker* blocker) { free(blocker); }

int32_t __Blocker_init(CBlocker* blocker, void* mutex)
{
    int ret;
    CWinBlocker* winBlocker = (CWinBlocker*)blocker;
    if (!mutex)
    {
        winBlocker->criticalSection = malloc(sizeof(CRITICAL_SECTION));
        if (winBlocker->criticalSection)
        {
            InitializeCriticalSection(winBlocker->criticalSection);
            InitializeConditionVariable(&winBlocker->cond);
            return 0;
        }
        ret = XCL_MEMORY_ERR;
    }
    else
    {
        winBlocker->criticalSection = mutex;
        InitializeConditionVariable(&winBlocker->cond);
        return 0;
    }
    return ret;
}

int32_t __Blocker_acquire(CBlocker* blocker)
{
    EnterCriticalSection(__WinBlocker_mutex(blocker));
    return 0;
}

int32_t __Blocker_release(CBlocker* blocker)
{
    LeaveCriticalSection(__WinBlocker_mutex(blocker));
    return 0;
}

int32_t __Blocker_wait(CBlocker* blocker)
{
    SleepConditionVariableCS(__WinBlocker_cond(blocker),
                             __WinBlocker_mutex(blocker), INFINITE);
    return 0;
}

int32_t __Blocker_notify(CBlocker* blocker)
{
    WakeConditionVariable(__WinBlocker_cond(blocker));
    return 0;
}

int32_t __Blocker_notifyAll(CBlocker* blocker)
{
    WakeAllConditionVariable(__WinBlocker_cond(blocker));
    return 0;
}

void __Blocker_finalize(CBlocker* blocker)
{
    if (!blocker->externalLock)
    {
        DeleteCriticalSection(__WinBlocker_mutex(blocker));
        free(__WinBlocker_mutex(blocker));
    }
}
