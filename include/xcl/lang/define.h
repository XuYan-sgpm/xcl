//
// Created by xuyan on 2022/6/20.
//

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define _aligned(x) __declspec(align(x))
#define always_inline __forceinline
#define _deprecated(msg) __declspec(deprecated(msg))
#else
#define _aligned(x) __attribute__((aligned(x)))
#define always_inline __attribute__((always_inline))
#define _deprecated(msg) __attribute__((deprecated(msg)))
#endif

#if UINTPTR_MAX == UINT32_MAX
#define PTR_SIZE 4
#define PTR_BITS 32
#else
#define PTR_SIZE 8
#define PTR_BITS 64
#endif

#ifdef __cplusplus
}
#endif
