//
// Created by xuyan on 2022/7/26.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <xcl/lang/XclDef.h>

#if CLANG || GNUC
#include <stdatomic.h>
#if GNUC
#define ATOMIC(type) _Atomic type
#else
#define ATOMIC(type) _Atomic(type)
#endif
#define ATOMIC_INCREMENT(atomicObjectPtr, memoryOrder) \
    atomic_fetch_add_explicit(atomicObjectPtr, 1, memoryOrder)
#define ATOMIC_LOAD(atomicObjectPtr, memoryOrder) \
    atomic_load_explicit(atomicObjectPtr, memoryOrder)
#elif defined(_MSC_VER)
#include <windows.h>
#define ATOMIC(type) volatile type
#define ATOMIC_INCREMENT(atomicObjectPtr, memoryOrder) \
    InterlockedIncrement(atomicObjectPtr)
#define ATOMIC_LOAD(atomicObjectPtr, memoryOrder) (*atomicObjectPtr)
#endif

#ifdef __cplusplus
}
#endif
