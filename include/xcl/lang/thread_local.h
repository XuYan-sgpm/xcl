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

#include "xcl/lang/xcl_def.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t id;
} ThreadLocal;

/**
 * create a thread local object
 * @return thread local object
 */
XCL_EXPORT ThreadLocal XCL_API
Local_make();

/**
 * set pointer to thread local object
 * @param local thread local object
 * @param ptr pointer to store data
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_set(ThreadLocal* local, void* ptr);

/**
 * set char to thread local object
 * @param local thread local object
 * @param val character set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setChar(ThreadLocal* local, char val);

/**
 * set byte to thread local
 * @param local thread local object
 * @param val byte set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU8(ThreadLocal* local, unsigned char val);

/**
 * set int to thread local
 * @param local thread local object
 * @param val int set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setInt32(ThreadLocal* local, int32_t val);

/**
 * set unsigned to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU32(ThreadLocal* local, uint32_t val);

/**
 * set short to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setInt16(ThreadLocal* local, int16_t val);

/**
 * set unsigned short to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU16(ThreadLocal* local, uint16_t val);

/**
 * set float to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setFloat(ThreadLocal* local, float val);

/**
 * set 64 bits int to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setInt64(ThreadLocal* local, int64_t val);

/**
 * set 64 bits unsigned to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setU64(ThreadLocal* local, uint64_t val);

/**
 * set double to thread local
 * @param local thread local object
 * @param val data set to thread local
 * @return true if set successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_setDouble(ThreadLocal* local, double val);

/**
 * get ptr from thread local
 * @param local thread local object
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_get(ThreadLocal* local, void** result);

/**
 * get character from thread local
 * @param local thread local object
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getChar(ThreadLocal* local, char* result);

/**
 * get byte from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU8(ThreadLocal* local, unsigned char* result);

/**
 * get short from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getInt16(ThreadLocal* local, int16_t* result);

/**
 * get unsigned short from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU16(ThreadLocal* local, uint16_t* result);

/**
 * get int from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getInt32(ThreadLocal* local, int32_t* result);

/**
 * get unsigned from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU32(ThreadLocal* local, uint32_t* result);

/**
 * get float from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getFloat(ThreadLocal* local, float* result);

/**
 * get long from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getInt64(ThreadLocal* local, int64_t* result);

/**
 * get unsigned long from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getU64(ThreadLocal* local, uint64_t* result);

/**
 * get double from thread local
 * @param local thread local
 * @param result pointer store data
 * @return true if get successfully, otherwise false
 */
XCL_EXPORT bool XCL_API
Local_getDouble(ThreadLocal* local, double* result);

/**
 * discard a thread local, thread local object is not available
 * @param local thread local object
 */
XCL_EXPORT void XCL_API
Local_discard(ThreadLocal* local);

#ifdef __cplusplus
}
#endif
