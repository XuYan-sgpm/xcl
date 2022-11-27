//
// Created by xuyan on 2022/7/21.
//

#include "lang/inter_thread_api.h"
#include "lang/xcl_err.h"
#include <windows.h>
#include <process.h>
#include <stdlib.h>

bool
__waitHandle(HANDLE handle, DWORD timeout);

typedef struct {
    void (*run)(void*);
    void* obj;
} ThreadProcArgs;

static unsigned __stdcall __Thread_proc(void* args)
{
    ThreadProcArgs* thread_proc_args = (ThreadProcArgs*)args;
    thread_proc_args->run(thread_proc_args->obj);
    free(args);
    return 0;
}

Thread XCL_API
Thread_create(void (*run)(void*), void* args)
{
    ThreadProcArgs* thread_proc_args =
        (ThreadProcArgs*)malloc(sizeof(ThreadProcArgs));
    if (!thread_proc_args)
        return 0;
    thread_proc_args->obj = args;
    thread_proc_args->run = run;
    Thread h =
        _beginthreadex(NULL, 0, __Thread_proc, thread_proc_args, 0, NULL);
    return h;
}

bool
__Thread_joinFor(Thread handle, int32_t timeout)
{
    return __waitHandle((HANDLE)handle, timeout >= 0 ? timeout : INFINITE) &&
           CloseHandle((HANDLE)handle);
}

bool XCL_API
Thread_join(Thread thread)
{
    return __Thread_joinFor(thread, INFINITE);
}

bool XCL_API
Thread_join2(Thread thread, int32_t timeout)
{
    return __Thread_joinFor(thread, timeout);
}

bool XCL_API
Thread_detach(Thread handle)
{
    bool ret = CloseHandle((HANDLE)handle);
    if (ret)
    {
        Err_set(GetLastError());
    }
    return ret;
}

bool XCL_API
Thread_alive(Thread handle)
{
    DWORD exit;
    if (!GetExitCodeThread((HANDLE)handle, &exit))
    {
        Err_set(GetLastError());
        return false;
    }
    return exit == STILL_ACTIVE;
}

unsigned long XCL_API
Thread_currentId()
{
    return GetCurrentThreadId();
}

Thread XCL_API
Thread_current()
{
    return (Thread)GetCurrentThread();
}

void XCL_API
Thread_yield()
{
    Sleep(0);
}

#ifdef STATIC

#  ifdef _MSC_VER
#    define THREAD_LOCAL __declspec(thread)
#  elif GNUC || CLANG
#    define THREAD_LOCAL __thread
#  endif

#endif

#ifdef STATIC

static THREAD_LOCAL LocalStorage* __thread_local_storage = NULL;

LocalStorage*
__Thread_getLocalStorage()
{
    return __thread_local_storage;
}

bool
__Thread_setLocalStorage(LocalStorage* local_storage)
{
    __thread_local_storage = local_storage;
    return true;
}

#elif defined(SHARED)

static DWORD __thread_local_storage_key = TLS_OUT_OF_INDEXES;

void
__Thread_initLocalStorage()
{
    if (__thread_local_storage_key != TLS_OUT_OF_INDEXES)
        return;
    DWORD idx = TlsAlloc();
    if (idx != TLS_OUT_OF_INDEXES)
    {
        __thread_local_storage_key = idx;
    }
    else
    {
        Err_set(GetLastError());
    }
}

LocalStorage*
__Thread_getLocalStorage()
{
    if (__thread_local_storage_key == TLS_OUT_OF_INDEXES)
        return NULL;
    return (LocalStorage*)TlsGetValue(__thread_local_storage_key);
}

bool
__Thread_setLocalStorage(LocalStorage* local_storage)
{
    if (__thread_local_storage_key == TLS_OUT_OF_INDEXES)
        return false;
    bool success = TlsSetValue(__thread_local_storage_key, local_storage);
    if (!success)
    {
        Err_set(GetLastError());
    }
    return success;
}

#  ifndef _MSC_VER
static __attribute__((constructor)) void
__initLocalCtx()
{
    __Thread_initLocalStorage();
}

static __attribute__((destructor)) void
__releaseLocalCtx()
{
    if (__thread_local_storage_key == TLS_OUT_OF_INDEXES)
        return;
    TlsFree(__thread_local_storage_key);
}
#  else
BOOL APIENTRY
DllMain(HANDLE hModule, DWORD reason, LPVOID lpReserved)
{
    BOOL ret = TRUE;
    switch (reason)
    {
        case DLL_PROCESS_ATTACH: {
            __Thread_initLocalStorage();
            ret = __thread_local_storage_key != TLS_OUT_OF_INDEXES;
            break;
        }

        case DLL_PROCESS_DETACH: {
            if (__thread_local_storage_key != TLS_OUT_OF_INDEXES)
                TlsFree(__thread_local_storage_key);
            break;
        }

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH: {
            LocalStorage* local_storage = __Thread_getLocalStorage();
            if (local_storage)
                LocalStorage_delete(local_storage);
            break;
        }
    }
    return ret;
}
#  endif

#endif
