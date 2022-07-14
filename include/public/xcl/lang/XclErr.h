//
// Created by xuyan on 2022/7/14.
//

#pragma once

#include <xcl/lang/XclDef.h>

#define XCL_ERR_BEGIN        0xffffff00
#define XCL_ERR(code)        (XCL_ERR_BEGIN + code)

#define XCL_NO_ERR           XCL_ERR(0)
#define XCL_MEMORY_ERR       XCL_ERR(1)
#define XCL_INVALID_PARAM    XCL_ERR(2)
#define XCL_LOCAL_INVALID_ID XCL_ERR(3)
#define XCL_LOCAL_NO_STORAGE XCL_ERR(4)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * get current thread error code
 * usually set from system-level api
 * however, if api no return value
 * and some errors has been occurred, you
 * can use error() to check
 * @return err code
 */
XCL_PUBLIC(unsigned)
error();

/**
 * set global error code
 * @param errorCode error code
 */
XCL_PUBLIC(void)
setErr(unsigned errorCode);

#ifdef __cplusplus
}
#endif
