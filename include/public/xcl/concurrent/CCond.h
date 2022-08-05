//
// Created by xuyan on 2022/7/5.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/XclDef.h"
#include "CMutex.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct _CCond_st CCond;

/**
 * new condition variable
 * @return condition variable if successfully, otherwise NULL
 */
XCL_EXPORT CCond* XCL_API
Cond_new();

/**
 * delete condition variable
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_delete(CCond* cond);

/**
 * wait signal util Cond_signal called
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_wait(CMutex* mutex, CCond* cond);

/**
 * wait signal on the condition during millis
 * this function may returned even if no signal
 * send to current thread
 * @param cond condition variable
 * @param millis timeout milliseconds
 * @return true if condition variable signaled, otherwise false
 */
XCL_EXPORT bool XCL_API
Cond_waitFor(CMutex* mutex, CCond* cond, int32_t millis);

/**
 * signal random thread wait on cond
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_signal(CCond* cond);

/**
 * signal all threads wait on cond
 * @param cond condition variable
 */
XCL_EXPORT bool XCL_API
Cond_signalAll(CCond* cond);

#ifdef __cplusplus
}
#endif
