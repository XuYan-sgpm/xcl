/**
 * @author xuyan
 * @brief atomic implementation only available for pure C invoke
 * @version 0.8
 * @date 2022-08-10
 */

#pragma once

#ifndef __cplusplus

#include <xcl/lang/XclDef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int64_t pair[2];
} __Int128;

#if !defined(__SIZEOF_INT128__) || defined(_MSC_VER)
#define __INT128__ __Int128
#else
#define __INT128__ __int128_t
#endif

#ifndef _MSC_VER
#define ALIGNED(x) __attribute__((aligned(x)))
#if CLANG
#define ATOMIC(type) _Atomic(type)
#else
#define ATOMIC(type) _Atomic type
#endif
#include <stdatomic.h>

#else
#define ALIGNED(x)   __declspec(align(x))
#define ATOMIC(type) volatile type

typedef enum
{
    memory_order_relaxed = 0xff,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;
#endif

/**
 * @brief atomic load char
 * @param m memory order
 * @return loaded character
 * @author xuyan
 * @date 2022-08-09
 */
char
__Atomic_load8(ATOMIC(char) * obj, memory_order m);

/**
 * @brief atomic load short
 * @param m memory order
 * @return loaded short value
 * @author xuyan
 * @date 2022-08-09
 */
short
__Atomic_load16(ATOMIC(short) * obj, memory_order m);

/**
 * @brief atomic load int32
 * @param m memory order
 * @return int
 * @author xuyan
 * @date 2022-08-09
 */
int32_t
__Atomic_load32(ATOMIC(int32_t) * obj, memory_order m);

/**
 * @brief atomic load int64
 * @param m memory order
 * @return int64
 * @author xuyan
 * @date 2022-08-09
 */
int64_t
__Atomic_load64(ATOMIC(int64_t) * obj, memory_order m);

#if X64
/**
 * @brief atomic load int128
 * @param m memory order
 * @return int128
 * @author xuyan
 * @date 2022-08-09
 */
__INT128__
__Atomic_load128(ATOMIC(__INT128__) * obj, memory_order m);
#endif

/**
 * @brief atomic store char
 * @param val stored char value
 * @param m memory order
 * @author xuyan
 * @date 2022-08-09
 */
void
__Atomic_store8(ATOMIC(char) * obj, char val, memory_order m);

/**
 * @brief atomic store short
 * @param val short value
 * @param m memory order
 * @author xuyan
 * @date 2022-08-09
 */
void
__Atomic_store16(ATOMIC(short) * obj, short val, memory_order m);

/**
 * @brief atomic store int32
 * @param val int32 value
 * @param m memory order
 * @author xuyan
 * @date 2022-08-09
 */
void
__Atomic_store32(ATOMIC(int32_t) * obj, int32_t val, memory_order m);

/**
 * @brief atomic store int64
 * @param val int64 value
 * @param m memory order
 * @author xuyan
 * @date 2022-08-09
 */
void
__Atomic_store64(ATOMIC(int64_t) * obj, int64_t val, memory_order m);

#if X64
/**
 * @brief atomic store int128
 * @param val int128 value
 * @param m memory order
 * @author xuyan
 * @date 2022-08-09
 */
void
__Atomic_store128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m);
#endif

/**
 * @brief atomic fetch original value and add delta
 * @param delta add delta
 * @param m memory order
 * @return original value
 * @author xuyan
 * @date 2022-08-09
 */
char
__Atomic_fetchAdd8(ATOMIC(char) * obj, char delta, memory_order m);

/**
 * @brief atomic fetch original value and add delta
 * @param delta add delta
 * @param m memory order
 * @return original short
 * @author xuyan
 * @date 2022-08-09
 */
short
__Atomic_fetchAdd16(ATOMIC(short) * obj, short delta, memory_order m);

/**
 * @brief atomic fetch original value and add int32
 * @param delta add delta
 * @param m memory order
 * @return original int32
 * @author xuyan
 * @date 2022-08-09
 */
int32_t
__Atomic_fetchAdd32(ATOMIC(int32_t) * obj, int32_t delta, memory_order m);

/**
 * @brief atomic fetch original int64 and add int64
 * @param delta add delta
 * @param m memory order
 * @return original int64
 * @author xuyan
 * @date 2022-08-09
 */
int64_t
__Atomic_fetchAdd64(ATOMIC(int64_t) * obj, int64_t delta, memory_order m);

#if X64
/**
 * @brief atomic fetch original int128 and add int128
 * @param delta add delta
 * @param m memory order
 * @return original int128
 * @author xuyan
 * @date 2022-08-09
 */
__INT128__
__Atomic_fetchAdd128(ATOMIC(__INT128__) * obj,
                     __INT128__ delta,
                     memory_order m);
