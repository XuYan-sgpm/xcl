//
// Created by xuyan on 2022/7/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <xcl/lang/XclDef.h>

XCL_EXPORT uint32_t XCL_API
_log2(uint64_t val);

XCL_EXPORT char* XCL_API
strDup(const char* str);

#ifdef __cplusplus
}
#endif
