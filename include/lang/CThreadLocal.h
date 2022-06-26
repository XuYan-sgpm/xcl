//
// Created by xuyan on 2022/6/20.
//

#pragma once

#include <lang/platform.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int64_t id;
} CThreadLocal;

CThreadLocal makeLocal();
bool setLocal(CThreadLocal *local, void *ptr);
bool setLocalChar(CThreadLocal *local, char val);
bool setLocalU8(CThreadLocal *local, unsigned char val);
bool setLocalInt32(CThreadLocal *local, int32_t val);
bool setLocalU32(CThreadLocal *local, uint32_t val);
bool setLocalInt16(CThreadLocal *local, int16_t val);
bool setLocalU16(CThreadLocal *local, uint16_t val);
bool setLocalFloat(CThreadLocal *local, float val);
bool setLocalInt64(CThreadLocal *local, int64_t val);
bool setLocalU64(CThreadLocal *local, uint64_t val);
bool setLocalDouble(CThreadLocal *local, double val);
bool getLocal(CThreadLocal *local, void **result);
bool getLocalChar(CThreadLocal *local, char *result);
bool getLocalU8(CThreadLocal *local, unsigned char *result);
bool getLocalInt16(CThreadLocal *local, int16_t *result);
bool getLocalU16(CThreadLocal *local, uint16_t *result);
bool getLocalInt32(CThreadLocal *local, int32_t *result);
bool getLocalU32(CThreadLocal *local, uint32_t *result);
bool getLocalFloat(CThreadLocal *local, float *result);
bool getLocalInt64(CThreadLocal *local, int64_t *result);
bool getLocalU64(CThreadLocal *local, uint64_t *result);
bool getLocalDouble(CThreadLocal *local, double *result);
void freeLocal(CThreadLocal *local);

#ifdef STATIC
void recycleUnusedThreadLocals();
#endif

#ifdef __cplusplus
}
#endif
