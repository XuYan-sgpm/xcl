//
// Created by xy on 7/10/22.
//

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <concurrent/cond.h>
#include "pool/pool.h"

struct Cond {
    pthread_cond_t handle;
};

XCL_EXPORT Cond* XCL_API
Cond_new()
{
    Cond* cond = Pool_alloc(Pool_def(), sizeof(Cond));
    if (cond)
    {
        memset(cond, 0, sizeof(Cond));
        int ret = pthread_cond_init(&cond->handle, NULL);
        if (ret == 0)
        {
            return cond;
        }
        errno = ret;
        Pool_dealloc(Pool_def(), cond, sizeof(Cond));
    }
    return NULL;
}

XCL_EXPORT bool XCL_API
Cond_delete(Cond* cond)
{
    if (cond)
    {
        int ret = pthread_cond_destroy(&((Cond*)cond)->handle);
        Pool_dealloc(Pool_def(), cond, sizeof(Cond));
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Cond_wait(Mutex* mutex, Cond* cond)
{
    if (mutex && cond)
    {
        int ret = pthread_cond_wait(&cond->handle, (pthread_mutex_t*)mutex);
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Cond_waitFor(Mutex* mutex, Cond* cond, int32_t millis)
{
    if (mutex && cond)
    {
        struct timespec ts = {0, 0};
        ts.tv_sec = millis / 1000;
        int64_t remaining = millis % 1000;
        ts.tv_nsec = remaining * 1000000;
        int ret = pthread_cond_timedwait(&cond->handle,
                                         (pthread_mutex_t*)mutex,
                                         &ts);
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Cond_signal(Cond* cond)
{
    if (cond)
    {
        int ret = pthread_cond_signal(&cond->handle);
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Cond_signalAll(Cond* cond)
{
    if (cond)
    {
        int ret = pthread_cond_broadcast(&cond->handle);
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}
