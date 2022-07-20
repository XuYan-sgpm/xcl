//
// Created by xuyan on 2022/7/5.
//

#include <Windows.h>
#include "xcl/concurrent/CCond.h"
#include "xcl/lang/XclErr.h"
#include "xcl/pool/CPool.h"

struct _CCond_st {
    CONDITION_VARIABLE conditionVariable;
};

XCL_PUBLIC(CCond*)
Cond_new()
{
    CCond* cond = (CCond*)Pool_alloc(NULL, sizeof(CCond));
    if (cond)
    {
        InitializeConditionVariable(&cond->conditionVariable);
    }
    else
    {
        Err_set(XCL_MEMORY_ERR);
    }
    return cond;
}

XCL_PUBLIC(bool)
Cond_delete(CCond* cond)
{
    Pool_dealloc(0, cond, sizeof(CCond));
    return true;
}

XCL_PUBLIC(bool)
Cond_wait(CMutex* mutex, CCond* cond)
{
    if (cond)
    {
        return SleepConditionVariableCS(&((CCond*)cond)->conditionVariable,
                                        (PCRITICAL_SECTION)mutex,
                                        INFINITE);
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
    }
    return false;
}

XCL_PUBLIC(bool)
Cond_waitFor(CMutex* mutex, CCond* cond, int32_t millis)
{
    if (!cond || !mutex)
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    bool success = SleepConditionVariableCS(&((CCond*)cond)->conditionVariable,
                                            (PCRITICAL_SECTION)mutex,
                                            millis);
    if (!success)
    {
        Err_set(GetLastError());
    }
    return success;
}

XCL_PUBLIC(bool)
Cond_signal(CCond* cond)
{
    if (cond)
    {
        WakeConditionVariable(&((CCond*)cond)->conditionVariable);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_PUBLIC(bool)
Cond_signalAll(CCond* cond)
{
    if (cond)
    {
        WakeAllConditionVariable(&((CCond*)cond)->conditionVariable);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}
