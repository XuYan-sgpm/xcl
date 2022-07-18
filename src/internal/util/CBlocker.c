//
// Created by xy on 7/14/22.
//

#include <stdlib.h>
#include <xcl/lang/XclErr.h>
#include <xcl/util/CBlockerInternalApi.h>

/**
 * get blocker state, if state is <= 0, blocker is in signaled
 * state; otherwise, blocker in blocking state
 * @param blocker CBlocker object
 * @return blocker current state
 */
static inline int32_t __Blocker_state(CBlocker* blocker)
{
    return blocker->wait - blocker->notify;
}

XCL_PUBLIC(CBlocker*)
Blocker_new()
{
    CBlocker* blocker = __Blocker_alloc();
    if (blocker)
    {
        blocker->wait = blocker->notify = 0;
        blocker->externalLock = false;
        int ret = __Blocker_init(blocker, 0);
        if (ret)
        {
            setErr(ret);
            __Blocker_dealloc(blocker);
            blocker = 0;
        }
    }
    else { setErr(XCL_MEMORY_ERR); }
    return blocker;
}

XCL_PUBLIC(CBlocker*)
Blocker_new2(void* mutex)
{
    CBlocker* blocker = __Blocker_alloc();
    if (blocker)
    {
        blocker->wait = blocker->notify = 0;
        blocker->externalLock = true;
        int ret = __Blocker_init(blocker, mutex);
        if (ret)
        {
            setErr(ret);
            free(blocker);
            blocker = 0;
        }
    }
    else { setErr(XCL_MEMORY_ERR); }
    return blocker;
}

XCL_PUBLIC(bool)
Blocker_delete(CBlocker* blocker)
{
    int ret = __Blocker_acquire(blocker);
    bool allow = false;
    if (ret != 0) { setErr(ret); }
    else
    {
        if (__Blocker_state(blocker) <= 0) { allow = true; }
        __Blocker_release(blocker);
        if (allow)
        {
            __Blocker_finalize(blocker);
            free(blocker);
        }
    }
    return allow;
}

XCL_PUBLIC(bool)
Blocker_wait(CBlocker* blocker)
{
    int ret = __Blocker_acquire(blocker);
    if (ret != 0) { setErr(ret); }
    else
    {
        ++blocker->wait;
        if (__Blocker_state(blocker) > 0)
        {
            ret = __Blocker_wait(blocker);
            if (ret) setErr(ret);
        }
    }
    __Blocker_release(blocker);
    return ret == 0;
}

XCL_PUBLIC(bool)
Blocker_cancel(CBlocker* blocker)
{
    int ret = __Blocker_acquire(blocker);
    if (ret != 0) { setErr(ret); }
    else
    {
        int32_t state = __Blocker_state(blocker);
        if (state >= 0)
        {
            if (state > 0)
            {
                ret = __Blocker_notify(blocker);
                if (ret) setErr(ret);
            }
            if (ret == 0) { ++blocker->notify; }
        }
    }
    __Blocker_release(blocker);
    return ret == 0;
}

XCL_PUBLIC(bool)
Blocker_wakeAll(CBlocker* blocker)
{
    int ret = __Blocker_acquire(blocker);
    if (ret != 0) { setErr(ret); }
    else
    {
        if (__Blocker_state(blocker) > 0)
        {
            ret = __Blocker_notifyAll(blocker);
            if (ret == 0) { blocker->wait = blocker->notify = 0; }
            else { setErr(ret); }
        }
    }
    __Blocker_release(blocker);
    return ret == 0;
}
