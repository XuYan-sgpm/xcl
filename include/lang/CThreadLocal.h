//
// Created by xuyan on 2022/6/20.
//

#pragma once

#include <lang/XclDef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int64_t id;
} CThreadLocal;

CThreadLocal Local_make();
bool Local_set(CThreadLocal* local, void* ptr);
bool Local_setChar(CThreadLocal* local, char val);
bool Local_setU8(CThreadLocal* local, unsigned char val);
bool Local_setInt32(CThreadLocal* local, int32_t val);
bool Local_setU32(CThreadLocal* local, uint32_t val);
bool Local_setInt16(CThreadLocal* local, int16_t val);
bool Local_setU16(CThreadLocal* local, uint16_t val);
bool Local_setFloat(CThreadLocal* local, float val);
bool Local_setInt64(CThreadLocal* local, int64_t val);
bool Local_setU64(CThreadLocal* local, uint64_t val);
bool Local_setDouble(CThreadLocal* local, double val);
bool Local_get(CThreadLocal* local, void** result);
bool Local_getChar(CThreadLocal* local, char* result);
bool Local_getU8(CThreadLocal* local, unsigned char* result);
bool Local_getInt16(CThreadLocal* local, int16_t* result);
bool Local_getU16(CThreadLocal* local, uint16_t* result);
bool Local_getInt32(CThreadLocal* local, int32_t* result);
bool Local_getU32(CThreadLocal* local, uint32_t* result);
bool Local_getFloat(CThreadLocal* local, float* result);
bool Local_getInt64(CThreadLocal* local, int64_t* result);
bool Local_getU64(CThreadLocal* local, uint64_t* result);
bool Local_getDouble(CThreadLocal* local, double* result);
void Local_discard(CThreadLocal* local);

#ifdef STATIC
void recycleUnusedThreadLocals();
#endif

#ifdef __cplusplus
}
#endif
