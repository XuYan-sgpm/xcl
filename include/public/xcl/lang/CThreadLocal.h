//
// Created by xuyan on 2022/6/20.
//

/**
 * note that thread local is not manage thread
 * local object data. for example, if you alloc memory,
 * and put address into thread local, you must free memory
 * manually, no any memory release operation when thread local
 * is not available or thread local data changed.
 */

#pragma once

#include "XclDef.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int64_t id;
} CThreadLocal;

XCL_PUBLIC CThreadLocal XCL_API
Local_make();
XCL_PUBLIC bool XCL_API
Local_set(CThreadLocal* local, void* ptr);
XCL_PUBLIC bool XCL_API
Local_setChar(CThreadLocal* local, char val);
XCL_PUBLIC bool XCL_API
Local_setU8(CThreadLocal* local, unsigned char val);
XCL_PUBLIC bool XCL_API
Local_setInt32(CThreadLocal* local, int32_t val);
XCL_PUBLIC bool XCL_API
Local_setU32(CThreadLocal* local, uint32_t val);
XCL_PUBLIC bool XCL_API
Local_setInt16(CThreadLocal* local, int16_t val);
XCL_PUBLIC bool XCL_API
Local_setU16(CThreadLocal* local, uint16_t val);
XCL_PUBLIC bool XCL_API
Local_setFloat(CThreadLocal* local, float val);
XCL_PUBLIC bool XCL_API
Local_setInt64(CThreadLocal* local, int64_t val);
XCL_PUBLIC bool XCL_API
Local_setU64(CThreadLocal* local, uint64_t val);
XCL_PUBLIC bool XCL_API
Local_setDouble(CThreadLocal* local, double val);
XCL_PUBLIC bool XCL_API
Local_get(CThreadLocal* local, void** result);
XCL_PUBLIC bool XCL_API
Local_getChar(CThreadLocal* local, char* result);
XCL_PUBLIC bool XCL_API
Local_getU8(CThreadLocal* local, unsigned char* result);
XCL_PUBLIC bool XCL_API
Local_getInt16(CThreadLocal* local, int16_t* result);
XCL_PUBLIC bool XCL_API
Local_getU16(CThreadLocal* local, uint16_t* result);
XCL_PUBLIC bool XCL_API
Local_getInt32(CThreadLocal* local, int32_t* result);
XCL_PUBLIC bool XCL_API
Local_getU32(CThreadLocal* local, uint32_t* result);
XCL_PUBLIC bool XCL_API
Local_getFloat(CThreadLocal* local, float* result);
XCL_PUBLIC bool XCL_API
Local_getInt64(CThreadLocal* local, int64_t* result);
XCL_PUBLIC bool XCL_API
Local_getU64(CThreadLocal* local, uint64_t* result);
XCL_PUBLIC bool XCL_API
Local_getDouble(CThreadLocal* local, double* result);

/**
 * discard a thread local, thread local object is not available
 * @param local thread local object
 */
XCL_PUBLIC void XCL_API
Local_discard(CThreadLocal* local);

#ifdef STATIC
XCL_PUBLIC void XCL_API
recycleUnusedThreadLocals();
#endif

#ifdef __cplusplus
}
#endif
