//
// Created by xy on 7/10/22.
//

#include "lang/system.h"
#include "pool/pool.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <concurrent/mutex.h>

struct _CMutex_st {
    pthread_mutex_t handle;
};

bool
__Mutex_init(CMutex* mutex)
{
    memset(mutex, 0, sizeof(CMutex));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int err;
    if ((err = pthread_mutex_init(&mutex->handle, &attr) == 0))
    {
        return true;
    }
    errno = err;
    return false;
}

CMutex*
__Mutex_newByPool(CPool* pool)
{
    CMutex* mutex = Pool_alloc(pool, sizeof(CMutex));
    if (mutex)
    {
        if (!__Mutex_init(mutex))
        {
            Pool_dealloc(pool, mutex, sizeof(CMutex));
        }
        else
        {
            return mutex;
        }
    }
    return NULL;
}

bool
__Mutex_deleteByPool(CMutex* mutex, CPool* pool)
{
    if (mutex)
    {
        int ret = pthread_mutex_destroy(&mutex->handle);
        Pool_dealloc(pool, mutex, sizeof(CMutex));
        if (ret != 0)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT CMutex* XCL_API
Mutex_new()
{
    return __Mutex_newByPool(Pool_def());
}

XCL_EXPORT bool XCL_API
Mutex_delete(CMutex* mutex)
{
    return __Mutex_deleteByPool(mutex, Pool_def());
}

XCL_EXPORT bool XCL_API
Mutex_lock(CMutex* mutex)
{
    if (mutex)
    {
        int ret = pthread_mutex_lock(&mutex->handle);
        if (ret == 0)
        {
            return true;
        }
        errno = ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Mutex_unlock(CMutex* mutex)
{
    if (mutex)
    {
        int ret = pthread_mutex_unlock(&mutex->handle);
        if (ret != 0)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Mutex_tryLock(CMutex* mutex)
{
    if (mutex)
    {
        int ret = pthread_mutex_trylock(&mutex->handle);
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Mutex_tryLock2(CMutex* mutex, int32_t timeout)
{
    if (mutex)
    {
#if LINUX && _POSIX_C_SOURCE >= 199309L
        struct timespec ts = {0, 0};
        clock_gettime(CLOCK_REALTIME, &ts);
        if (timeout > 1000)
        {
            ts.tv_sec += timeout / 1000;
            timeout = timeout % 1000;
        }
        ts.tv_nsec += (int64_t)timeout * 1000000L;
        if (ts.tv_nsec > 1000000000)
        {
            ts.tv_sec += ts.tv_nsec / 1000000000;
            ts.tv_nsec = ts.tv_nsec % 1000000000;
        }
        int ret = pthread_mutex_timedlock(&mutex->handle, &ts);
        if (ret)
            errno = ret;
        return !ret;
#else
        int64_t nanoTimeout = (int64_t)timeout * 1000000L;
        int64_t totalWait = 0;
        int64_t st = nanos();
        while (totalWait < nanoTimeout)
        {
            if ((pthread_mutex_trylock(&mutex->handle)) == 0)
            {
                return true;
            }
#if _POSIX_C_SOURCE >= 199309L
            struct timespec ts = {0, 1000000};
            nanosleep(&ts, NULL);
#else
            usleep(1000);
#endif
            totalWait = nanos() - st;
        }
        int ret = pthread_mutex_trylock(&mutex->handle);
        if (ret)
            errno = ret;
        return !ret;
#endif
    }
    return false;
}
