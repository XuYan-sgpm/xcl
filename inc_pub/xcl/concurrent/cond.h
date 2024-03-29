//
// Created by xuyan on 2022/7/5.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include "xcl/concurrent/mutex.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Cond Cond;

/**
 * new condition variable
 * @return condition variable if successfully, otherwise NULL
 */
XCL_EXPORT Cond* XCL_API
Cond_new();

/**
 * delete condition variable
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_delete(Cond* cond);

/**
 * wait signal util Cond_signal called
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_wait(Mutex* mutex, Cond* cond);

/**
 * wait signal on the condition during millis
 * this function may returned even if no signal
 * send to current thread
 * @param cond condition variable
 * @param millis timeout milliseconds
 * @return true if condition variable signaled, otherwise false
 */
XCL_EXPORT bool XCL_API
Cond_waitFor(Mutex* mutex, Cond* cond, int32_t millis);

/**
 * signal random thread wait on cond
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_signal(Cond* cond);

/**
 * signal all threads wait on cond
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_signalAll(Cond* cond);

#ifdef __cplusplus
}
#endif
