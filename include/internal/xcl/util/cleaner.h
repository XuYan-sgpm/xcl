//
// Created by xuyan on 2022/7/8.
//

#pragma once

#include "xcl/lang/XclDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

XCL_PUBLIC bool XCL_API
__internal_registerCleanProc(void (*proc)(void*), void* usr);

#ifdef __cplusplus
}
#endif
