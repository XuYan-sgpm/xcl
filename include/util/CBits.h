//
// Created by xuyan on 2022/6/29.
//

#pragma once

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
} CBitsUsr;

#ifdef __cplusplus
extern "C" {
#endif

CBits *Bits_new(int32_t size);
void Bits_delete(CBits *bits);
int32_t Bits_len(CBits *bits);
void Bits_set(CBits *bits, int32_t idx, bool on);
bool Bits_get(CBits *bits, int32_t idx);

CBits64 bits64();
CBits8 bits8();
CBits16 bits16();
CBits32 bits32();

#ifdef __cplusplus
}
#endif
