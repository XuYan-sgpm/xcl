//
// Created by xuyan on 2022/6/21.
//

#include "lang/Bytes.h"
#include <string.h>

static bool __isCPUBigEndian() {
  union {
    short val;
    char data[2];
  } unit;
  unit.val = 0x0001;
  return unit.data[1] == 1;
}

static void __translate(const void *src, void *dst, int size, bool big) {
  if (big == __isCPUBigEndian()) {
    memcpy(dst, src, size);
  } else {
    for (int i = 0; i < size; ++i) {
      ((char *)dst)[i] = ((const char *)src)[size - i - 1];
    }
  }
}

void int2Bytes(int val, void *dst, bool big) {
  __translate(&val, dst, sizeof(int), big);
}

int bytes2Int(const void *src, bool big) {
  int result;
  __translate(src, &result, sizeof(int), big);
  return result;
}

void LongLong2Bytes(int64_t val, void *dst, bool big) {
  __translate(&val, dst, 8, big);
}

int64_t bytes2LongLong(const void *src, bool big) {
  int64_t result;
  __translate(src, &result, 8, big);
  return result;
}
