//
// Created by xuyan on 2022/7/6.
//

#include <xcl/lang/CBaseThreadImpl.h>
#include <windows.h>
#include <synchapi.h>
#include <process.h>
#include <xcl/lang/XclErr.h>

bool
__Win32_wait(HANDLE handle, DWORD timeout);

void
__Thread_beforeCreate(CThread*thread)
{
}

void
__Thread_afterCreate(CThread*thread)
{
}

void
__Thread_wait(CThread*thread)
{
    __Win32_wait(__Thread_handle(thread), INFINITE);
}

bool
__Thread_waitTimeout(CThread*thread, int32_t timeout)
{
    return __Win32_wait(__Thread_handle(thread), timeout);
}

bool
__Thread_create(bool suspend,
                __ThreadRunProc run,
                void*usr,
                ThreadHandle*handle)
{
    ThreadHandle h = (ThreadHandle)_beginthreadex(
        NULL, 0, run, usr, suspend ? CREATE_SUSPENDED : 0, NULL);
    *handle = h;
    return h != NULL;
}

void
__Thread_resume(CThread*thread)
{
    __Thread_setState(thread, ALIVE);
    DWORD ret = ResumeThread(__Thread_handle(thread));
    if (ret == -1)
    {
        setErr(GetLastError());
    }
}

void
__Thread_onStart(CThread*thread)
{
}

void
__Thread_onFinish(CThread*thread, __ThreadRunReturnType retVal)
{
    _endthreadex(retVal);
}

unsigned
__Thread_currentId()
{
    return GetCurrentThreadId();
}

ThreadHandle
__Thread_currentHandle()
{
    return GetCurrentThread();
}

void
__Thread_finalize(CThread*thread)
{
    CloseHandle(__Thread_handle(thread));
}

void
__Thread_detach(CThread*thread)
{
    CloseHandle(__Thread_handle(thread));
    __Thread_setState(thread, DETACHED);
}
