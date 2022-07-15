//
// Created by xy on 7/11/22.
//

#include "xcl/lang/XclErr.h"
#include "xcl/util/CBlocker.h"
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <xcl/util/CAbstractBlockerImpl.h>

typedef struct {
    CBlocker base;
    pthread_mutex_t* mutex;
    pthread_cond_t cond;
} CUnixBlocker;

static inline pthread_mutex_t* __UnixBlocker_mutex(CBlocker* blocker)
{
    return ((CUnixBlocker*)blocker)->mutex;
}

static inline pthread_cond_t* __UnixBlocker_cond(CBlocker* blocker)
{
    return &((CUnixBlocker*)blocker)->cond;
}

CBlocker* __Blocker_alloc() { return malloc(sizeof(CUnixBlocker)); }

void __Blocker_dealloc(CBlocker* blocker) { free(blocker); }

int32_t __Blocker_init(CBlocker* blocker, void* mutex)
{
    int ret;
    CUnixBlocker* unixBlocker = (CUnixBlocker*)blocker;
    if (!mutex)
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        unixBlocker->mutex = malloc(sizeof(pthread_mutex_t));
        if (unixBlocker->mutex)
        {
            if ((ret = pthread_mutex_init(unixBlocker->mutex, &attr)) != 0)
            {
                free(unixBlocker->mutex);
                unixBlocker->mutex = 0;
            }
        }
        else { ret = XCL_MEMORY_ERR; }
    }
    else { unixBlocker->mutex = mutex; }
    if (unixBlocker->mutex)
    {
        if ((ret = pthread_cond_init(&unixBlocker->cond, NULL)) == 0)
        {
            return 0;
        }
        pthread_mutex_destroy(unixBlocker->mutex);
        free(unixBlocker->mutex);
    }
    return ret;
}

int32_t __Blocker_acquire(CBlocker* blocker)
{
    return pthread_mutex_lock(__UnixBlocker_mutex(blocker));
}

int32_t __Blocker_release(CBlocker* blocker)
{
    return pthread_mutex_unlock(__UnixBlocker_mutex(blocker));
}

int32_t __Blocker_wait(CBlocker* blocker)
{
    return pthread_cond_wait(__UnixBlocker_cond(blocker),
                             __UnixBlocker_mutex(blocker));
}

int32_t __Blocker_notify(CBlocker* blocker)
{
    return pthread_cond_signal(__UnixBlocker_cond(blocker));
}

int32_t __Blocker_notifyAll(CBlocker* blocker)
{
    return pthread_cond_broadcast(__UnixBlocker_cond(blocker));
}

void __Blocker_finalize(CBlocker* blocker)
{
    pthread_cond_destroy(__UnixBlocker_cond(blocker));
    if (!blocker->externalLock)
    {
        pthread_mutex_destroy(__UnixBlocker_mutex(blocker));
        free(__UnixBlocker_mutex(blocker));
    }
}
