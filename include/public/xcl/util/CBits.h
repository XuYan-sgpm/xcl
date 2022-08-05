//
// Created by xuyan on 2022/6/29.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/XclDef.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const int32_t nBits;
} CBits;

typedef struct {
    CBits base;
    uint64_t val;
} CBits64;

typedef struct {
    CBits base;
    uint8_t val;
} CBits8;

typedef struct {
    CBits base;
    uint16_t val;
} CBits16;

typedef struct {
    CBits base;
    uint32_t val;
} CBits32;

typedef struct {
    CBits base;
    char ctx[0];
} CBitsSet;

XCL_EXPORT CBits* XCL_API
Bits_new(int32_t size);

/**
 * delete a bits object, must be created by Bits_new
 * @param bits bits object
 */
XCL_EXPORT void XCL_API
Bits_delete(CBits* bits);

XCL_EXPORT int32_t XCL_API
Bits_len(CBits* bits);

XCL_EXPORT void XCL_API
Bits_set(CBits* bits, int32_t idx, bool on);

XCL_EXPORT bool XCL_API
Bits_get(CBits* bits, int32_t idx);

XCL_EXPORT unsigned char XCL_API
Bits_getByte(CBits* bits, int32_t idx);

XCL_EXPORT CBits64 XCL_API
bits64();

XCL_EXPORT CBits8 XCL_API
bits8();

XCL_EXPORT CBits16 XCL_API
bits16();

XCL_EXPORT CBits32 XCL_API
bits32();

#ifdef __cplusplus
}
#endif
