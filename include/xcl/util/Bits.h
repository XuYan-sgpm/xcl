//
// Created by xuyan on 2022/6/29.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    const int32_t n_bits;
} Bits;

typedef struct {
    Bits base;
    uint64_t val;
} Bits64;

typedef struct {
    Bits base;
    uint8_t val;
} Bits8;

typedef struct {
    Bits base;
    uint16_t val;
} Bits16;

typedef struct {
    Bits base;
    uint32_t val;
} Bits32;

typedef struct {
    Bits base;
    char ctx[0];
} BitsSet;

XCL_EXPORT Bits* XCL_API
Bits_new(int32_t size);

/**
 * delete a bits object, must be created by Bits_new
 * @param bits bits object
 */
XCL_EXPORT void XCL_API
Bits_delete(Bits* bits);

XCL_EXPORT int32_t XCL_API
Bits_len(Bits* bits);

XCL_EXPORT void XCL_API
Bits_set(Bits* bits, int32_t idx, bool on);

XCL_EXPORT bool XCL_API
Bits_get(Bits* bits, int32_t idx);

XCL_EXPORT unsigned char XCL_API
Bits_getByte(Bits* bits, int32_t idx);

XCL_EXPORT Bits64 XCL_API
bits64();

XCL_EXPORT Bits8 XCL_API
bits8();

XCL_EXPORT Bits16 XCL_API
bits16();

XCL_EXPORT Bits32 XCL_API
bits32();

#ifdef __cplusplus
}
#endif
