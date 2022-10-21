//
// Created by xuyan on 2022/7/21.
//

#include "lang/inter_thread_api.h"
#include "lang/xcl_err.h"
#include <windows.h>
#include <process.h>

bool
__Win32_wait(HANDLE handle, DWORD timeout);

static unsigned WINAPI
__Win32_threadRoutine(void* args)
{
    __Thread_run(args);
    return 0;
}

uintptr_t
__Thread_createHandle(void* args)
{
    uintptr_t h = _beginthreadex(NULL, 0, __Win32_threadRoutine, args, 0, NULL);
    return h;
}

bool
__Thread_joinFor(uintptr_t handle, int32_t timeout)
{
    return __Win32_wait((HANDLE)handle, timeout >= 0 ? timeout : INFINITE)
           && CloseHandle((HANDLE)handle);
}

bool
__Thread_detach(uintptr_t handle)
{
    bool ret = CloseHandle((HANDLE)handle);
    if (ret)
    {
        Err_set(GetLastError());
    }
    return ret;
}

bool
__Thread_alive(uintptr_t handle)
{
    DWORD exit;
    if (!GetExitCodeThread((HANDLE)handle, &exit))
    {
        Err_set(GetLastError());
        return false;
    }
    return exit == STILL_ACTIVE;
}

unsigned long
__Thread_currentId()
{
    return GetCurrentThreadId();
}

uintptr_t
__Thread_currentHandle()
{
    return (uintptr_t)GetCurrentThread();
}

void
__Thread_yield()
{
    Sleep(0);
}

#ifdef STATIC

#ifdef _MSC_VER
static __declspec(thread) LocalStorage* __Win32_Thread_local_storage = NULL;
#elif GNUC || CLANG
static __thread LocalStorage* __Win32_Thread_local_storage = NULL;
#endif

LocalStorage*
__Thread_getLocalStorage()
{
    return __Win32_Thread_local_storage;
}

bool
__Thread_setLocalStorage(LocalStorage* local_storage)
{
    __Win32_Thread_local_storage = local_storage;
    return true;
}

#elif defined(DYNAMIC)

#include <concurrent/GlobalLock.h>

static DWORD __Win32_Thread_storage_key = TLS_OUT_OF_INDEXES;

void
__allocTls()
{
    DWORD idx = TlsAlloc();
    if (idx != TLS_OUT_OF_INDEXES)
    {
        __Win32_Thread_storage_key = idx;
    }
    else
    {
        Err_set(GetLastError());
    }
}

static void
__Thread_ensureTlsKey()
{
    // static volatile bool initTlsDone = false;
    // if (!initTlsDone)
    // {
    //     __acquireGlobalLock();
    //     if (!initTlsDone)
    //     {
    //         __allocTls();
    //         initTlsDone = true;
    //     }
    //     __releaseGlobalLock();
    // }
}

LocalStorage*
__Thread_getLocalStorage()
{
    __Thread_ensureTlsKey();
    return (LocalStorage*)TlsGetValue(__Win32_Thread_storage_key);
}

bool
__Thread_setLocalStorage(LocalStorage* local_storage)
{
    __Thread_ensureTlsKey();
    bool success = TlsSetValue(__Win32_Thread_storage_key, local_storage);
    if (!success)
    {
        Err_set(GetLastError());
    }
    return success;
}

#endif
