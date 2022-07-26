//
// Created by xuyan on 2022/6/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "xcl/lang/XclDef.h"

/**
 * translate int16_t to bytes
 * @param val int16_t value
 * @param dst pointer store bytes
 * @param big big ending
 */
XCL_PUBLIC(void)
int16ToBytes(int16_t val, void* dst, bool big);

/**
 * translate 2 bytes to int16_t
 * @param src source bytes data
 * @param big big ending
 * @return int16_t value
 */
XCL_PUBLIC(int16_t)
bytes2Int16(const void* src, bool big);

/**
 * translate int32 to bytes
 * @param val int32_t value
 * @param dst pointer store bytes
 * @param big big endian
 */
XCL_PUBLIC(void)
int32ToBytes(int val, void* dst, bool big);

/**
 * translate 4 bytes to int32
 * @param src source bytes data
 * @param big big endian
 * @return int32 value
 */
XCL_PUBLIC(int32_t)
bytesToInt32(const void* src, bool big);

/**
 * translate float to 4 bytes
 * @param val float value
 * @param dst pointer store bytes
 * @param big big endian
 */
XCL_PUBLIC(void)
floatToBytes(float val, void* dst, bool big);

/**
 * translate 4 bytes to float
 * @param src source bytes data
 * @param big big endian
 * @return float value
 */
XCL_PUBLIC(float)
bytesToFloat(const void* src, bool big);

/**
 * int64 to 8 bytes
 * @param val int64 value
 * @param dst pointer store bytes
 * @param big big endian
 */
XCL_PUBLIC(void)
int64ToBytes(int64_t val, void* dst, bool big);

/**
 * translate 8 bytes to int64
 * @param src source bytes data
 * @param big big endian
 * @return int64 value
 */
XCL_PUBLIC(int64_t)
bytesToInt64(const void* src, bool big);

/**
 * translate double to 8 bytes
 * @param val double value
 * @param dst pointer store data
 * @param big big endian
 */
XCL_PUBLIC(void)
doubleToBytes(double val, void* dst, bool big);

/**
 * translate 8 bytes to double
 * @param src source bytes data
 * @param big big endian
 * @return double value
 */
XCL_PUBLIC(double)
bytesToDouble(const void* src, bool big);

/**
 * copy bytes in byte order
 * @param src source bytes data
 * @param dst pointer store data
 * @param len source bytes length
 * @param big big endian
 */
XCL_PUBLIC(void)
bytesCopy(const void* src, void* dst, int len, bool big);

#ifdef __cplusplus
}
#endif
