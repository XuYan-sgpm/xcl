//
// Created by xuyan on 2022/7/4.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include <xcl/util/str.h>
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
inline bool XCL_API
isCpuBigEndian()
{
    union
    {
        short val;
        char data[2];
    } unit;
    unit.val = 0x0001;
    return unit.data[1] == 1;
}

/**
 * @brief get current working directory
 * @param cwd buffer store cwd info
 * @param len max size of cwd
 * @return true if get cwd successfully,
 * false if passing insufficient memory or syscall failed
 * @author xuyan
 * @date 2022-08-18
 */
XCL_EXPORT bool XCL_API
getCwd(TChar* cwd, int32_t len);

#ifdef __cplusplus
}
#endif
