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
    DETACHED
} CThreadState;

#if WINDOWS
typedef unsigned __ThreadRunReturnType;
typedef void* ThreadHandle;
#    define INVALID_THREAD_HANDLE NULL
#else
#    include <pthread.h>
typedef pthread_t ThreadHandle;
typedef void* __ThreadRunReturnType;
#    define INVALID_THREAD_HANDLE (0)
#endif

typedef struct {
    void (*const cb)(void*);
    void* const usr;
} CallbackObj;

struct _CThread_st {
    const ThreadHandle handle;
    void* const threadLock;
    CThreadState state;
    const unsigned threadId;
    CSingleList* callStack;
};

typedef struct _CThread_st CThread;

typedef __ThreadRunReturnType(XCL_API* __ThreadRunProc)(void*);

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
 */
void __Thread_wait(CThread* thread);

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
 * @param handle output the thread handle
 * @param tid output the thread id
 * @return true if create successfully, otherwise false
 */
bool __Thread_create(bool suspend, __ThreadRunProc run, void* usr,
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
 * called before thread routine return
 * @param thread thread object
 * @param retVal thread return code
 */
void __Thread_onFinish(CThread* thread, __ThreadRunReturnType retVal);

/**
 * get current thread id
 * @return current thread id
 */
unsigned __Thread_currentId();

/**
 * get current thread handle
 * @param tid current thread id
 * @return current thread handle
 */
ThreadHandle __Thread_currentHandle();

/**
 * release thread resource when thread routine
 * is returned
 * @param thread thread object
 */
void __Thread_finalize(CThread* thread);

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
void* __Thread_mutex(CThread* thread);

/**
 * get thread handle
 * @param thread CThread object
 * @return thread handle
 */
ThreadHandle __Thread_handle(CThread* thread);

/**
 * release current thread local storage
 */
void __Thread_releaseLocalStorage();

#ifdef __cplusplus
}
#endif
