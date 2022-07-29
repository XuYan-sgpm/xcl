//
// Created by xuyan on 2022/7/26.
//

#pragma once

#ifndef __cplusplus

#include <xcl/lang/XclDef.h>
#include <stdbool.h>

#if CLANG || GNUC
#define ALIGNED(x) __attribute__((aligned(x)))
#include <stdatomic.h>
#if GNUC
#define ATOMIC(type) _Atomic type
#else
#define ATOMIC(type) _Atomic(type)
#endif
#elif defined(_MSC_VER)
#define ALIGNED(x)   __declspec(align(x))
#define ATOMIC(type) volatile type
typedef enum
{
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
} memory_order;
#endif

bool
__XCL_atomic_inc(void* object,
                 void* result,
                 int size,
                 memory_order memoryOrder);

bool
__XCL_atomic_dec(void* object,
                 void* result,
                 int size,
                 memory_order memoryOrder);

bool
__XCL_atomic_add(void* object,
                 const void* value,
                 void* result,
                 int size,
                 memory_order memoryOrder);

bool
__XCL_atomic_load(void* object,
                  void* result,
                  int size,
                  memory_order memoryOrder);

bool
__XCL_atomic_store(void* object,
                   const void* value,
                   int size,
                   memory_order memoryOrder);

bool
__XCL_atomic_exchange(void* object,
                      const void* value,
                      void* result,
                      int size,
                      memory_order memoryOrder);

bool
__XCL_atomic_cas(void* object,
                 void* expect,
                 const void* exchange,
                 int size,
                 memory_order memoryOrder);

bool
__XCL_atomic_casWeak(void* object,
                     void* expect,
                     const void* exchange,
                     int size,
                     memory_order memoryOrder);

#define ATOMIC_INCREMENT(object, result, memoryOrder) \
    __XCL_atomic_inc(object, &result, sizeof(*object), memoryOrder)
#define ATOMIC_DECREMENT(object, result, memoryOrder) \
    __XCL_atomic_dec(object, &result, sizeof(*object), memoryOrder)
#define ATOMIC_LOAD(object, result, memoryOrder) \
    __XCL_atomic_load(object, &result, sizeof(*object), memoryOrder)
#define ATOMIC_STORE(object, val, memoryOrder) \
    __XCL_atomic_store(object, &val, sizeof(*object), memoryOrder)
#define ATOMIC_ADD(object, delta, result, memoryOrder) \
    __XCL_atomic_add(object, &delta, &result, sizeof(*object), memoryOrder)
#define ATOMIC_EXCHANGE(object, val, result, memoryOrder) \
    __XCL_atomic_exchange(object, &val, &result, sizeof(*object), memoryOrder)
#define CAS(object, expect, exchange, memoryOrder) \
    __XCL_atomic_cas(object, &expect, &exchange, sizeof(*object), memoryOrder)
#define CAS_WEAK(object, expect, exchange, memoryOrder) \
    __XCL_atomic_casWeak(object,                        \
                         &expect,                       \
                         &exchange,                     \
                         sizeof(*object),               \
                         memoryOrder)

#endif
