//
// Created by xuyan on 2022/6/21.
//

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void int16ToBytes(short val, void *dst, bool big);
short bytes2Int16(const void *src, bool big);
void int32ToBytes(int val, void *dst, bool big);
int bytesToInt32(const void *src, bool big);
void floatToBytes(float val, void *dst, bool big);
float bytesToFloat(const void *src, bool big);
void int64ToBytes(int64_t val, void *dst, bool big);
int64_t bytesToInt64(const void *src, bool big);
void doubleToBytes(double val, void *dst, bool big);
double bytesToDouble(const void *src, bool big);
void bytesCopy(const void *src, void *dst, int len, bool big);

#ifdef __cplusplus
}
#endif
