//
// Created by xuyan on 2022/7/5.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <lang/XclDef.h>

/**
 * new condition variable
 * @return condition variable if successfully, otherwise NULL
 */
XCL_PUBLIC void XCL_API*
Cond_new();

/**
 * delete condition variable
 * @param cond condition variable
 */
XCL_PUBLIC void XCL_API
Cond_delete(void* cond);

/**
 * wait signal util Cond_signal called
 * @param cond condition variable
 */
XCL_PUBLIC bool XCL_API
Cond_wait(void* mutex, void* cond);

/**
 * wait signal on the condition during millis
 * @param cond condition variable
 * @param millis timeout milliseconds
 * @return true if condition variable signaled, otherwise false
 */
XCL_PUBLIC bool XCL_API
Cond_waitFor(void* mutex, void* cond, int32_t millis);

/**
 * signal random thread wait on cond
 * @param cond condition variable
 */
XCL_PUBLIC void XCL_API
Cond_signal(void* cond);

/**
 * signal all threads wait on cond
 * @param cond condition variable
 */
XCL_PUBLIC void XCL_API
Cond_signalAll(void* cond);

#ifdef __cplusplus
}
#endif
