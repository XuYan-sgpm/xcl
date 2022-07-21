//
// Created by xuyan on 2022/7/10.
//

#include "xcl/lang/CThread.h"
#include "xcl/concurrent/CMutex.h"
#include "xcl/lang/CLocalStorage.h"
#include "xcl/lang/CInterThreadApi.h"
#include "xcl/lang/CThreadLocal.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

/**
 * position 0 used to store current CThread object
 */
static CThreadLocal __Thread_localObject = {0};

static inline void __Thread_setHandle(CThread* thread, ThreadHandle handle)
{
    memcpy((void*)&thread->handle, (void*)&handle, sizeof(ThreadHandle));
}

static inline CSingleNode* __Thread_allocCbNode()
{
    return Pool_alloc(NULL, sizeof(CSingleNode) + sizeof(CallbackObj));
}

static inline void __Thread_deallocCbNode(CSingleNode* node)
{
    Pool_dealloc(NULL, node, sizeof(CSingleNode) + sizeof(CallbackObj));
}

static inline bool __Thread_initLock(CThread* thread)
{
    CMutex* lock = Mutex_new();
    if (lock)
    {
        memcpy((void*)&thread->threadLock, (void*)&lock, sizeof(void*));
    }
    return lock;
}

static inline void __Thread_setId(CThread* thread, unsigned long tid)
{
    memcpy((void*)&thread->threadId, (void*)&tid, sizeof(tid));
}

static inline bool __Thread_initCallStack(CThread* thread)
{
    thread->cleanStack = SingleList_new();
    return thread->cleanStack;
}

static inline bool
__Thread_setRunProc(CThread* thread, Callback proc, void* usr)
{
    CallbackObj callbackObj = {.cb = proc, .usr = usr};
    memcpy((void*)&thread->run, &callbackObj, sizeof(CallbackObj));
    return true;
}

static bool __Thread_wait(CThread* thread, int32_t timeout)
{
    bool success = true;
    Mutex_unlock(__Thread_mutex(thread));
    if (timeout < 0)
    {
        success = __Thread_join(thread);
    }
    else
    {
        success = __Thread_joinTimeout(thread, timeout);
    }
    Mutex_lock(__Thread_mutex(thread));
    return success;
}

static bool __Thread_addCb(CThread* thread, bool front, Callback cb, void* usr)
{
    Mutex_lock(thread->threadLock);
    bool ret = false;
    if (thread->state == SUSPEND)
    {
        CallbackObj cbObj = {.cb = cb, .usr = usr};
        CSingleNode* node = __Thread_allocCbNode();
        if (node)
        {
            memcpy(node->data, &cbObj, sizeof(CallbackObj));
            front ? SingleList_pushFront(thread->cleanStack, node)
                  : SingleList_pushBack(thread->cleanStack, node);
            ret = true;
        }
    }
    Mutex_unlock(thread->threadLock);
    return ret;
}

void __Thread_releaseLocalStorage()
{
    CLocalStorage* localStorage = __Thread_getLocalStorage();
    if (localStorage)
    {
        __Thread_setLocalStorage(NULL);
        LocalStorage_delete(localStorage);
    }
}

void __Thread_setState(CThread* thread, CThreadState state)
{
    thread->state |= state;
}

bool __Thread_inState(CThread* thread, CThreadState state)
{
    return thread->state & state;
}

void __Thread_resetState(CThread* thread, CThreadState state)
{
    thread->state = state;
}

void __Thread_rmState(CThread* thread, CThreadState state)
{
    int32_t val = state;
    thread->state &= ~val;
}

static bool __Thread_checkStates(CThread* thread, bool exclude, int n, ...)
{
    va_list states;
    va_start(states, n);
    bool ret = true;
    for (int i = 0; i < n; i++)
    {
        if (exclude == __Thread_inState(thread, va_arg(states, CThreadState)))
        {
            ret = false;
            break;
        }
    }
    va_end(states);
    return ret;
}

static bool __Thread_hasOneOfStates(CThread* thread, int n, ...)
{
    va_list states;
    va_start(states, n);
    bool ret = false;
    for (int i = 0; i < n; i++)
    {
        if (__Thread_inState(thread, va_arg(states, CThreadState)))
        {
            ret = true;
            break;
        }
    }
    va_end(states);
    return ret;
}

CMutex* __Thread_mutex(CThread* thread)
{
    return thread->threadLock;
}

ThreadHandle __Thread_handle(CThread* thread)
{
    return thread->handle;
}

void XCL_API __Thread_run(void* args)
{
    CThread* thread = args;
    __Thread_setId(thread, __Thread_currentId());
    Local_set(&__Thread_localObject, thread);
    __Thread_onStart(thread);
    /*
     * execute thread run proc
     * thread run proc is always the first callback
     * to execute
     */
    thread->run.cb(thread->run.usr);
    /*
     * set thread state to CLEANING and remote NORMAL state
     * CLEANING and NORMAL can not set at a time
     */
    Mutex_lock(__Thread_mutex(thread));
    assert(__Thread_checkStates(thread,
                                true,
                                4,
                                INVALID,
                                SUSPEND,
                                TERMINATED,
                                CLEANING));
    assert(__Thread_checkStates(thread, false, 1, NORMAL));
    __Thread_rmState(thread, NORMAL);
    __Thread_setState(thread, CLEANING);
    Mutex_unlock(__Thread_mutex(thread));
    /*
     * now after thread run finished, we execute clean stack
     */
    CSingleNode* node;
    while ((node = SingleList_popFront(thread->cleanStack)))
    {
        CallbackObj* cbObj = (CallbackObj*)node->data;
        cbObj->cb(cbObj->usr);
        __Thread_deallocCbNode(node);
    }
    SingleList_delete(thread->cleanStack);
    thread->cleanStack = NULL;
    __Thread_onFinish(thread);
    __Thread_releaseLocalStorage();
    /*
     * check thread state
     * if thread is in CLEANING or WAITING, set state to TERMINATED
     * otherwise, Thread_recycle invoked, we need to delete thread object
     */
    Mutex_lock(__Thread_mutex(thread));
    assert(__Thread_checkStates(thread,
                                true,
                                4,
                                INVALID,
                                SUSPEND,
                                TERMINATED,
                                NORMAL));
    assert(__Thread_checkStates(thread, false, 1, CLEANING));
    __Thread_rmState(thread, CLEANING);
    bool detached = false;
    if (!__Thread_inState(thread, DETACHED))
    {
        __Thread_resetState(thread, TERMINATED);
    }
    else
    {
        detached = true;
        __Thread_detach(thread);
    }
    Mutex_unlock(__Thread_mutex(thread));
    if (detached)
    {
        Mutex_delete(__Thread_mutex(thread));
        __Thread_dealloc(thread);
    }
}

