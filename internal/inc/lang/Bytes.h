//
// Created by xuyan on 2022/6/21.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void short2Bytes(short val, void *dst, bool big);
short bytes2Short(const void *src, bool big);
void int2Bytes(int val, void *dst, bool big);
int bytes2Int(const void *src, bool big);
void float2Bytes(float val, void *dst, bool big);
float bytes2Float(const void *src, bool big);
void LongLong2Bytes(int64_t val, void *dst, bool big);
int64_t bytes2LongLong(const void *src, bool big);
void double2Bytes(double val, void *dst, bool big);
double bytes2Double(const void *src, bool big);

#ifdef __cplusplus
}
#endif
