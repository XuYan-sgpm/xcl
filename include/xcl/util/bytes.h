//
// Created by xuyan on 2022/6/21.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <xcl/lang/define.h>

/**
 * @brief translate byte sequence to hex string
 * @param src source byte sequence
 * @param len byte sequence length
 * @param dst destination hex buffer
 * @return hex length
 */
int byte2hex(const unsigned char* src, int32_t len, char* dst);

/**
 * @brief translate hex string to byte buffer
 * @param src source hex string
 * @param len hex string length
 * @param dst dest byte buffer
 * @return byte length
 */
int hex2byte(const char* src, int32_t len, unsigned char* dst);

/**
 * @brief set bit(0/1) of position idx
 * @param val 8bit value
 * @param idx index
 * @param on 0/1
 * @return new value
 */
uint8_t Bit8_set(uint8_t val, uint8_t idx, bool on);

/**
 * @brief set bit of val at position idx
 * @param val 16bit value
 * @param idx index
 * @param on 0/1
 * @return new value
 */
uint16_t Bit16_set(uint16_t val, uint8_t idx, bool on);

/**
 * @brief set bit of val at position idx
 * @param val 32bit value
 * @param idx index
 * @param on 0/1
 * @return new value
 */
uint32_t Bit32_set(uint32_t val, uint8_t idx, bool on);

/**
 * @brief set bit of val at position idx
 * @param val 64bit value
 * @param idx index
 * @param on 0/1
 * @return new value
 */
uint64_t Bit64_set(uint64_t val, uint8_t idx, bool on);

/**
 * @brief check if bit at position idx of val is 1
 * @param val 8bit value
 * @param idx index
 * @return true if bit at idx is 1, otherwise false
 */
bool Bit8_isSet(uint8_t val, uint8_t idx);

/**
 * @brief check if bit at position idx of val is 1
 * @param val 16bit value
 * @param idx index
 * @return true if bit at idx is 1, otherwise false
 */
bool Bit16_isSet(uint16_t val, uint8_t idx);

/**
 * @brief check if bit at position idx of val is 1
 * @param val 32bit value
 * @param idx index
 * @return true if bit at idx is 1, otherwise false
 */
bool Bit32_isSet(uint32_t val, uint8_t idx);

/**
 * @brief check if bit at position idx of val is 1
 * @param val 64bit value
 * @param idx index
 * @return true if bit at idx is 1, otherwise false
 */
bool Bit64_isSet(uint64_t val, uint8_t idx);

/**
 * @brief serialize integer to bytes
 * @param val integer value
 * @param buf buffer hold bytes
 * @param big is big endian
 */
void int2byte(int val, void* buf, bool big);

/**
 * @brief serialize bytes to integer
 * @param buf byte buffer
 * @param big big endian
 * @return integer value
 */
int byte2int(const void* buf, bool big);

/**
 * @brief serialize short to bytes
 * @param val short value
 * @param buf byte buffer
 * @param big big endian
 */
void short2byte(short val, void* buf, bool big);

/**
 * @brief serialize byte buffer to short
 * @param buf byte buffer
 * @param big big endian
 * @return short value
 */
short byte2short(const void* buf, bool big);

/**
 * @brief serialize float value to bytes
 * @param val float value
 * @param buf byte buffer
 * @param big big endian
 */
void float2byte(float val, void* buf, bool big);

/**
 * @brief serialize byte buffer to float
 * @param buf byte buffer
 * @param big big endian
 * @return float value
 */
float byte2float(const void* buf, bool big);

/**
 * @brief serialize 64bit value to bytes
 * @param val long long value
 * @param buf byte buffer
 * @param big big endian
 */
void long2byte(int64_t val, void* buf, bool big);

/**
 * @brief serialize byte buffer to long long value
 * @param buf byte buffer
 * @param big big endian
 * @return long long value
 */
int64_t byte2long(const void* buf, bool big);

/**
 * @brief serialize double to bytes
 * @param val double value
 * @param buf byte buffer
 * @param big big endian
 */
void double2byte(double val, void* buf, bool big);

/**
 * @brief serialize byte buffer to double value
 * @param buf byte buffer
 * @param big big endian
 * @return double value
 */
double byte2double(const void* buf, bool big);

#ifdef __cplusplus
}
#endif
