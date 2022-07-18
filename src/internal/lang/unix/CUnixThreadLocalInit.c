//
// Created by xuyan on 2022/7/14.
//

#include "xcl/lang/CThreadLocal.h"
#include "xcl/lang/CLocalStorageReg.h"
#include <stdbool.h>
#include <xcl/lang/CLocalStorage.h>

void __LocalId_initQueue();

void __Local_implInitialize();

#if STATIC

#    include <stddef.h>
#    include <unistd.h>
#    include <syscall.h>

static __thread CLocalStorage* __Unix_Thread_localStorage = NULL;

CLocalStorage* __Thread_getLocalStorage() { return __Unix_Thread_localStorage; }

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Unix_Thread_localStorage = localStorage;
    if (!localStorage) { __deregisterLocalStorage(syscall(__NR_gettid)); }
    else { __regLocalStorage(localStorage); }
    return true;
}

void __Local_implInitialize()
{
    __LocalId_initQueue();
    __initializeRegQueue();
}

#elif DYNAMIC

#    include "xcl/lang/XclErr.h"
#    include <assert.h>
#    include <pthread.h>

pthread_key_t __Unix_storageKey = 0;

static void __releaseLocalStorage(void* args)
{
    CLocalStorage* localStorage = args;
    if (localStorage) { LocalStorage_free(localStorage); }
}

void __Local_implInitialize()
{
    __LocalId_initQueue();
    int ret = pthread_key_create(&__Unix_storageKey, __releaseLocalStorage);
    if (ret) { setErr(ret); }
    assert(ret == 0);
    __initializeRegQueue();
}

CLocalStorage* __Thread_getLocalStorage()
{
    return pthread_getspecific(__Unix_storageKey);
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    int ret = pthread_setspecific(__Unix_storageKey, localStorage);
    if (ret) { setErr(ret); }
    return !ret;
}

#endif

static __attribute__((constructor)) void __Unix_initLocalEnv()
{
    Local_initEnv();
}
