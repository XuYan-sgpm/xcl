//
// Created by xuyan on 2022/7/21.
//

#include "xcl/lang/CInterThreadApi.h"
#include "xcl/lang/XclErr.h"
#include "xcl/lang/CLocalStorageReg.h"
#include <windows.h>
#include <process.h>
#include <assert.h>

bool __Win32_wait(HANDLE handle, DWORD timeout);

static unsigned __stdcall __Win32_threadRoutine(void* args)
{
    __Thread_run(args);
    return 0;
}

bool __Thread_createHandle(void* args, CThread* thread)
{
    uintptr_t h = _beginthreadex(NULL,
                                 0,
                                 __Win32_threadRoutine,
                                 args,
                                 CREATE_SUSPENDED,
                                 NULL);
    thread->handle = h;
    if (h)
    {
        assert(ResumeThread((HANDLE)h) != -1);
    }
    return h;
}

bool __Thread_wait(uintptr_t handle, int32_t timeout)
{
    return __Win32_wait((HANDLE)handle, timeout > 0 ? timeout : INFINITE);
}

bool __Thread_detach(uintptr_t handle)
{
    return __Thread_closeHandle(handle);
}

bool __Thread_alive(uintptr_t handle)
{
    DWORD exit;
    if (!GetExitCodeThread((HANDLE)handle, &exit))
    {
        Err_set(GetLastError());
        return false;
    }
    return exit == STILL_ACTIVE;
}

unsigned long __Thread_currentId()
{
    return GetCurrentThreadId();
}

uintptr_t __Thread_currentHandle()
{
    return (uintptr_t)GetCurrentThread();
}

bool __Thread_closeHandle(uintptr_t handle)
{
    bool ret = CloseHandle((HANDLE)handle);
    if (!ret)
    {
        Err_set(GetLastError());
    }
    return ret;
}

#ifdef STATIC

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
            __deregisterLocalStorage(Thread_currentHandle());
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
