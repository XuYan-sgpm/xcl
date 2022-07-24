//
// Created by xuyan on 2022/7/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "xcl/lang/XclDef.h"
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
XCL_PUBLIC(CThread)
Thread_create(void (*proc)(void*), void* usr);

/**
 * check whether thread object returned by Thread_create is valid
 * @param thread thread object
 * @return true if thread object is valid, otherwise false
 */
XCL_PUBLIC(bool)
Thread_valid(CThread thread);

/**
 * wait util thread is terminated
 * @param thread thread object
 * @return true if join thread successfully, otherwise false
 */
XCL_PUBLIC(bool)
Thread_join(CThread* thread);

/**
 * wait thread util terminated or timeout expired
 * @param thread thread object
 * @param timeout wait timeout milliseconds
 * @return true if function succeed and thread is terminated,
 * otherwise false
 */
XCL_PUBLIC(bool)
Thread_join2(CThread* thread, int32_t timeout);

/**
 * check whether thread is still alive
 * @param thread thread object
 * @return true if thread is alive, otherwise false
 */
XCL_PUBLIC(bool)
Thread_alive(CThread* thread);

/**
 * detach thread
 * @param thread thread object
 * @return true if detach operation succeed, otherwise false
 */
XCL_PUBLIC(bool)
Thread_detach(CThread* thread);

/**
 * get current thread object
 * if thread is created by system call, thread object returned
 * will be invalid
 * @return current running thread object
 */
XCL_PUBLIC(CThread)
Thread_current();

/**
 * get current thread id
 * @return thread id
 */
XCL_PUBLIC(unsigned long)
Thread_currentId();

/**
 * get current thread handle
 * @return thread handle
 */
XCL_PUBLIC(uintptr_t)
Thread_currentHandle();

#ifdef __cplusplus
}
#endif
