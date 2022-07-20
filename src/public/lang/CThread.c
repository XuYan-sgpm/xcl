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

/**
 * position 0 used to store current CThread object
 */
static CThreadLocal __localThread = {0};

static inline void __Thread_setThreadHandle(CThread* thread,
                                            ThreadHandle handle)
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

static inline bool __Thread_initThreadLock(CThread* thread)
{
    CMutex* lock = Mutex_new();
    if (lock)
    {
        memcpy((void*)&thread->threadLock, (void*)&lock, sizeof(void*));
    }
    return lock;
}

static inline void __Thread_setThreadId(CThread* thread, unsigned long tid)
{
    memcpy((void*)&thread->threadId, (void*)&tid, sizeof(tid));
}

static inline bool __Thread_initThreadCallStack(CThread* thread)
{
    thread->callStack = SingleList_new();
    return thread->callStack;
}

static inline bool __Thread_setThreadProc(CThread* thread, Callback proc,
                                          void* usr)
{
    CallbackObj callbackObj = {.cb = proc, .usr = usr};
    CSingleNode* node = __Thread_allocCbNode();
    if (node)
    {
        memcpy((CallbackObj*)node->data, &callbackObj, sizeof(CallbackObj));
        SingleList_pushBack(thread->callStack, node);
    }
    return node;
}

static inline CThreadState __Thread_syncGetThreadState(CThread* thread)
{
    Mutex_lock(thread->threadLock);
    CThreadState state = thread->state;
    Mutex_unlock(thread->threadLock);
    return state;
}

static inline void __Thread_syncSetThreadState(CThread* thread,
                                               CThreadState state)
{
    Mutex_lock(thread->threadLock);
    thread->state = state;
    Mutex_unlock(thread->threadLock);
}

static inline bool __Thread_block(CThread* thread, int32_t timeout)
{
    bool success = true;
    Mutex_unlock(__Thread_mutex(thread));
    if (timeout < 0)
    {
        do
        {
            if (!__Thread_wait(thread))
            {
                success = false;
                break;
            }
        } while (__Thread_syncGetThreadState(thread) != TERMINATED);
    }
    else
    {
        success = __Thread_waitTimeout(thread, timeout);
    }
    Mutex_lock(__Thread_mutex(thread));
    return success;
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
    thread->state = state;
}

CThreadState __Thread_state(CThread* thread)
{
    return thread->state;
}

CMutex* __Thread_mutex(CThread* thread)
{
    return thread->threadLock;
}

ThreadHandle __Thread_handle(CThread* thread)
{
    return thread->handle;
}

static __ThreadRunReturnType XCL_API __Thread_run(void* args)
{
    CThread* thread = args;
    __Thread_setThreadId(thread, __Thread_currentId());
    Local_set(&__localThread, thread);
    __Thread_onStart(thread);
    CSingleNode* node;
    while ((node = SingleList_popFront(thread->callStack)))
    {
        CallbackObj* cbObj = (CallbackObj*)node->data;
        cbObj->cb(cbObj->usr);
        __Thread_deallocCbNode(node);
    }
    SingleList_delete(thread->callStack);
    thread->callStack = NULL;
    __Thread_onFinish(thread);
    __Thread_releaseLocalStorage();
    /**
     * check thread state, if state is ALIVE,
     * we just set state to TERMINATED, otherwise
     * thread state must be DETACHED, so we need
     * to delete thread object
     */
    Mutex_lock(__Thread_mutex(thread));
    CThreadState state = __Thread_state(thread);
    assert(state == ALIVE || state == DETACHED || state == JOINING);
    if (state == ALIVE || state == JOINING)
    {
        __Thread_setState(thread, TERMINATED);
    }
    else
    {
        __Thread_detach(thread);
    }
    Mutex_unlock(__Thread_mutex(thread));
    if (state == DETACHED)
    {
        Mutex_delete(__Thread_mutex(thread));
        __Thread_dealloc(thread);
    }
    return 0;
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
    thread->state = INVALID;
    if (__Thread_initThreadLock(thread))
    {
        if (__Thread_initThreadCallStack(thread))
        {
            __Thread_beforeCreate(thread);
            __Thread_setThreadProc(thread, cb, usr);
            ThreadHandle handle;
            /**
                 * we set thread state to suspend before create handle
                 * because if thread is running, all thread members
                 * are initialized
                 * otherwise, thread create failed, and thread
                 * run proc would not be executed, so we set to INVALID
                 */
            thread->state = SUSPEND;
            if (__Thread_create(true, __Thread_run, thread, &handle))
            {
                __Thread_setThreadHandle(thread, handle);
                success = true;
            }
            else
            {
                thread->state = INVALID;
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
            __Thread_deallocCbNode(SingleList_popFront(thread->callStack));
            SingleList_delete(thread->callStack);
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
    if (!Local_get(&__localThread, (void**)&thread))
    {
        return NULL;
    }
    return thread;
}

XCL_PUBLIC(bool)
Thread_addCbFront(CThread* thread, Callback cb, void* usr)
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
            SingleList_pushFront(thread->callStack, node);
            ret = true;
        }
    }
    Mutex_unlock(thread->threadLock);
    return ret;
}

XCL_PUBLIC(bool)
Thread_addCbBack(CThread* thread, Callback cb, void* usr)
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
            SingleList_pushBack(thread->callStack, node);
            ret = true;
        }
    }
    Mutex_unlock(thread->threadLock);
    return ret;
}

XCL_PUBLIC(bool)
Thread_delete(CThread* thread)
{
    unsigned long tid = __Thread_currentId();
    bool success = false;
    if (tid != thread->threadId)
    {
        Mutex_lock(__Thread_mutex(thread));
        CThreadState state = __Thread_state(thread);
        if (state == SUSPEND)
        {
            __Thread_resume(thread);
        }
        if (state == ALIVE)
        {
            success = __Thread_block(thread, -1);
            if (success)
            {
                assert(__Thread_state(thread) == TERMINATED);
                __Thread_closeHandle(thread);
            }
        }
        else if (state == TERMINATED)
        {
            success = true;
        }
        else
        {
            assert(false);
        }
        Mutex_unlock(__Thread_mutex(thread));
        if (success)
        {
            Mutex_delete(thread->threadLock);
            __Thread_dealloc(thread);
        }
    }
    return success;
}

XCL_PUBLIC(void)
Thread_start(CThread* thread)
{
    Mutex_lock(thread->threadLock);
    if (thread->state == SUSPEND)
    {
        __Thread_resume(thread);
    }
    Mutex_unlock(thread->threadLock);
}

XCL_PUBLIC(bool)
Thread_isAlive(CThread* thread)
{
    Mutex_lock(thread->threadLock);
    bool ret = thread->state == ALIVE;
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
    if (__Thread_state(thread) == ALIVE)
    {
        __Thread_setState(thread, JOINING);
        if (__Thread_block(thread, timeout))
        {
            assert(__Thread_state(thread) == TERMINATED);
            //            __Thread_closeHandle(thread);
            success = true;
        }
    }
    Mutex_unlock(__Thread_mutex(thread));
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
    CThreadState state = __Thread_state(thread);
    if (state == ALIVE)
    {
        success = true;
        __Thread_setState(thread, DETACHED);
    }
    else if (state == TERMINATED)
    {
        success = true;
        if (__Thread_wait(thread))
        {
            __Thread_closeHandle(thread);
        }
    }
    Mutex_unlock(__Thread_mutex(thread));
    if (state == TERMINATED)
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
