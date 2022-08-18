//
// Created by xuyan on 2022/7/14.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lang/xcldef.h>

#define XCL_ERR_BEGIN 0xffffff00
#define XCL_ERR(code) (XCL_ERR_BEGIN + (code))

#define XCL_NO_ERR              XCL_ERR(0)
#define XCL_MEMORY_ERR          XCL_ERR(1)
#define XCL_INSUFFICIENT_MEMORY XCL_ERR(2)
#define XCL_INVALID_PARAM       XCL_ERR(3)
#define XCL_LOCAL_INVALID_ID    XCL_ERR(4)
#define XCL_LOCAL_NO_STORAGE    XCL_ERR(5)

/**
 * get current thread Err_get code
 * usually set from system-level api
 * however, if api no return value
 * and some errors has been occurred, you
 * can use Err_get() to check
 * @return err code
 */
XCL_EXPORT unsigned XCL_API
Err_get();

/**
 * set global Err_get code
 * @param errorCode Err_get code
 */
XCL_EXPORT void XCL_API
Err_set(unsigned errorCode);

#ifdef __cplusplus
}
#endif
