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
sleepMillis(int32_t timeout);

#ifdef __cplusplus
}
#endif
