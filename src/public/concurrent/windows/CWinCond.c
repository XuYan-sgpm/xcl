//
// Created by xuyan on 2022/7/5.
//

#include "xcl/concurrent/CCond.h"
#include "xcl/lang/XclErr.h"
#include <WinBase.h>
#include <Windows.h>
#include <synchapi.h>

typedef struct {
    CONDITION_VARIABLE conditionVariable;
} CWinCond;

XCL_PUBLIC(void*)
Cond_new()
{
    CWinCond* cond = (CWinCond*)malloc(sizeof(CWinCond));
    if (cond) { InitializeConditionVariable(&cond->conditionVariable); }
    else { setErr(XCL_MEMORY_ERR); }
    return cond;
}

XCL_PUBLIC(bool)
Cond_delete(void* cond) { return true; }

XCL_PUBLIC(bool)
Cond_wait(void* mutex, void* cond)
{
    if (cond)
    {
        return SleepConditionVariableCS(&((CWinCond*)cond)->conditionVariable,
                                        mutex, INFINITE);
    }
    else { setErr(XCL_INVALID_PARAM); }
    return false;
}

XCL_PUBLIC(bool)
Cond_waitFor(void* mutex, void* cond, int32_t millis)
{
    if (!cond || !mutex)
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
    bool success = SleepConditionVariableCS(
        &((CWinCond*)cond)->conditionVariable, mutex, millis);
    if (!success) { setErr(GetLastError()); }
    return success;
}

XCL_PUBLIC(bool)
Cond_signal(void* cond)
{
    if (cond)
    {
        WakeConditionVariable(&((CWinCond*)cond)->conditionVariable);
        return true;
    }
    else
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
}

XCL_PUBLIC(bool)
Cond_signalAll(void* cond)
{
    if (cond)
    {
        WakeAllConditionVariable(&((CWinCond*)cond)->conditionVariable);
        return true;
    }
    else
    {
        setErr(XCL_INVALID_PARAM);
        return false;
    }
}
