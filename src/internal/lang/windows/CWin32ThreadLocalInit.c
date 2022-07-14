//
// Created by xuyan on 2022/7/14.
//

#include <xcl/lang/XclDef.h>
#include <stdbool.h>
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CThreadLocal.h"

void
__LocalId_initQueue();

void
__Local_implInitialize();

#ifdef STATIC

static __declspec(thread) CLocalStorage* __Win32_Thread_localStorage = NULL;

CLocalStorage*
__Thread_getLocalStorage()
{
    return __Win32_Thread_localStorage;
}
bool
__Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Win32_Thread_localStorage = localStorage;
    return true;
}
void
__Local_implInitialize()
{
    __LocalId_initQueue();
}

#elif DYNAMIC

    #include <windows.h>
    #include <processthreadsapi.h>
    #include "xcl/lang/XclErr.h"

static DWORD __Win32_storageKey = TLS_OUT_OF_INDEXES;

void
__allocTls()
{
    DWORD idx = TlsAlloc();
    if (idx != TLS_OUT_OF_INDEXES)
    {
        __Win32_storageKey = idx;
    }
    else
    {
        setErr(GetLastError());
    }
}

void
__Local_implInitialize()
{
    __LocalId_initQueue();
    __allocTls();
}

CLocalStorage*
__Thread_getLocalStorage()
{
    return (CLocalStorage*)TlsGetValue(__Win32_storageKey);
}

bool
__Thread_setLocalStorage(CLocalStorage*localStorage)
{
    bool success = TlsSetValue(__Win32_storageKey, localStorage);
    if (!success)
    {
        setErr(GetLastError());
    }
    return success;
}

#endif

#if GNUC || CLANG

static __attribute__((constructor)) void
__Win32_initLocalEnv()
{
    Local_initEnv();
}

#endif
