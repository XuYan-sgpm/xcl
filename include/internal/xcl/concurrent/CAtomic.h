//
// Created by xuyan on 2022/7/26.
//

#pragma once

#ifndef __cplusplus

#include <xcl/lang/XclDef.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(_MSC_VER)
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
#elif CLANG || GNUC
#define ALIGNED(x) __attribute__((aligned(x)))
#include <stdatomic.h>
#if GNUC
#define ATOMIC(type) _Atomic type
#else
#define ATOMIC(type) _Atomic(type)
#endif
#endif

#endif
