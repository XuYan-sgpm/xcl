//
// Created by xuyan on 2022/7/10.
//

#pragma once

/**
 * all apis in this header should not be invoked
 * directly, please using CThread.h api instead
 * these apis only for developer to develop thread
 * impl for different platforms
 */

#include "CLocalStorage.h"
#include "xcl/util/CSingleList.h"
#include "xcl/concurrent/CMutex.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    /*
     * thread handle is created but thread is not running
     */
    SUSPEND = 1,
    /*
     * thread is executing run proc, without join or detach operations
     * when run proc is finished, thread will execute clean callbacks
     */
    NORMAL = 1 << 1,
    /*
     * thread run proc is returned
     */
    TERMINATED = 1 << 2,
    /*
     * create thread handle failed
     */
    INVALID = 1 << 3,
    /*
     * thread is detached, thread handle and associated native resources
     * will be released by system
     */
    DETACHED = 1 << 4,
    /*
     * thread is in join progress, when join finished thread object will
     * be deleted
     */
    WAITING = 1 << 5,
    /*
     * thread is calling clean callbacks and release resources
     * this state set when thread routine
     */
    CLEANING = 1 << 6
} CThreadState;

typedef uintptr_t ThreadHandle;

#define INVALID_THREAD_HANDLE (0)

typedef struct {
    void (*const cb)(void*);
    void* const usr;
} CallbackObj;

/**
 * basic thread structure, may be need
 * to expand on different platforms
 */
struct _CThread_st {
    const ThreadHandle handle;
    CMutex* const threadLock;
    uint32_t state;
    const unsigned long threadId;
    const CallbackObj run;
    CSingleList* cleanStack;
};

typedef struct _CThread_st CThread;

typedef void(XCL_API* __ThreadRunProc)(void*);

/**
 * functions below are strong associated with platform
 * implementation is required for CThread api
 */

/**
 * alloc a thread object
 * @return thread object if successfully, otherwise NULL
 */
CThread* __Thread_alloc();

/**
 * dealloc thread object
 * @param thread thread object
 */
void __Thread_dealloc(CThread* thread);

/**
 * called before thread create
 * @param thread thread object
 */
void __Thread_beforeCreate(CThread* thread);

/**
 * called after thread create
 * @param thread thread object
 */
void __Thread_afterCreate(CThread* thread);

/**
 * wait util thread execute finished and release thread handle
 * @param thread thread object
 * @return true if thread is terminated, otherwise false
 */
bool __Thread_join(CThread* thread);

/**
 * wait thread until thread terminated or timeout expired
 * if thread terminated, release thread handle
 * @param thread thread object
 * @param timeout wait milliseconds
 * @return true if thread terminated, otherwise false
 */
bool __Thread_joinTimeout(CThread* thread, int32_t timeout);

/**
 * create a thread, auto use __Thread_run as main proc
 * @param suspend if thread suspended after create
 * @param usr pointer store data for run use
 * @param handle output pointer store thread handle
 * @return true if create successfully, otherwise false
 */
bool __Thread_create(bool suspend, void* usr, ThreadHandle* handle);

/**
 * resume thread if thread in suspend state
 * @param thread thread object
 */
void __Thread_resume(CThread* thread);

/**
 * called when enter thread run routine
 * @param thread thread object
 */
void __Thread_onStart(CThread* thread);

/**
 * called before thread run proc is prepared to return
 * @param thread thread object
 */
void __Thread_onFinish(CThread* thread);

/**
 * get current thread id
 * @return current thread id
 */
unsigned long __Thread_currentId();

/**
 * get current thread handle
 * @param tid current thread id
 * @return current thread handle
 */
ThreadHandle __Thread_currentHandle();

/**
 * detach thread
 * @param thread thread object
 */
void __Thread_detach(CThread* thread);

/**
 * get current thread localstorage
 * @return local storage
 */
CLocalStorage* __Thread_getLocalStorage();

/**
 * set current thread localstorage
 * @param localStorage local storage
 * @return true if set localstorage successfully, otherwise false
 */
bool __Thread_setLocalStorage(CLocalStorage* localStorage);

/**
 * check if thread is still alive
 * @param handle thread handle
 * @return true if thread is alive, otherwise false
 */
bool __Thread_isAlive(ThreadHandle handle);

/**
 * functions below are already implemented in CThread.c
 * and do not implement these apis if you want to reuse
 * implementation in CThread.c
 * (for example, implement thread for specified platforms)
 */

/**
 * set thread current state
 * @param thread CThread object
 * @param state CThreadState
 */
void __Thread_setState(CThread* thread, CThreadState state);

/**
 * get if thread is in specified thread state
 * @param thread thread object
 * @param state thread state
 * @return true if thread is in specified thread state, otherwise false
 */
bool __Thread_inState(CThread* thread, CThreadState state);

/**
 * clean all thread states set before, and reset current thread state
 * @param thread thread object
 * @param state specified thread state
 */
void __Thread_resetState(CThread* thread, CThreadState state);

/**
 * remote specified thread state
 * @param thread thread object
 * @param state thread state
 */
void __Thread_rmState(CThread* thread, CThreadState state);

/**
 * get thread mutex
 * @param thread CThread object
 * @return mutex lock thread using
 */
CMutex* __Thread_mutex(CThread* thread);

/**
 * get thread handle
 * @param thread thread object
 * @return thread native handle
 */
ThreadHandle __Thread_handle(CThread* thread);

/**
 * thread run proc
 * @param args thread run proc args
 */
void XCL_API __Thread_run(void* args);

#ifdef __cplusplus
}
#endif
