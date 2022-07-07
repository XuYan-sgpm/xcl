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
 * And also remember, thread local is not manage thread
 * local object data. for example, if you alloc memory,
 * and put address into thread local, you must free memory
 * manually, no any memory release operation when thread local
 * is not available or thread local data changed.
 */

#pragma once

#include <lang/XclDef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CThread_st CThread;

/**
 * new a thread object
 * @param suspend if thread suspend when created
 * @return thread object if successfully, otherwise false
 */
XCL_PUBLIC CThread* XCL_API
Thread_new(bool suspend,unsigned (*threadProc)(void*),void*usr);

/**
 * get current thread object
 * @return thread object current running
 */
XCL_PUBLIC CThread* XCL_API
Thread_current();

/**
 * add cb for front call, cb will called first if no any other
 * cb added to the front
 * @param thread thread object
 * @param cb callback
 * @param usr callback parameter
 * @return true if add cb successfully, otherwise false
 */
XCL_PUBLIC bool XCL_API
Thread_addCbFront(CThread* thread, void (*cb)(void*), void* usr);

/**
 * add cb for back call, cb will called last if no any other
 * cb added to the back
 * @param thread thread object
 * @param cb callback
 * @param usr callback parameter
 * @return true if cb added successfully, otherwise false
 */
XCL_PUBLIC bool XCL_API
Thread_addCbBack(CThread* thread, void (*cb)(void*), void* usr);

/**
 * delete thread object
 * @param thread thread object
 * @return true if thread is joined or detached before
 * delete call, otherwise return false
 */
XCL_PUBLIC bool XCL_API
Thread_delete(CThread* thread);

/**
 * start a thread if thread is suspended
 * @param thread thread object
 * @return 0 if successfully, otherwise error code
 */
XCL_PUBLIC int32_t XCL_API
Thread_start(CThread* thread);

/**
 * handle if thread is still alive
 * @param thread thread object
 * @return true if thread is created and not terminated
 *
 */
XCL_PUBLIC bool XCL_API
Thread_isAlive(CThread* thread);

/**
 * wait util thread is terminated
 * @param thread thread object
 */
XCL_PUBLIC void XCL_API
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
XCL_PUBLIC int32_t XCL_API
Thread_join2(CThread* thread, int32_t timeout, bool* terminated);

/**
 * detach thread
 * @param thread thread object
 */
XCL_PUBLIC void XCL_API
Thread_detach(CThread* thread);

#ifdef __cplusplus
}
#endif
