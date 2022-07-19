//
// Created by xy on 7/10/22.
//

#include "xcl/lang/CInterThreadApi.h"
#include "xcl/lang/system.h"
#include "xcl/util/CBlocker.h"
#include "xcl/lang/CLocalStorageReg.h"
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef struct {
    CThread base;
    CBlocker* blocker;
} CUnixThread;

static inline CBlocker* __UnixThread_blocker(CThread* thread)
{
    return ((CUnixThread*)thread)->blocker;
}

CThread* __Thread_alloc()
{
    return Pool_alloc(NULL, sizeof(CUnixThread));
}

void __Thread_dealloc(CThread* thread)
{
    Pool_dealloc(NULL, thread, sizeof(CUnixThread));
}

void __Thread_beforeCreate(CThread* thread)
{
    ((CUnixThread*)thread)->blocker = Blocker_new2(__Thread_mutex(thread));
}

void __Thread_afterCreate(CThread* thread)
{}

void __Thread_wait(CThread* thread)
{
    int ret = pthread_join(__Thread_handle(thread), NULL);
    if (ret)
        errno = ret;
}

bool __Thread_waitTimeout(CThread* thread, int32_t timeout)
{
    int64_t nanoTimeout = timeout * 1000000L;
    int64_t totalWait = 0;
    ThreadHandle handle = __Thread_handle(thread);
    int64_t st = nanos();
    while (totalWait < nanoTimeout)
    {
        int ret = pthread_kill(handle, 0);
        if (ret == ESRCH)
        {
            return true;
        }
#if _POSIX_C_SOURCE >= 199309L
        struct timespec ts = {0, nanoTimeout - totalWait};
        nanosleep(&ts, NULL);
#else
        usleep((nanoTimeout - totalWait) / 1000);
#endif
        totalWait = nanos() - st;
    }
    return pthread_kill(handle, 0) == ESRCH;
}

bool __Thread_create(bool suspend, __ThreadRunProc run, void* usr,
                     ThreadHandle* handle)
{
    pthread_t h;
    int ret = pthread_create(&h, NULL, run, usr);
    if (ret == 0)
    {
        *handle = h;
        return true;
    }
    errno = ret;
    return false;
}

void __Thread_resume(CThread* thread)
{
    __Thread_setState(thread, ALIVE);
    Blocker_cancel(__UnixThread_blocker(thread));
}

void __Thread_onStart(CThread* thread)
{
    Blocker_wait(__UnixThread_blocker(thread));
}

void __Thread_onFinish(CThread* thread, __ThreadRunReturnType retVal)
{}

unsigned __Thread_currentId()
{
    return syscall(__NR_gettid);
}

ThreadHandle __Thread_currentHandle()
{
    return pthread_self();
}

void __Thread_finalize(CThread* thread)
{
    Blocker_delete(__UnixThread_blocker(thread));
}

void __Thread_detach(CThread* thread)
{
    int ret = pthread_detach(__Thread_handle(thread));
    if (ret)
        errno = ret;
}

bool __Thread_isAlive(ThreadHandle handle)
{
    return pthread_kill(handle, 0) != ESRCH;
}

void __LocalId_initQueue();

#if STATIC

#include <stddef.h>
#include <unistd.h>
#include <syscall.h>

static __thread CLocalStorage* __Unix_Thread_localStorage = NULL;

CLocalStorage* __Thread_getLocalStorage()
{
    return __Unix_Thread_localStorage;
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Unix_Thread_localStorage = localStorage;
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

#elif DYNAMIC

#include "xcl/lang/XclErr.h"
#include <assert.h>

pthread_key_t __Unix_storageKey = 0;

static void __releaseLocalStorage(void* args)
{
    CLocalStorage* localStorage = args;
    if (localStorage)
    {
        LocalStorage_delete(localStorage);
    }
}

CLocalStorage* __Thread_getLocalStorage()
{
    return pthread_getspecific(__Unix_storageKey);
}

bool __Thread_setLocalStorage(CLocalStorage* localStorage)
{
    int ret = pthread_setspecific(__Unix_storageKey, localStorage);
    bool success = ret == 0;
    if (!localStorage)
    {
        if (success)
            __deregisterLocalStorage(__Thread_currentHandle());
    }
    else
    {
        if (!success)
        {
            setErr(ret);
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
#ifdef DYNAMIC
    int ret = pthread_key_create(&__Unix_storageKey, __releaseLocalStorage);
    if (ret)
    {
        setErr(ret);
    }
    assert(ret == 0);
#endif
}
