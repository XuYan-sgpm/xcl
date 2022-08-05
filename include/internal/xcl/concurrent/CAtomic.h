//
// Created by xuyan on 2022/7/26.
//

#pragma once

#ifndef __cplusplus

#include <xcl/lang/XclDef.h>
#include <stdbool.h>
#include <stdint.h>

#if CLANG || GNUC
#define ALIGNED(x) __attribute__((aligned(x)))
#if GNUC
#define ATOMIC(type) _Atomic type
#else
#define ATOMIC(type) _Atomic(type)
#endif

#elif defined(_MSC_VER)
#define ALIGNED(x)   __declspec(align(x))
#define ATOMIC(type) volatile type
#endif

#endif
