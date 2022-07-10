//
// Created by xuyan on 2022/6/29.
//

#pragma once

#include "xcl/lang/XclDef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

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

XCL_PUBLIC CBits* XCL_API
Bits_new(int32_t size);

XCL_PUBLIC void XCL_API
Bits_delete(CBits* bits);

XCL_PUBLIC int32_t XCL_API
Bits_len(CBits* bits);

XCL_PUBLIC void XCL_API
Bits_set(CBits* bits, int32_t idx, bool on);

XCL_PUBLIC bool XCL_API
Bits_get(CBits* bits, int32_t idx);

XCL_PUBLIC unsigned char XCL_API
Bits_getByte(CBits* bits, int32_t idx);

XCL_PUBLIC CBits64 XCL_API
bits64();

XCL_PUBLIC CBits8 XCL_API
bits8();

XCL_PUBLIC CBits16 XCL_API
bits16();

XCL_PUBLIC CBits32 XCL_API
bits32();

#ifdef __cplusplus
}
#endif
