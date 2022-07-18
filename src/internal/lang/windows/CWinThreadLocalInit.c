//
// Created by xuyan on 2022/7/14.
//

#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CThreadLocal.h"
#include "xcl/lang/CLocalStorageReg.h"
#include <stdbool.h>
#include <xcl/lang/XclDef.h>
#include <stdio.h>

void __LocalId_initQueue();

void __Local_implInitialize();

#ifdef STATIC

#    include <processthreadsapi.h>
#    include <assert.h>

#    ifdef _MSC_VER
static __declspec(thread) CLocalStorage* __Win32_Thread_localStorage = NULL;
#    elif GNUC || CLANG
static __thread CLocalStorage* __Win32_Thread_localStorage = NULL;
#    endif

CLocalStorage* __Thread_getLocalStorage()
{
    return __Win32_Thread_localStorage;
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Win32_Thread_localStorage = localStorage;
    if (!localStorage) { __deregisterLocalStorage(GetCurrentThreadId()); }
    else { __regLocalStorage(localStorage); }
    return true;
}

void __Local_implInitialize()
{
    __LocalId_initQueue();
    __initializeRegQueue();
}

#elif defined(DYNAMIC)

#    include <windows.h>
#    include "xcl/lang/XclErr.h"
#    include <processthreadsapi.h>
#    include <assert.h>

static DWORD __Win32_storageKey = TLS_OUT_OF_INDEXES;

void __allocTls()
{
    DWORD idx = TlsAlloc();
    if (idx != TLS_OUT_OF_INDEXES) { __Win32_storageKey = idx; }
    else { setErr(GetLastError()); }
}

void __Local_implInitialize()
{
    __LocalId_initQueue();
    __allocTls();
    __initializeRegQueue();
}

CLocalStorage* __Thread_getLocalStorage()
{
    return (CLocalStorage*)TlsGetValue(__Win32_storageKey);
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    bool success = TlsSetValue(__Win32_storageKey, localStorage);
    if (!localStorage) { __deregisterLocalStorage(GetCurrentThreadId()); }
    else
    {
        if (!success) { setErr(GetLastError()); }
        else { __regLocalStorage(localStorage); }
    }
    return success;
}

#endif

#if GNUC || CLANG
static __attribute__((constructor)) void __Win32_initLocalEnv()
{
    Local_initEnv();
}

static __attribute__((destructor)) void __Win32_checkLocalStorageReg()
{
    assert(__hasRegLocalStorage() == false);
    printf("no unrecycled thread local storage\n");
}
#endif
