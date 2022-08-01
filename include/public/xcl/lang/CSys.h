//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "XclDef.h"
#include <stdint.h>
#include <stdbool.h>

const static int64_t NANOS_PER_SEC = 1000000000;

/**
 * get current ticks
 * @return current cpu ticks
 */
XCL_PUBLIC(int64_t)
currentMillis();

/**
 * get current ticks but in nano period
 * @return ticks for nano period
 */
XCL_PUBLIC(int64_t)
nanos();

/**
 * sleep current thread during timeout milliseconds
 * @param timeout milliseconds timeout
 */
XCL_PUBLIC(void)
mSleep(int32_t timeout);

/**
 * get if cpu byte order is big-endian
 * @return true if byte order of current cpu is big-endian, otherwise false
 */
XCL_PUBLIC(bool)
isCpuBigEndian();

#ifdef __cplusplus
}
#endif
