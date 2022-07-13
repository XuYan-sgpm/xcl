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

XCL_EXPORT(CBits*)
Bits_new(int32_t size);

XCL_EXPORT(void)
Bits_delete(CBits* bits);

XCL_EXPORT(int32_t)
Bits_len(CBits* bits);

XCL_EXPORT(void)
Bits_set(CBits* bits, int32_t idx, bool on);

XCL_EXPORT(bool)
Bits_get(CBits* bits, int32_t idx);

XCL_EXPORT(unsigned char)
Bits_getByte(CBits* bits, int32_t idx);

XCL_EXPORT(CBits64)
bits64();

XCL_EXPORT(CBits8)
bits8();

XCL_EXPORT(CBits16)
bits16();

XCL_EXPORT(CBits32)
bits32();

#ifdef __cplusplus
}
#endif
