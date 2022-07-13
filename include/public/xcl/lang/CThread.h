//
// Created by xuyan on 2022/7/6.
//

/**
 * a cross-platform thread api for pure C or C++ use
 * support thread local(CThreadLocal)
 * remember, thread local will be released when thread
 * object is terminated only if thread is created by
 * Thread_new. if not, thread local is also available,
 * but not guaranteed to be released when thread is
 * terminated. thread local will be released when current
 * process is terminated.
 */

#pragma once

#include "XclDef.h"
#include <stdbool.h>
#include <stdint.h>

typedef void (*Callback)(void*);

#if WINDOWS
typedef void* ThreadHandle;
#else
#  include <pthread.h>
typedef pthread_t ThreadHandle;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CThread_st CThread;

/**
 * new a thread object
 * @param suspend if thread suspend when created
 * @return thread object if successfully, otherwise false
 */
XCL_PUBLIC(CThread*)
Thread_new(bool suspend, Callback cb, void* usr);

/**
 * get current CThread object
 * if current thread is created by system api
 * not with Thread_new, Thread_current will
 * return NULL
 * @return thread object current running
 */
XCL_PUBLIC(CThread*)
Thread_current();

/**
 * get current thread id
 * @return current thread id
 */
XCL_PUBLIC(unsigned)
Thread_currentId();

/**
 * get current thread handle
 * @return current thread handle
 */
XCL_PUBLIC(ThreadHandle)
Thread_currentHandle();

/**
 * add cb for front call, cb will called first if no any other
 * cb added to the front
 * @param thread thread object
 * @param cb callback
 * @param usr callback parameter
 * @return true if add cb successfully, otherwise false
 */
XCL_PUBLIC(bool)
Thread_addCbFront(CThread* thread, Callback cb, void* usr);

/**
 * add cb for back call, cb will called last if no any other
 * cb added to the back
 * @param thread thread object
 * @param cb callback
 * @param usr callback parameter
 * @return true if cb added successfully, otherwise false
 */
XCL_PUBLIC(bool)
Thread_addCbBack(CThread* thread, Callback cb, void* usr);

/**
 * delete thread object
 * warning! this function is not thread safe and
 * can only called once, and can not invoked together
 * with other Thread_xxx apis
 * @param thread thread object
 * @return true if thread is joined or detached before
 * delete call, otherwise return false
 * @return true if delete successfully, otherwise false
 * Thread_delete return false only when delete current
 * thread object, you must delete one thread object in
 * another thread
 */
XCL_PUBLIC(bool)
Thread_delete(CThread* thread);

/**
 * start a thread if thread is suspended
 * @param thread thread object
 */
XCL_PUBLIC(void)
Thread_start(CThread* thread);

/**
 * handle if thread is still alive
 * @param thread thread object
 * @return true if thread is created and not terminated
 *
 */
XCL_PUBLIC(bool)
Thread_isAlive(CThread* thread);

/**
 * wait util thread is terminated
 * @param thread thread object
 */
XCL_PUBLIC(void)
Thread_join(CThread* thread);

/**
 * wait thread for terminated during timeout milliseconds
 * if thread was already terminated, function succeed and
 * set terminated to true
 * @param thread thread object
 * @param timeout milliseconds
 * @param terminated if thread is terminated
 * @return 0 if function succeed, otherwise -1
 */
XCL_PUBLIC(int32_t)
Thread_join2(CThread* thread, int32_t timeout, bool* terminated);

/**
 * detach thread
 * @param thread thread object
 */
XCL_PUBLIC(void)
Thread_detach(CThread* thread);

#ifdef __cplusplus
}
#endif
