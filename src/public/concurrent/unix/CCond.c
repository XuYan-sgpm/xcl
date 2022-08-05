//
// Created by xy on 7/10/22.
//

#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <xcl/concurrent/CCond.h>
#include "xcl/pool/CPool.h"

struct _CCond_st {
    pthread_cond_t handle;
};

XCL_EXPORT CCond* XCL_API
Cond_new()
{
    CCond* cond = Pool_alloc(Pool_def(), sizeof(CCond));
    if (cond)
    {
        memset(cond, 0, sizeof(CCond));
        int ret = pthread_cond_init(&cond->handle, NULL);
        if (ret == 0)
        {
            return cond;
        }
        errno = ret;
        Pool_dealloc(Pool_def(), cond, sizeof(CCond));
    }
    return NULL;
}

XCL_EXPORT bool XCL_API
Cond_delete(CCond* cond)
{
    if (cond)
    {
        int ret = pthread_cond_destroy(&((CCond*)cond)->handle);
        Pool_dealloc(Pool_def(), cond, sizeof(CCond));
        if (ret)
            errno = ret;
        return !ret;
    }
    return false;
}

XCL_EXPORT bool XCL_API
Cond_wait(CMutex* mutex, CCond* cond)
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
Cond_waitFor(CMutex* mutex, CCond* cond, int32_t millis)
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
Cond_signal(CCond* cond)
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
Cond_signalAll(CCond* cond)
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
