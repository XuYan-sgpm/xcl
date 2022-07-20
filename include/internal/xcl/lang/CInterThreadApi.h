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
    SUSPEND,
    ALIVE,
    TERMINATED,
    INVALID,
    DETACHED,
    /*
     * mark thread is joining, use for join operation
     * if state is JOINING, detach operation will skip
     */
    JOINING
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
    CThreadState state;
    const unsigned long threadId;
    CSingleList* callStack;
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
 * wait util thread execute finished
 * @param thread thread object
 * @return true if thread is terminated, otherwise false
 */
bool __Thread_wait(CThread* thread);

/**
 * wait thread, return if thread finished
 * or timeout expired
 * @param thread thread object
 * @param timeout wait milliseconds
 * @return true if thread finished, otherwise false
 */
bool __Thread_waitTimeout(CThread* thread, int32_t timeout);

/**
 * create a thread
 * @param suspend if thread suspended after create
 * @param run thread run proc
 * @param usr pointer store data for run use
 * @param handle output pointer store thread handle
 * @return true if create successfully, otherwise false
 */
bool __Thread_create(bool suspend,
                     __ThreadRunProc run,
                     void* usr,
                     ThreadHandle* handle);

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
 * called after waiting thread finished
 * to close thread native handle
 * @param thread thread object
 */
void __Thread_closeHandle(CThread* thread);

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
 * get thread current state
 * @param thread CThread object
 * @return current thread state
 */
CThreadState __Thread_state(CThread* thread);

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
