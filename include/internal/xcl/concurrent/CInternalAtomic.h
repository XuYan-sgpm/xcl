//
// Created by xuyan on 2022/7/26.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/lang/XclDef.h>
#include <stdbool.h>

#if CLANG || GNUC
#include <stdatomic.h>
#if GNUC
#define ATOMIC(type) _Atomic type
#else
#define ATOMIC(type) _Atomic(type)
#endif
#define ATOMIC_INCREMENT(object, result, memoryOrder) \
    (result = atomic_fetch_add_explicit(object, 1, memoryOrder))
#define ATOMIC_LOAD(object, result, memoryOrder) \
    (result = atomic_load_explicit(object, memoryOrder))
#define ATOMIC_STORE(object, val, memoryOrder) \
    atomic_store_explicit(object, val, memoryOrder)
#define ATOMIC_DECREMENT(object, result, memoryOrder) \
    (result = atomic_fetch_sub_explicit(object, 1, memoryOrder))
#define CAS(object, expect, exchange, memoryOrder)       \
    atomic_compare_exchange_strong_explicit(object,      \
                                            expect,      \
                                            exchange,    \
                                            memoryOrder, \
                                            memoryOrder)
#define CAS_WEAK(object, expect, exchange, memoryOrder) \
    atomic_compare_exchange_weak_explicit(object,       \
                                          expect,       \
                                          exchange,     \
                                          memoryOrder,  \
                                          memoryOrder)
#define ATOMIC_ADD(object, delta, result, memoryOrder) \
    (result = atomic_fetch_add_explicit(object, delta, memoryOrder))
#define ATOMIC_EXCHANGE(object, val, result, memoryOrder) \
    (result = atomic_exchange_explicit(object, val, memoryOrder))

#elif defined(_MSC_VER)
#include <windows.h>

/*
 * msvc compiler atomic operation auto ignore memory order
 * some operation may not support 8-bits value
 */

bool
__Msvc_atomic_inc(volatile void* object, void* result, int size);

bool
__Msvc_atomic_dec(volatile void* object, void* result, int size);

bool
__Msvc_atomic_add(volatile void* object,
                  const void* value,
                  void* result,
                  int size);

bool
__Msvc_atomic_load(volatile void* object, void* result, int size);

bool
__Msvc_atomic_store(volatile void* object, const void* value, int size);

bool
__Msvc_atomic_exchange(volatile void* object,
                       const void* value,
                       void* result,
                       int size);

bool
__Msvc_atomic_compare_exchange(volatile void* object,
                               void* expect,
                               const void* exchange,
                               int size);

#define ATOMIC(type) volatile type
#define ATOMIC_INCREMENT(object, result, memoryOrder) \
    __Msvc_atomic_inc(object, &result, sizeof(*object))
#define ATOMIC_DECREMENT(object, result, memoryOrder) \
    __Msvc_atomic_dec(object, &result, sizeof(*object))
#define ATOMIC_LOAD(object, result, memoryOrder) \
    __Msvc_atomic_load(object, &result, sizeof(*object))
#define ATOMIC_STORE(object, val, memoryOrder) \
    __Msvc_atomic_store(object, &val, sizeof(*object))
#define ATOMIC_ADD(object, delta, result, memoryOrder) \
    __Msvc_atomic_add(object, &delta, &result, sizeof(*object))
#define ATOMIC_EXCHANGE(object, val, result, memoryOrder) \
    __Msvc_atomic_exchange(object, &val, &result, sizeof(*object))
#define CAS(object, expect, exchange, memoryOrder) \
    __Msvc_atomic_compare_exchange(object, &expect, &exchange, sizeof(*object))
#define CAS_WEAK(object, expect, exchange, memoryOrder) \
    CAS(object, expect, exchange, memoryOrder)

#endif

#ifdef __cplusplus
}
#endif
