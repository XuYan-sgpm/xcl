#include <xcl/lang/inter_thread_api.h>
#include <pthread.h>
#include <sys/errno.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "lang/xcl_err.h"
#include "lang/system.h"

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
        int64_t total_wait = 0;
        int64_t st = nanos();
        do
        {
            mSleep(1);
            alive = __Thread_alive(handle);
            total_wait = nanos() - st;
        } while (alive && total_wait < require);
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

void
__Thread_yield()
{
    sleep(0);
}

#ifdef STATIC
static __thread LocalStorage* __Unix_Thread_local_storage = NULL;

LocalStorage*
__Thread_getLocalStorage()
{
    return __Unix_Thread_local_storage;
}

bool
__Thread_setLocalStorage(LocalStorage* local_storage)
{
    __Unix_Thread_local_storage = local_storage;
    return true;
}
#elif defined(DYNAMIC)
#include <concurrent/GlobalLock.h>
#include <assert.h>

static pthread_key_t __Unix_Thread_local_storage_key;

static void
__destroyLocalStorage(void* args)
{
    LocalStorage_delete(args);
}

static void
__Thread_ensureLocalStorageKey()
{
    // static volatile bool initStorageKey = false;
    // if (!initStorageKey)
    // {
    //     __acquireGlobalLock();
    //     if (!initStorageKey)
    //     {
    //         int ret = pthread_key_create(&__Unix_Thread_local_storage_key,
    //                                      __destroyLocalStorage);
    //         assert(ret == 0);
    //         initStorageKey = true;
    //     }
    //     __releaseGlobalLock();
    // }
}

LocalStorage*
__Thread_getLocalStorage()
{
    __Thread_ensureLocalStorageKey();
    return pthread_getspecific(__Unix_Thread_local_storage_key);
}

bool
__Thread_setLocalStorage(LocalStorage* local_storage)
{
    __Thread_ensureLocalStorageKey();
    int ret
        = pthread_setspecific(__Unix_Thread_local_storage_key, local_storage);
    if (ret)
    {
        Err_set(ret);
    }
    return !ret;
}
#endif
