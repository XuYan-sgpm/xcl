/**
 * @author xuyan
 * @brief cross-platform thread api implementation
 * @version 0.1
 * @date 2022-08-06
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lang/XclDef.h"
#include <stdbool.h>

typedef uintptr_t ThreadHandle;

#define INVALID_THREAD_HANDLE (0)

typedef struct {
    ThreadHandle handle;
} CThread;

/**
 * create a thread object
 * @param proc thread run proc
 * @param usr thread run args
 * @return thread object
 */
XCL_EXPORT CThread XCL_API
Thread_create(void (*proc)(void*), void* usr);

/**
 * check whether thread object returned by Thread_create is valid
 * @param thread thread object
 * @return true if thread object is valid, otherwise false
 */
XCL_EXPORT bool XCL_API
Thread_valid(CThread thread);

/**
 * wait util thread is terminated
 * @param thread thread object
 * @return true if join thread successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Thread_join(CThread* thread);

/**
 * wait thread util terminated or timeout expired
 * @param thread thread object
 * @param timeout wait timeout milliseconds
 * @return true if function succeed and thread is terminated,
 * otherwise false
 */
XCL_EXPORT bool XCL_API
Thread_join2(CThread* thread, int32_t timeout);

/**
 * check whether thread is still alive
 * @param thread thread object
 * @return true if thread is alive, otherwise false
 */
XCL_EXPORT bool XCL_API
Thread_alive(CThread* thread);

/**
 * detach thread
 * @param thread thread object
 * @return true if detach operation succeed, otherwise false
 */
XCL_EXPORT bool XCL_API
Thread_detach(CThread* thread);

/**
 * get current thread object
 * if thread is created by system call, thread object returned
 * will be invalid
 * @return current running thread object
 */
XCL_EXPORT CThread XCL_API
Thread_current();

/**
 * get current thread id
 * @return thread id
 */
XCL_EXPORT unsigned long XCL_API
Thread_currentId();

/**
 * get current thread handle
 * @return thread handle
 */
XCL_EXPORT uintptr_t XCL_API
Thread_currentHandle();

/**
 * @brief yield current thread
 * @author xuyan
 * @date 2022-08-04
 */
XCL_EXPORT void XCL_API
Thread_yield();

#ifdef __cplusplus
}
#endif