#endif

/**
 * @brief atomic exchange char
 * @param val exchanged char
 * @param m memory order
 * @return original char
 * @author xuyan
 * @date 2022-08-09
 */
char
__Atomic_exchange8(ATOMIC(char) * obj, char val, memory_order m);

/**
 * @brief atomic exchange short
 * @param val exchanged short
 * @param m memory order
 * @return original short
 * @author xuyan
 * @date 2022-08-09
 */
short
__Atomic_exchange16(ATOMIC(short) * obj, short val, memory_order m);

/**
 * @brief atomic exchange int32
 * @param val exchanged int32
 * @param m memory order
 * @return original int32
 * @author xuyan
 * @date 2022-08-09
 */
int32_t
__Atomic_exchange32(ATOMIC(int32_t) * obj, int32_t val, memory_order m);

/**
 * @brief atomic exchange int64
 * @param val exchanged int64
 * @param m memory order
 * @return original int64
 * @author xuyan
 * @date 2022-08-09
 */
int64_t
__Atomic_exchange64(ATOMIC(int64_t) * obj, int64_t val, memory_order m);

#if X64
/**
 * @brief atomic int128
 * @param val exchanged int128
 * @param m memory order
 * @return original int128
 * @author xuyan
 * @date 2022-08-09
 */
__INT128__
__Atomic_exchange128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m);
#endif

#if X86
/**
 * @brief atomic cas char
 * note that cas char may not available for x64 platform
 * @param expect pointer store expected value
 * and will store actual value of atomic object
 * @param exchange exchanged char
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_cas8(ATOMIC(char) * obj, char* expect, char exchange, memory_order m);
#endif

/**
 * @brief atomic cas short
 * @param expect pointer store expected short value and
 * will store actual short value of atomic object
 * @param exchange exchanged short value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_cas16(ATOMIC(short) * obj,
               short* expect,
               short exchange,
               memory_order m);

/**
 * @brief atomic cas int32
 * @param expect pointer store expected int32 value and
 * will store actual int32 value of atomic object
 * @param exchange exchanged int32 value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_cas32(ATOMIC(int32_t) * obj,
               int32_t* expect,
               int32_t exchange,
               memory_order m);

/**
 * @brief atomic cas int64
 * @param expect pointer store expected int64 value and
 * will store actual int64 value of atomic object
 * @param exchange exchanged int64 value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_cas64(ATOMIC(int64_t) * obj,
               int64_t* expect,
               int64_t exchange,
               memory_order m);

#if X64
/**
 * @brief atomic cas int128
 * note that cas 128 bits variable is not available for x86 platform
 * @param expect pointer store expected int128 value and
 * will store actual int128 value of atomic object
 * @param exchange exchanged int128 value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_cas128(ATOMIC(__INT128__) * obj,
                __INT128__* expect,
                __INT128__ exchange,
                memory_order m);
#endif

#if X86
/**
 * @brief weak atomic cas char
 * note that cas char may not available for x64 platform
 * @param expect pointer store expected char value
 * and will store actual char value of atomic object
 * @param exchange exchanged char value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_weakCas8(ATOMIC(char) * obj,
                  char* expect,
                  char exchange,
                  memory_order m);
#endif

/**
 * @brief weak atomic cas short
 * @param expect pointer store expected short value
 * and will store actual short value of atomic object
 * @param exchange exchanged short value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_weakCas16(ATOMIC(short) * obj,
                   short* expect,
                   short exchange,
                   memory_order m);

/**
 * @brief weak atomic cas int32
 * @param expect pointer store expected int32 value
 * and will store actual int32 value of atomic object
 * @param exchange exchanged int32 value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_weakCas32(ATOMIC(int32_t) * obj,
                   int32_t* expect,
                   int32_t exchange,
                   memory_order m);

/**
 * @brief weak atomic cas int64
 * @param expect pointer store expected int64 value
 * and will store actual int64 value of atomic object
 * @param exchange exchanged int64 value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_weakCas64(ATOMIC(int64_t) * obj,
                   int64_t* expect,
                   int64_t exchange,
                   memory_order m);

#if X64
/**
 * @brief weak atomic cas int128
 * note that cas 128 bits variable is not available for x86 platform
 * @param expect pointer store expected int128 value
 * and will store actual int128 value of atomic object
 * @param exchange exchanged int128 value
 * @param m memory order
 * @return true if cas successfully, otherwise false
 * @author xuyan
 * @date 2022-08-09
 */
bool
__Atomic_weakCas128(ATOMIC(__INT128__) * obj,
                    __INT128__* expect,
                    __INT128__ exchange,
                    memory_order m);
#endif

#endif
