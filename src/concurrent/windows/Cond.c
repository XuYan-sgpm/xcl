//
// Created by xuyan on 2022/7/5.
//

#include <Windows.h>
#include "concurrent/cond.h"
#include "lang/xcl_err.h"
#include "pool/pool.h"

struct Cond {
    CONDITION_VARIABLE conditionVariable;
};

XCL_EXPORT Cond* XCL_API
Cond_new()
{
    Cond* cond = (Cond*)Pool_alloc(Pool_def(), sizeof(Cond));
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

XCL_EXPORT bool XCL_API
Cond_delete(Cond* cond)
{
    Pool_dealloc(Pool_def(), cond, sizeof(Cond));
    return true;
}

XCL_EXPORT bool XCL_API
Cond_wait(Mutex* mutex, Cond* cond)
{
    if (cond)
    {
        return SleepConditionVariableCS(&((Cond*)cond)->conditionVariable,
                                        (PCRITICAL_SECTION)mutex,
                                        INFINITE);
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
    }
    return false;
}

XCL_EXPORT bool XCL_API
Cond_waitFor(Mutex* mutex, Cond* cond, int32_t millis)
{
    if (!cond || !mutex)
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
    bool success = SleepConditionVariableCS(&((Cond*)cond)->conditionVariable,
                                            (PCRITICAL_SECTION)mutex,
                                            millis);
    if (!success)
    {
        Err_set(GetLastError());
    }
    return success;
}

XCL_EXPORT bool XCL_API
Cond_signal(Cond* cond)
{
    if (cond)
    {
        WakeConditionVariable(&((Cond*)cond)->conditionVariable);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_EXPORT bool XCL_API
Cond_signalAll(Cond* cond)
{
    if (cond)
    {
        WakeAllConditionVariable(&((Cond*)cond)->conditionVariable);
        return true;
    }
    else
    {
        Err_set(XCL_INVALID_PARAM);
        return false;
    }
}
