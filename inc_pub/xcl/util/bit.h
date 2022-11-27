//
// Created by xuyan on 2022/6/29.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "xcl/lang/xcl_def.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief set bit at position index of val to 0/1
 * @param val uint8 value
 * @param index position of bit
 * @param on true if set bit to 1, otherwise false
 * @return new uint8 value which bit at index is set
 */
XCL_EXPORT uint8_t XCL_API
Bit8_set(uint8_t val, int32_t index, bool on);

/**
 * @brief check whether bit at position index is 1
 * @param val uint8 value
 * @param index position of bit
 * @return true if bit at index is 1, otherwise false
 */
XCL_EXPORT bool XCL_API
Bit8_on(uint8_t val, int32_t index);

/**
 * @brief set bit at position index of val to 0/1
 * @param val uint16 value
 * @param index position of bit
 * @param on true if set bit to 1, otherwise false
 * @return new uint16 value which bit at index is set
 */
XCL_EXPORT uint16_t XCL_API
Bit16_set(uint16_t val, int32_t index, bool on);

/**
 * @brief check whether bit at position index is 1
 * @param val uint16 value
 * @param index position of bit
 * @return true if bit at index is 1, otherwise false
 */
XCL_EXPORT bool XCL_API
Bit16_on(uint16_t val, int32_t index);

/**
 * @brief set bit at position index of val to 0/1
 * @param val uint32 value
 * @param index position of bit
 * @param on true if set bit to 1, otherwise false
 * @return new uint32 value which bit at index is set
 */
XCL_EXPORT uint32_t XCL_API
Bit32_set(uint32_t val, int32_t index, bool on);

/**
 * @brief check whether bit at position index is 1
 * @param val uint32 value
 * @param index position of bit
 * @return true if bit at index is 1, otherwise false
 */
XCL_EXPORT bool XCL_API
Bit32_on(uint32_t val, int32_t index);

/**
 * @brief set bit at position index of val to 0/1
 * @param val uint64 value
 * @param index position of bit
 * @param on true if set bit to 1, otherwise false
 * @return new uint64 value which bit at index is set
 */
XCL_EXPORT uint64_t XCL_API
Bit64_set(uint64_t val, int32_t index, bool on);

/**
 * @brief check whether bit at position index is 1
 * @param val uint64 value
 * @param index position of bit
 * @return true if bit at index is 1, otherwise false
 */
XCL_EXPORT bool XCL_API
Bit64_on(uint64_t val, int32_t index);

/**
 * @brief set bit at position index start from address of obj to 0/1
 * @param obj object address
 * @param index position of bit
 * @param on true if set bit to 1, otherwise false
 */
XCL_EXPORT void XCL_API
Bit_set(void* obj, int32_t index, bool on);

/**
 * @brief check whether bit at position index start from obj is 1
 * @param obj object address
 * @param index position of bit
 * @return true if bit at index set to 1, otherwise false
 */
XCL_EXPORT bool XCL_API
Bit_on(const void* obj, int32_t index);

#ifdef __cplusplus
}
#endif
