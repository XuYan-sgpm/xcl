//
// Created by xuyan on 2022/7/4.
//

#pragma once

#include <stdint.h>
#include "XclDef.h"

#ifdef __cplusplus
extern "C" {
#endif

const static int64_t NANOS_PER_SEC = 1000000000;

/**
 * get current ticks
 * @return current cpu ticks
 */
XCL_PUBLIC int64_t XCL_API
currentMillis();

/**
 * get current ticks but in nano period
 * @return ticks for nano period
 */
XCL_PUBLIC int64_t XCL_API
nanos();

/**
 * sleep current thread during timeout milliseconds
 * @param timeout milliseconds timeout
 */
XCL_PUBLIC void XCL_API
sleepMillis(int32_t timeout);

/**
 * get current thread error code
 * usually set from system-level api
 * however, if api no return value
 * and some errors has been occurred, you
 * can use error() to check
 * @return err code
 */
XCL_PUBLIC unsigned XCL_API
error();

/**
 * set global error code
 * @param errorCode error code
 */
XCL_PUBLIC void XCL_API
setErr(unsigned errorCode);

#ifdef __cplusplus
}
#endif