XCL_PUBLIC(CThread*)
Thread_new(bool suspend, Callback cb, void* usr)
{
    bool success = false;
    CThread* thread = __Thread_alloc();
    if (!thread)
    {
        return NULL;
    }
    memset(thread, 0, sizeof(CThread));
    __Thread_resetState(thread, INVALID);
    if (__Thread_initLock(thread))
    {
        if (__Thread_initCallStack(thread))
        {
            __Thread_beforeCreate(thread);
            __Thread_setRunProc(thread, cb, usr);
            /**
             * we set thread state to suspend before create handle
             * because if thread is running, all thread members
             * are initialized
             * otherwise, thread create failed, and thread
             * run proc would not be executed, so we set to INVALID
             */
            __Thread_resetState(thread, SUSPEND);
            ThreadHandle h;
            if (__Thread_create(true, thread, &h))
            {
                __Thread_setHandle(thread, h);
                success = true;
            }
            else
            {
                __Thread_resetState(thread, INVALID);
            }
            __Thread_afterCreate(thread);
            if (success)
            {
                if (!suspend)
                {
                    __Thread_resume(thread);
                }
                return thread;
            }
            assert(SingleList_delete(thread->cleanStack));
        }
        Mutex_delete(thread->threadLock);
    }
    __Thread_dealloc(thread);
    return NULL;
}

XCL_PUBLIC(CThread*)
Thread_current()
{
    CThread* thread = NULL;
    if (!Local_get(&__Thread_localObject, (void**)&thread))
    {
        return NULL;
    }
    return thread;
}

XCL_PUBLIC(bool)
Thread_postClean(CThread* thread, Callback cb, void* usr)
{
    return __Thread_addCb(thread, false, cb, usr);
}

XCL_PUBLIC(void)
Thread_start(CThread* thread)
{
    Mutex_lock(thread->threadLock);
    assert(__Thread_checkStates(thread, true, 1, INVALID));
    if (__Thread_inState(thread, SUSPEND))
    {
        __Thread_resume(thread);
    }
    Mutex_unlock(thread->threadLock);
}

XCL_PUBLIC(bool)
Thread_isAlive(CThread* thread)
{
    Mutex_lock(thread->threadLock);
    assert(__Thread_checkStates(thread, true, 1, INVALID));
    bool ret = __Thread_hasOneOfStates(thread, 3, NORMAL, WAITING, CLEANING);
    Mutex_unlock(thread->threadLock);
    return ret;
}

XCL_PUBLIC(bool)
Thread_join(CThread* thread)
{
    return Thread_join2(thread, -1);
}

XCL_PUBLIC(bool)
Thread_join2(CThread* thread, int32_t timeout)
{
    bool success = false;
    Mutex_lock(__Thread_mutex(thread));
    assert(__Thread_checkStates(thread, true, 2, INVALID, DETACHED));
    if (__Thread_hasOneOfStates(thread, 3, NORMAL, CLEANING))
    {
        __Thread_setState(thread, WAITING);
        success = __Thread_wait(thread, timeout);
    }
    else if (__Thread_inState(thread, TERMINATED))
    {
        success = __Thread_wait(thread, timeout);
    }
    Mutex_unlock(__Thread_mutex(thread));
    if (success)
    {
        Mutex_delete(__Thread_mutex(thread));
        __Thread_dealloc(thread);
    }
    return success;
}

XCL_PUBLIC(bool) Thread_recycle(CThread* thread)
{
    bool success = false;
    /**
     * check thread state, only apply for ALIVE
     * or TERMINATED. if state is ALIVE, set thread
     * state to DETACHED so that thread object will
     * be deleted in thread run proc;otherwise, if
     * state is TERMINATED, thread run proc is returned
     * so we need to delete thread object manually
     */
    Mutex_lock(__Thread_mutex(thread));
    assert(__Thread_checkStates(thread, true, 3, INVALID, DETACHED, WAITING));
    bool terminated = false;
    if (__Thread_inState(thread, SUSPEND))
    {
        /*
         * if thread is in suspend state, need to start thread
         * Thread_recycle will do nothing in this case
         */
    }
    if (__Thread_hasOneOfStates(thread, 2, CLEANING, NORMAL))
    {
        __Thread_setState(thread, DETACHED);
        success = true;
    }
    else
    {
        terminated = __Thread_inState(thread, TERMINATED);
        assert(terminated);
        success = __Thread_join(thread);
    }
    Mutex_unlock(__Thread_mutex(thread));
    if (terminated && success)
    {
        Mutex_delete(__Thread_mutex(thread));
        __Thread_dealloc(thread);
    }
    return success;
}

XCL_PUBLIC(unsigned long)
Thread_currentId()
{
    return __Thread_currentId();
}
