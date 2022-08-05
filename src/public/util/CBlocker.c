//
// Created by xy on 7/14/22.
//

#include <stdlib.h>
#include "xcl/lang/XclErr.h"
#include "xcl/util/CBlocker.h"
#include "xcl/concurrent/CCond.h"
#include "xcl/pool/CPool.h"

struct _CBlocker_st {
    int32_t wait;
    int32_t notify;
    bool externalLock;
    CMutex* mutex;
    CCond* cond;
};

/**
 * get blocker state, if state is <= 0, blocker is in signaled
 * state; otherwise, blocker in blocking state
 * @param blocker CBlocker object
 * @return blocker current state
 */
static inline int32_t
__Blocker_state(CBlocker* blocker)
{
    return blocker->wait - blocker->notify;
}

static bool
__Blocker_init(CBlocker* blocker, CMutex* mutex)
{
    blocker->wait = blocker->notify = 0;
    blocker->externalLock = mutex;
    if (mutex)
    {
        blocker->mutex = mutex;
    }
    else
    {
        blocker->mutex = Mutex_new();
    }
    if (blocker->mutex)
    {
        blocker->cond = Cond_new();
        if (blocker->cond)
        {
            return true;
        }
        if (!mutex)
        {
            Mutex_delete(blocker->mutex);
        }
    }
    return false;
}

static CBlocker*
__Blocker_new(CMutex* mutex)
{
    CBlocker* blocker = Pool_alloc(Pool_def(), sizeof(CBlocker));
    if (blocker)
    {
        if (!__Blocker_init(blocker, mutex))
        {
            Pool_dealloc(Pool_def(), blocker, sizeof(CBlocker));
            blocker = 0;
        }
    }
    else
    {
        Err_set(XCL_MEMORY_ERR);
    }
    return blocker;
}

XCL_EXPORT CBlocker* XCL_API
Blocker_new()
{
    return __Blocker_new(NULL);
}

XCL_EXPORT CBlocker* XCL_API
Blocker_new2(CMutex* mutex)
{
    return __Blocker_new(mutex);
}

XCL_EXPORT bool XCL_API
Blocker_delete(CBlocker* blocker)
{
    bool allow = false;
    if (!Mutex_lock(blocker->mutex))
    {
        return false;
    }
    if (__Blocker_state(blocker) <= 0)
    {
        allow = true;
    }
    Mutex_unlock(blocker->mutex);
    if (allow)
    {
        if (!blocker->externalLock)
        {
            Mutex_delete(blocker->mutex);
        }
        Cond_delete(blocker->cond);
        Pool_dealloc(Pool_def(), blocker, sizeof(CBlocker));
    }
    return allow;
}

XCL_EXPORT bool XCL_API
Blocker_wait(CBlocker* blocker)
{
    if (!Mutex_lock(blocker->mutex))
    {
        return false;
    }
    ++blocker->wait;
    bool success = true;
    if (__Blocker_state(blocker) > 0)
    {
        success = Cond_wait(blocker->mutex, blocker->cond);
    }
    Mutex_unlock(blocker->mutex);
    return success;
}

XCL_EXPORT bool XCL_API
Blocker_cancel(CBlocker* blocker)
{
    if (!Mutex_lock(blocker->mutex))
    {
        return false;
    }
    bool success = true;
    int32_t state = __Blocker_state(blocker);
    if (state >= 0)
    {
        if (state > 0)
        {
            success = Cond_signal(blocker->cond);
        }
        if (success)
        {
            ++blocker->notify;
        }
    }
    Mutex_unlock(blocker->mutex);
    return success;
}

XCL_EXPORT bool XCL_API
Blocker_wakeAll(CBlocker* blocker)
{
    if (!Mutex_lock(blocker->mutex))
    {
        return false;
    }
    bool success = true;
    if (__Blocker_state(blocker) > 0)
    {
        if (Cond_signalAll(blocker->cond))
        {
            blocker->wait = blocker->notify = 0;
        }
        else
        {
            success = false;
        }
    }
    Mutex_unlock(blocker->mutex);
    return success;
}
