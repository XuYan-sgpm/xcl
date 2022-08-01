#include <xcl/lang/CInterThreadApi.h>
#include <pthread.h>
#include <sys/errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "xcl/lang/XclErr.h"
#include "xcl/lang/CSys.h"

static void*
__Unix_threadRoutine(void* args)
{
    __Thread_run(args);
    return NULL;
}

uintptr_t
__Thread_createHandle(void* args)
{
    pthread_t handle;
    int ret = pthread_create(&handle, NULL, __Unix_threadRoutine, args);
    if (ret)
    {
        Err_set(ret);
        return 0;
    }
    else
    {
        return (uintptr_t)handle;
    }
}

bool
__Thread_joinFor(uintptr_t handle, int32_t timeout)
{
    if (timeout < 0)
    {
        int ret = pthread_join((pthread_t)handle, 0);
        if (ret)
        {
            Err_set(ret);
        }
        return !ret;
    }
    else
    {
        bool alive = __Thread_alive(handle);
        if (!alive)
        {
            return true;
        }
        int64_t require = (int64_t)timeout * 1000000;
        int64_t totalWait = 0;
        int64_t st = nanos();
        do
        {
            mSleep(1);
            alive = __Thread_alive(handle);
            totalWait = nanos() - st;
        } while (alive && totalWait < require);
        if (alive)
        {
            alive = __Thread_alive(handle);
        }
        return !alive;
    }
}

bool
__Thread_detach(uintptr_t handle)
{
    int ret = pthread_detach((pthread_t)handle);
    if (ret)
        Err_set(ret);
    return !ret;
}

bool
__Thread_alive(uintptr_t handle)
{
    return pthread_kill((pthread_t)handle, 0) != ESRCH;
}

unsigned long
__Thread_currentId()
{
#if LINUX
    return syscall(__NR_gettid);
#elif MACOSX
    return syscall(SYS_gettid);
#endif
}

uintptr_t
__Thread_currentHandle()
{
    return (uintptr_t)pthread_self();
}

#ifdef STATIC
static __thread CLocalStorage* __Unix_Thread_localStorage = NULL;

CLocalStorage*
__Thread_getLocalStorage()
{
    return __Unix_Thread_localStorage;
}

bool
__Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Unix_Thread_localStorage = localStorage;
    return true;
}
#elif defined(DYNAMIC)
#include <xcl/concurrent/GlobalLock.h>
#include <assert.h>

static pthread_key_t __Unix_Thread_localStorageKey;

static void
__destroyLocalStorage(void* args)
{
    LocalStorage_delete(args);
}

static void
__Thread_ensureLocalStorageKey()
{
    static bool initStorageKey = false;
    if (!initStorageKey)
    {
        __acquireGlobalLock();
        if (!initStorageKey)
        {
            int ret = pthread_key_create(&__Unix_Thread_localStorageKey,
                                         __destroyLocalStorage);
            assert(ret == 0);
            initStorageKey = true;
        }
        __releaseGlobalLock();
    }
}

CLocalStorage*
__Thread_getLocalStorage()
{
    __Thread_ensureLocalStorageKey();
    return pthread_getspecific(__Unix_Thread_localStorageKey);
}

bool
__Thread_setLocalStorage(CLocalStorage* localStorage)
{
    __Thread_ensureLocalStorageKey();
    int ret = pthread_setspecific(__Unix_Thread_localStorageKey, localStorage);
    if (ret)
    {
        Err_set(ret);
    }
    return !ret;
}
#endif
