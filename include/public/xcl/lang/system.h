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
XCL_EXPORT(int64_t)
currentMillis();

/**
 * get current ticks but in nano period
 * @return ticks for nano period
 */
XCL_EXPORT(int64_t)
nanos();

/**
 * sleep current thread during timeout milliseconds
 * @param timeout milliseconds timeout
 */
XCL_EXPORT(void)
sleepMillis(int32_t timeout);

/**
 * get current thread error code
 * usually set from system-level api
 * however, if api no return value
 * and some errors has been occurred, you
 * can use error() to check
 * @return err code
 */
XCL_EXPORT(unsigned)
error();

/**
 * set global error code
 * @param errorCode error code
 */
XCL_EXPORT(void)
setErr(unsigned errorCode);

#ifdef __cplusplus
}
#endif
