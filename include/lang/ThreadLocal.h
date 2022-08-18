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

#ifdef __cplusplus
extern "C" {
#endif

#include "xcldef.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t id;
} CThreadLocal;

/**
 * create a thread local object
 * @return thread local object
 */
XCL_EXPORT CThreadLocal XCL_API
Local_make();

/**
 * set pointer to thread local object
 * @param local thread local object
 * @param ptr pointer to store data
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_set(CThreadLocal* local, void* ptr);

/**
 * set char to thread local object
 * @param local thread local object
 * @param val character set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setChar(CThreadLocal* local, char val);

/**
 * set byte to thread local
 * @param local thread local object
 * @param val byte set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU8(CThreadLocal* local, unsigned char val);

/**
 * set int to thread local
 * @param local thread local object
 * @param val int set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setInt32(CThreadLocal* local, int32_t val);

/**
 * set unsigned to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU32(CThreadLocal* local, uint32_t val);

/**
 * set short to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setInt16(CThreadLocal* local, int16_t val);

/**
 * set unsigned short to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU16(CThreadLocal* local, uint16_t val);

/**
 * set float to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setFloat(CThreadLocal* local, float val);

/**
 * set 64 bits int to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setInt64(CThreadLocal* local, int64_t val);

/**
 * set 64 bits unsigned to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU64(CThreadLocal* local, uint64_t val);

/**
 * set double to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setDouble(CThreadLocal* local, double val);

/**
 * get ptr from thread local
 * @param local thread local object
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_get(CThreadLocal* local, void** result);

/**
 * get character from thread local
 * @param local thread local object
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getChar(CThreadLocal* local, char* result);

/**
 * get byte from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU8(CThreadLocal* local, unsigned char* result);

/**
 * get short from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getInt16(CThreadLocal* local, int16_t* result);

/**
 * get unsigned short from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU16(CThreadLocal* local, uint16_t* result);

/**
 * get int from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getInt32(CThreadLocal* local, int32_t* result);

/**
 * get unsigned from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU32(CThreadLocal* local, uint32_t* result);

/**
 * get float from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getFloat(CThreadLocal* local, float* result);

/**
 * get long from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getInt64(CThreadLocal* local, int64_t* result);

/**
 * get unsigned long from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU64(CThreadLocal* local, uint64_t* result);

/**
 * get double from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getDouble(CThreadLocal* local, double* result);

/**
 * discard a thread local, thread local object is not available
 * @param local thread local object
 */
XCL_EXPORT void XCL_API
Local_discard(CThreadLocal* local);

#ifdef __cplusplus
}
#endif
