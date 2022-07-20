//
// Created by xuyan on 2022/7/6.
//

#include <Windows.h>
#include <process.h>
#include "xcl/lang/CInterThreadApi.h"
#include "xcl/lang/XclErr.h"
#include <xcl/lang/CLocalStorageReg.h>

bool __Win32_wait(HANDLE handle, DWORD timeout);

CThread* __Thread_alloc()
{
    return Pool_alloc(NULL, sizeof(CThread));
}

void __Thread_dealloc(CThread* thread)
{
    Pool_dealloc(NULL, thread, sizeof(CThread));
}

void __Thread_beforeCreate(CThread* thread)
{}

void __Thread_afterCreate(CThread* thread)
{}

bool __Thread_wait(CThread* thread)
{
    return __Win32_wait((HANDLE)__Thread_handle(thread), INFINITE);
}

bool __Thread_waitTimeout(CThread* thread, int32_t timeout)
{
    return __Win32_wait((HANDLE)__Thread_handle(thread), timeout);
}

static unsigned __Win32_threadRoutine(void* usr)
{
    __Thread_run(usr);
    return 0;
}

bool __Thread_create(bool suspend, void* usr, ThreadHandle* handle)
{
    ThreadHandle h = _beginthreadex(NULL,
                                    0,
                                    __Win32_threadRoutine,
                                    usr,
                                    suspend ? CREATE_SUSPENDED : 0,
                                    NULL);
    *handle = h;
    return h != 0;
}

void __Thread_resume(CThread* thread)
{
    __Thread_setState(thread, ALIVE);
    DWORD ret = ResumeThread((HANDLE)__Thread_handle(thread));
    if (ret == -1)
    {
        Err_set(GetLastError());
    }
}

void __Thread_onStart(CThread* thread)
{}

void __Thread_onFinish(CThread* thread)
{}

unsigned long __Thread_currentId()
{
    return GetCurrentThreadId();
}

ThreadHandle __Thread_currentHandle()
{
    return (ThreadHandle)GetCurrentThread();
}

void __Thread_closeHandle(CThread* thread)
{
    CloseHandle((HANDLE)__Thread_handle(thread));
}

void __Thread_detach(CThread* thread)
{
    CloseHandle((HANDLE)__Thread_handle(thread));
    __Thread_setState(thread, DETACHED);
}

bool __Thread_isAlive(ThreadHandle handle)
{
    DWORD exit;
    if (!GetExitCodeThread((HANDLE)handle, &exit))
    {
        Err_set(GetLastError());
        return false;
    }
    return exit == STILL_ACTIVE;
}

#ifdef STATIC

#include <processthreadsapi.h>
#include <assert.h>

#ifdef _MSC_VER
static __declspec(thread) CLocalStorage* __Win32_Thread_localStorage = NULL;
#elif GNUC || CLANG
static __thread CLocalStorage* __Win32_Thread_localStorage = NULL;
#endif

CLocalStorage* __Thread_getLocalStorage()
{
    return __Win32_Thread_localStorage;
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Win32_Thread_localStorage = localStorage;
    if (!localStorage)
    {
        __deregisterLocalStorage(__Thread_currentHandle());
    }
    else
    {
        __regLocalStorage(localStorage);
    }
    return true;
}

#elif defined(DYNAMIC)

#include <processthreadsapi.h>

static DWORD __Win32_storageKey = TLS_OUT_OF_INDEXES;

void __allocTls()
{
    DWORD idx = TlsAlloc();
    if (idx != TLS_OUT_OF_INDEXES)
    {
        __Win32_storageKey = idx;
    }
    else
    {
        Err_set(GetLastError());
    }
}

CLocalStorage* __Thread_getLocalStorage()
{
    return (CLocalStorage*)TlsGetValue(__Win32_storageKey);
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    bool success = TlsSetValue(__Win32_storageKey, localStorage);
    if (!localStorage)
    {
        if (success)
            __deregisterLocalStorage(__Thread_currentHandle());
    }
    else
    {
        if (!success)
        {
            Err_set(GetLastError());
        }
        else
        {
            __regLocalStorage(localStorage);
        }
    }
    return success;
}

#endif

void __initializeLocalStorageAccess()
{
    __initializeRegQueue();
#if DYNAMIC
    __allocTls();
#endif
}
