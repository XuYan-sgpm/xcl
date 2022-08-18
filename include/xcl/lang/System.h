//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/XclDef.h"
#include <stdint.h>
#include <stdbool.h>

const static int64_t NANOS_PER_SEC = 1000000000;

/**
 * get current ticks
 * @return current cpu ticks
 */
XCL_EXPORT int64_t XCL_API
currentMillis();

/**
 * get current ticks but in nano period
 * @return ticks for nano period
 */
XCL_EXPORT int64_t XCL_API
nanos();

/**
 * sleep current thread during timeout milliseconds
 * @param timeout milliseconds timeout
 */
XCL_EXPORT void XCL_API
mSleep(int32_t timeout);

/**
 * get if cpu byte order is big-endian
 * @return true if byte order of current cpu is big-endian, otherwise false
 */
XCL_EXPORT bool XCL_API
isCpuBigEndian();

#ifdef __cplusplus
}
#endif
