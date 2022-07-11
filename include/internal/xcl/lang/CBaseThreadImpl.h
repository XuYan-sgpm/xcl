//
// Created by xuyan on 2022/7/10.
//

#pragma once

#include <xcl/lang/CThread.h>
#include <stdint.h>
#include "CLocalStorage.h"

#ifdef __cplusplus
extern "C" {
#endif

#if WINDOWS
typedef unsigned __ThreadRunReturnType;
#  define INVALID_THREAD_HANDLE NULL
#else
typedef void* __ThreadRunReturnType;
#  define INVALID_THREAD_HANDLE (0)
#endif

typedef __ThreadRunReturnType(XCL_API* __ThreadRunProc)(void*);

/**
 * called before thread create
 * @param thread thread object
 */
void
__Thread_beforeCreate(CThread* thread);

/**
 * called after thread create
 * @param thread thread object
 */
void
__Thread_afterCreate(CThread* thread);

/**
 * wait util thread execute finished
 * @param thread thread object
 */
void
__Thread_wait(CThread* thread);

/**
 * wait thread, return if thread finished
 * or timeout expired
 * @param thread thread object
 * @param timeout wait milliseconds
 * @return true if thread finished, otherwise false
 */
bool
__Thread_waitTimeout(CThread* thread, int32_t timeout);

/**
 * create a thread
 * @param suspend if thread suspended after create
 * @param run thread run run
 * @param usr pointer store data for run use
 * @param handle output the thread handle
 * @param tid output the thread id
 * @return true if create successfully, otherwise false
 */
bool
__Thread_create(bool suspend,
                __ThreadRunProc run,
                void* usr,
                ThreadHandle* handle,
                unsigned* tid);

/**
 * resume thread if thread in suspend state
 * @param thread thread object
 */
void
__Thread_resume(CThread* thread);

/**
 * called when enter thread run routine
 * @param thread thread object
 */
void
__Thread_onStart(CThread* thread);

/**
 * called before thread routine return
 * @param thread thread object
 * @param retVal thread return code
 */
void
__Thread_onFinish(CThread* thread, __ThreadRunReturnType retVal);

/**
 * get current thread id
 * @return current thread id
 */
unsigned
__Thread_currentId();

/**
 * get current thread handle
 * @param tid current thread id
 * @return current thread handle
 */
ThreadHandle
__Thread_currentHandle(unsigned tid);

/**
 * release thread resource when thread routine
 * is returned
 * @param thread thread object
 */
void
__Thread_finalize(CThread* thread);

/**
 * detach thread
 * @param thread thread object
 */
void
__Thread_detach(CThread* thread);

typedef enum { SUSPEND, ALIVE, TERMINATED, INVALID, DETACHED } CThreadState;

CLocalStorage*
__Thread_getLocalStorage();
bool
__Thread_setLocalStorage(CLocalStorage* localStorage);
void
__Thread_setState(CThread* thread, CThreadState state);
CThreadState
__Thread_state(CThread* thread);
void*
__Thread_mutex(CThread* thread);
ThreadHandle
__Thread_handle(CThread* thread);

#ifdef __cplusplus
}
#endif
