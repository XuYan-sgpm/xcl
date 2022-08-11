//
// Created by 徐琰 on 2022/7/24.
//

#include <assert.h>
#include "xcl/concurrent/GlobalLock.h"
#include "xcl/lang/XclDef.h"
#include "xcl/concurrent/CMutex.h"

static CMutex* __XCL_globalMutex = NULL;

/*
 * static storage for CMutex initialization
 * in msvc tls callback, malloc has some problems,
 * so we use static memory to prevent this case
 */
static char __XCL_globalMutexCtx[1024];

bool
__Mutex_init(CMutex* mutex);

bool
__GlobalLock_acquire()
{
    return Mutex_lock(__XCL_globalMutex);
}

void
__GlobalLock_release()
{
    Mutex_unlock(__XCL_globalMutex);
}

#if CLANG || GNUC
static __attribute__((constructor)) void
__initXclGlobalMutex()
{
    __XCL_globalMutex = (CMutex*)__XCL_globalMutexCtx;
    assert(__Mutex_init(__XCL_globalMutex));
}
#elif defined(_MSC_VER)
#  include <windows.h>

void NTAPI
__TlsCb(PVOID DllHandle, DWORD dwReason, PVOID _)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        __XCL_globalMutex = (CMutex*)__XCL_globalMutexCtx;
        assert(__Mutex_init(__XCL_globalMutex));
    }
}

#  ifdef _WIN64
#    pragma comment(linker, "/INCLUDE:_tls_used")
#    pragma comment(linker, "/INCLUDE:tls_callback_func")
#  else
#    pragma comment(linker, "/INCLUDE:__tls_used")
#    pragma comment(linker, "/INCLUDE:_tls_callback_func")
#  endif

#  ifdef _WIN64
#    pragma const_seg(".CRT$XLF")
EXTERN_C const
#  else
#    pragma data_seg(".CRT$XLF")
EXTERN_C
#  endif
    PIMAGE_TLS_CALLBACK tls_callback_func
    = __TlsCb;
#  ifdef _WIN64
#    pragma const_seg()
#  else
#    pragma data_seg()
#  endif //_WIN64
#else
#  error "global lock can not be initialized"
#endif
