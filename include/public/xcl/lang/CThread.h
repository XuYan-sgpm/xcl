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
XCL_PUBLIC(unsigned long)
Thread_currentId();

/**
 * add clean callback, even if thread is started
 * however, if thread run proc is not returned,
 * you can post clean cb successfully, the clean cb
 * will be executed after thread run proc finished
 * @param thread thread object
 * @param cb callback
 * @param usr callback parameter
 * @return true if cb added successfully, otherwise false
 */
XCL_PUBLIC(bool)
Thread_postClean(CThread* thread, Callback cb, void* usr);

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
 * wait util thread is terminated and delete thread object
 * thread join must be called at last and can only apple once
 * @param thread thread object
 * @return true if join successfully, otherwise false
 */
XCL_PUBLIC(bool)
Thread_join(CThread* thread);

/**
 * as same as Thread_join, can only called once and must
 * be called at last
 * wait thread util thread terminated during timeout milliseconds
 * if thread was already terminated, delete thread object
 * @param thread thread object
 * @param timeout milliseconds
 * @return true if thread is terminated, otherwise false
 */
XCL_PUBLIC(bool)
Thread_join2(CThread* thread, int32_t timeout);

/**
 * this function as same as Thread_delete, must called once at last
 * because Thread_recycle will make thread object invalidate, so
 * any invoke after Thread_recycle is undefined behaviour
 * recycle thread object, for these case
 * if thread is not started, do nothing
 * if thread is running, mark thread object is invalidate
 * if thread is terminated, delete thread object
 * Thread_recycle will be return immediately for any case
 * @param thread thread object
 * @return true if successfully, otherwise false
 */
XCL_PUBLIC(bool)
Thread_recycle(CThread* thread);

#ifdef __cplusplus
}
#endif
