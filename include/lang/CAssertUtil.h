//
// Created by xuyan on 2022/6/30.
//

#pragma once

#include <stdbool.h>
#include <lang/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

XCL_PUBLIC void XCL_API assertIf(bool cond, const char *msg);
XCL_PUBLIC void assertMessage(bool cond, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
