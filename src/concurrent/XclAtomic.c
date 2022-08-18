#include "concurrent/XclAtomic.h"

#ifndef _MSC_VER
char
__Atomic_load8(ATOMIC(char) * obj, memory_order m)
{
    return atomic_load_explicit(obj, m);
}

short
__Atomic_load16(ATOMIC(short) * obj, memory_order m)
{
    return atomic_load_explicit(obj, m);
}

int32_t
__Atomic_load32(ATOMIC(int32_t) * obj, memory_order m)
{
    return atomic_load_explicit(obj, m);
}

int64_t
__Atomic_load64(ATOMIC(int64_t) * obj, memory_order m)
{
    return atomic_load_explicit(obj, m);
}

void
__Atomic_store8(ATOMIC(char) * obj, char val, memory_order m)
{
    atomic_store_explicit(obj, val, m);
}

void
__Atomic_store16(ATOMIC(short) * obj, short val, memory_order m)
{
    atomic_store_explicit(obj, val, m);
}

void
__Atomic_store32(ATOMIC(int32_t) * obj, int32_t val, memory_order m)
{
    atomic_store_explicit(obj, val, m);
}

void
__Atomic_store64(ATOMIC(int64_t) * obj, int64_t val, memory_order m)
{
    atomic_store_explicit(obj, val, m);
}

char
__Atomic_fetchAdd8(ATOMIC(char) * obj, char delta, memory_order m)
{
    return atomic_fetch_add_explicit(obj, delta, m);
}

short
__Atomic_fetchAdd16(ATOMIC(short) * obj, short delta, memory_order m)
{
    return atomic_fetch_add_explicit(obj, delta, m);
}

int32_t
__Atomic_fetchAdd32(ATOMIC(int32_t) * obj, int32_t delta, memory_order m)
{
    return atomic_fetch_add_explicit(obj, delta, m);
}

int64_t
__Atomic_fetchAdd64(ATOMIC(int64_t) * obj, int64_t delta, memory_order m)
{
    return atomic_fetch_add_explicit(obj, delta, m);
}

char
__Atomic_exchange8(ATOMIC(char) * obj, char val, memory_order m)
{
    return atomic_exchange_explicit(obj, val, m);
}

short
__Atomic_exchange16(ATOMIC(short) * obj, short val, memory_order m)
{
    return atomic_exchange_explicit(obj, val, m);
}

int32_t
__Atomic_exchange32(ATOMIC(int32_t) * obj, int32_t val, memory_order m)
{
    return atomic_exchange_explicit(obj, val, m);
}

int64_t
__Atomic_exchange64(ATOMIC(int64_t) * obj, int64_t val, memory_order m)
{
    return atomic_exchange_explicit(obj, val, m);
}

bool
__Atomic_cas8(ATOMIC(char) * obj, char* expect, char exchange, memory_order m)
{
    char origin = *expect;
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_cas16(ATOMIC(short) * obj,
               short* expect,
               short exchange,
               memory_order m)
{
    short origin = *expect;
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_cas32(ATOMIC(int32_t) * obj,
               int32_t* expect,
               int32_t exchange,
               memory_order m)
{
    int32_t origin = *expect;
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_cas64(ATOMIC(int64_t) * obj,
               int64_t* expect,
               int64_t exchange,
               memory_order m)
{
    int64_t origin = *expect;
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_weakCas8(ATOMIC(char) * obj,
                  char* expect,
                  char exchange,
                  memory_order m)
{
    char origin = *expect;
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_weakCas16(ATOMIC(short) * obj,
                   short* expect,
                   short exchange,
                   memory_order m)
{
    short origin = *expect;
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_weakCas32(ATOMIC(int32_t) * obj,
                   int32_t* expect,
                   int32_t exchange,
                   memory_order m)
{
    int32_t origin = *expect;
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_weakCas64(ATOMIC(int64_t) * obj,
                   int64_t* expect,
                   int64_t exchange,
                   memory_order m)
{
    int64_t origin = *expect;
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m)
           == origin;
}

#if X64
__INT128__
__Atomic_load128(ATOMIC(__INT128__) * obj, memory_order m)
{
    return atomic_load_explicit(obj, m);
}

void
__Atomic_store128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m)
{
    atomic_store_explicit(obj, val, m);
}

__INT128__
__Atomic_fetchAdd128(ATOMIC(__INT128__) * obj, __INT128__ delta, memory_order m)
{
    return atomic_fetch_add_explicit(obj, delta, m);
}

__INT128__
__Atomic_exchange128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m)
{
    return atomic_exchange_explicit(obj, val, m);
}

bool
__Atomic_cas128(ATOMIC(__INT128__) * obj,
                __INT128__* expect,
                __INT128__ exchange,
                memory_order m)
{
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m);
}

bool
__Atomic_weakCas128(ATOMIC(__INT128__) * obj,
                    __INT128__* expect,
                    __INT128__ exchange,
                    memory_order m)
{
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m);
}
#endif
#else
#include <windows.h>

#define ENABLE_128_CAS (X64 && defined(InterlockedCompareExchange128))

char
__Atomic_load8(ATOMIC(char) * obj, memory_order m)
{
    return InterlockedOr8(obj, 0);
}

short
__Atomic_load16(ATOMIC(short) * obj, memory_order m)
{
    return InterlockedOr16(obj, 0);
}

int32_t
__Atomic_load32(ATOMIC(int32_t) * obj, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedOrNoFence(obj, 0);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedOrAcquire(obj, 0);
        case memory_order_release:
            return InterlockedOrRelease(obj, 0);
        default:
            return InterlockedOr(obj, 0);
    }
}

#ifdef InterlockedOr64
int64_t
__Atomic_load64(ATOMIC(int64_t) * obj, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
#ifdef InterlockedOr64NoFence
            return InterlockedOr64NoFence(obj, 0);
#endif
        case memory_order_consume:
        case memory_order_acquire:
#ifdef InterlockedOr64Acquire
            return InterlockedOr64Acquire(obj, 0);
#endif
        case memory_order_release:
#ifdef InterlockedOr64Release
            return InterlockedOr64Release(obj, 0);
#endif
        default:
            return InterlockedOr64(obj, 0);
    }
}
#endif

#if ENABLE_128_CAS
__INT128__
__Atomic_load128(ATOMIC(__INT128__) * obj, memory_order m)
{
    __INT128__ expect = *obj;
    __INT128__ exchange = expect;
    InterlockedCompareExchange128(obj->pair,
                                  exchange.pair[1],
                                  exchange.pair[0],
                                  expect.pair);
    return expect;
}
#endif

void
__Atomic_store8(ATOMIC(char) * obj, char val, memory_order m)
{
    InterlockedExchange8(obj, val);
}

void
__Atomic_store16(ATOMIC(short) * obj, short val, memory_order m)
{
    InterlockedExchange16(obj, val);
}

void
__Atomic_store32(ATOMIC(int32_t) * obj, int32_t val, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
#ifdef InterlockedExchangeNoFence
            InterlockedExchangeNoFence(obj, val);
            break;
#endif
        case memory_order_consume:
        case memory_order_acquire:
#ifdef InterlockedExchangeAcquire
            InterlockedExchangeAcquire(obj, val);
            break;
#endif
        case memory_order_release:
#ifdef InterlockedExchangeRelease
            InterlockedExchangeRelease(obj, val);
            break;
#endif
        default:
            InterlockedExchange(obj, val);
            break;
    }
}

#ifdef InterlockedExchange64
void
__Atomic_store64(ATOMIC(int64_t) * obj, int64_t val, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
#ifdef InterlockedExchangeNoFence64
            InterlockedExchangeNoFence64(obj, val);
            break;
#endif
        case memory_order_consume:
        case memory_order_acquire:
#ifdef InterlockedExchangeAcquire64
            InterlockedExchangeAcquire64(obj, val);
            break;
#endif
        case memory_order_release:
#ifdef InterlockedExchangeRelease64
            InterlockedExchangeRelease64(obj, val);
            break;
#endif
        default:
            InterlockedExchange64(obj, val);
            break;
    }
}
#endif

#if ENABLE_128_CAS
void
__Atomic_store128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m)
{
    __INT128__ expect = *obj;
    for (;;)
    {
        if (!InterlockedCompareExchange128(obj->pair,
                                           val.pair[1],
                                           val.pair[0],
                                           expect.pair))
        {
            continue;
        }
        else
        {
            break;
        }
    }
}
#endif

char
__Atomic_fetchAdd8(ATOMIC(char) * obj, char delta, memory_order m)
{
    return InterlockedExchangeAdd8(obj, delta);
}

#if defined(InterlockedExchangeAdd16) || defined(InterlockedCompareExchange16)
short
__Atomic_fetchAdd16(ATOMIC(short) * obj, short delta, memory_order m)
{
#ifdef InterlockedExchangeAdd16
    return InterlockedExchangeAdd16(obj, delta);
#else
#ifndef InterlockedCompareExchangeNoFence16
#define InterlockedCompareExchangeNoFence16 InterlockedCompareExchange16
#endif
#ifndef InterlockedCompareExchangeAcquire16
#define InterlockedCompareExchangeAcquire16 InterlockedCompareExchange16
#endif
#ifndef InterlockedCompareExchangeRelease16
#define InterlockedCompareExchangeRelease16 InterlockedCompareExchange16
#endif
    short original = *obj;
    switch (m)
    {
        case memory_order_relaxed: {
            for (;;)
            {
                short ret
                    = InterlockedCompareExchangeNoFence16(obj,
                                                          original + delta,
                                                          original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
        case memory_order_consume:
        case memory_order_acquire: {
            for (;;)
            {
                short ret
                    = InterlockedCompareExchangeAcquire16(obj,
                                                          original + delta,
                                                          original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
        case memory_order_release: {
            for (;;)
            {
                short ret
                    = InterlockedCompareExchangeRelease16(obj,
                                                          original + delta,
                                                          original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
        default: {
            for (;;)
            {
                short ret = InterlockedCompareExchange16(obj,
                                                         original + delta,
                                                         original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
    }
#endif
}
#endif

int32_t
__Atomic_fetchAdd32(ATOMIC(int32_t) * obj, int32_t delta, memory_order m)
{
#ifndef InterlockedExchangeAddNoFence
#define InterlockedExchangeAddNoFence InterlockedExchangeAdd
#endif
#ifndef InterlockedExchangeAddAcquire
#define InterlockedExchangeAddAcquire InterlockedExchangeAdd
#endif
#ifndef InterlockedExchangeAddRelease
#define InterlockedExchangeAddRelease InterlockedExchangeAdd
#endif
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeAddNoFence(obj, delta);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedExchangeAddAcquire(obj, delta);
        case memory_order_release:
            return InterlockedExchangeAddRelease(obj, delta);
        default:
            return InterlockedExchangeAdd(obj, delta);
    }
}

#if defined(InterlockedExchangeAdd64) || defined(InterlockedCompareExchange64)
int64_t
__Atomic_fetchAdd64(ATOMIC(int64_t) * obj, int64_t delta, memory_order m)
{
#ifdef InterlockedExchangeAdd64
#ifndef InterlockedExchangeAddNoFence64
#define InterlockedExchangeAddNoFence64 InterlockedExchangeAdd64
#endif
#ifndef InterlockedExchangeAddAcquire64
#define InterlockedExchangeAddAcquire64 InterlockedExchangeAdd64
#endif
#ifndef InterlockedExchangeAddRelease64
#define InterlockedExchangeAddRelease64 InterlockedExchangeAdd64
#endif
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeAddNoFence64(obj, delta);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedExchangeAddAcquire64(obj, delta);
        case memory_order_release:
            return InterlockedExchangeAddRelease64(obj, delta);
        default:
            return InterlockedExchangeAdd64(obj, delta);
    }
#else
#ifndef InterlockedCompareExchangeNoFence64
#define InterlockedCompareExchangeNoFence64 InterlockedCompareExchange64
#endif
#ifndef InterlockedCompareExchangeAcquire64
#define InterlockedCompareExchangeAcquire64 InterlockedCompareExchange64
#endif
#ifndef InterlockedCompareExchangeRelease64
#define InterlockedCompareExchangeRelease64 InterlockedCompareExchange64
#endif
    int64_t original = *obj;
    switch (m)
    {
        case memory_order_relaxed: {
            for (;;)
            {
                int64_t ret
                    = InterlockedCompareExchangeNoFence64(obj,
                                                          original + delta,
                                                          original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
        case memory_order_consume:
        case memory_order_acquire: {
            for (;;)
            {
                int64_t ret
                    = InterlockedCompareExchangeAcquire64(obj,
                                                          original + delta,
                                                          original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
        case memory_order_release: {
            for (;;)
            {
                int64_t ret
                    = InterlockedCompareExchangeRelease64(obj,
                                                          original + delta,
                                                          original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
        default: {
            for (;;)
            {
                int64_t ret = InterlockedCompareExchange64(obj,
                                                           original + delta,
                                                           original);
                if (ret == original)
                {
                    return ret;
                }
                original = ret;
            }
        }
    }
#endif
}
#endif

#if ENABLE_128_CAS
#include <lang/System.h>

static __INT128__
__Int128_add(__INT128__ val, __INT128__ delta)
{
    __INT128__ ret = val;
    uint64_t limit = -1ull;
    int lowIdx = isCpuBigEndian();
    uint64_t low = *(uint64_t*)(ret.pair + lowIdx);
    ret.pair[lowIdx] += delta.pair[lowIdx];
    bool overflow = false;
    if (limit - low < (uint64_t)delta.pair[lowIdx])
    {
        overflow = true;
    }
    ret.pair[1 - lowIdx] += delta.pair[1 - lowIdx];
    if (overflow)
    {
        ++ret.pair[1 - lowIdx];
    }
    return ret;
}

__INT128__
__Atomic_fetchAdd128(ATOMIC(__INT128__) * obj, __INT128__ delta, memory_order m)
{
    __INT128__ original = *obj;
    for (;;)
    {
        __INT128__ exchange = __Int128_add(original, delta);
        bool ret = InterlockedCompareExchange128(obj->pair,
                                                 exchange.pair[1],
                                                 exchange.pair[0],
                                                 original.pair);
        if (ret)
        {
            return original;
        }
    }
}
#endif

char
__Atomic_exchange8(ATOMIC(char) * obj, char val, memory_order m)
{
    return InterlockedExchange8(obj, val);
}

short
__Atomic_exchange16(ATOMIC(short) * obj, short val, memory_order m)
{
    return InterlockedExchange16(obj, val);
}

int32_t
__Atomic_exchange32(ATOMIC(int32_t) * obj, int32_t val, memory_order m)
{
#ifndef InterlockedExchangeNoFence
#define InterlockedExchangeNoFence InterlockedExchange
#endif
#ifndef InterlockedExchangeAcquire
#define InterlockedExchangeAcquire InterlockedExchange
#endif
#ifndef InterlockedExchangeRelease
#define InterlockedExchangeRelease InterlockedExchange
#endif
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeNoFence(obj, val);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedExchangeAcquire(obj, val);
        case memory_order_release:
            return InterlockedExchangeRelease(obj, val);
        default:
            return InterlockedExchange(obj, val);
    }
}

#if defined(InterlockedExchange64) || defined(InterlockedCompareExchange64)
int64_t
__Atomic_exchange64(ATOMIC(int64_t) * obj, int64_t val, memory_order m)
{
#ifdef InterlockedExchange64
#ifndef InterlockedExchangeNoFence64
#define InterlockedExchangeNoFence64 InterlockedExchange64
#endif
#ifndef InterlockedExchangeAcquire64
#define InterlockedExchangeAcquire64 InterlockedExchange64
#endif
#ifndef InterlockedExchangeRelease64
#define InterlockedExchangeRelease64 InterlockedExchange64
#endif
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeNoFence64(obj, val);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedExchangeAcquire64(obj, val);
        case memory_order_release:
            return InterlockedExchangeRelease64(obj, val);
        default:
            return InterlockedExchange64(obj, val);
    }
#else
#ifndef InterlockedCompareExchangeNoFence64
#define InterlockedCompareExchangeNoFence64 InterlockedCompareExchange64
#endif
#ifndef InterlockedCompareExchangeAcquire64
#define InterlockedCompareExchangeAcquire64 InterlockedCompareExchange64
#endif
#ifndef InterlockedCompareExchangeRelease64
#define InterlockedCompareExchangeRelease64 InterlockedCompareExchange64
#endif
    int64_t original = *obj;
    switch (m)
    {
        case memory_order_relaxed: {
            for (;;)
            {
                int64_t ret
                    = InterlockedCompareExchangeNoFence64(obj, val, original);
                if (ret == original)
                {
                    return original;
                }
                original = ret;
            }
            break;
        }
        case memory_order_consume:
        case memory_order_acquire: {
            for (;;)
            {
                int64_t ret
                    = InterlockedCompareExchangeAcquire64(obj, val, original);
                if (ret == original)
                {
                    return original;
                }
                original = ret;
            }
            break;
        }
        case memory_order_release: {
            for (;;)
            {
                int64_t ret
                    = InterlockedCompareExchangeRelease64(obj, val, original);
                if (ret == original)
                {
                    return original;
                }
                original = ret;
            }
            break;
        }
        default: {
            for (;;)
            {
                int64_t ret = InterlockedCompareExchange64(obj, val, original);
                if (ret == original)
                {
                    return original;
                }
                original = ret;
            }
            break;
        }
    }
#endif
}
#endif

#if ENABLE_128_CAS
__INT128__
__Atomic_exchange128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m)
{
    __INT128__ original = *obj;
    for (;;)
    {
        bool ret = InterlockedCompareExchange128(obj->pair,
                                                 val.pair[1],
                                                 val.pair[0],
                                                 original.pair);
        if (ret)
        {
            return original;
        }
    }
}
#endif

#if X86
bool
__Atomic_cas8(ATOMIC(char) * obj, char* expect, char exchange, memory_order m)
{
#ifdef InterlockedCompareExchange8
    char original = InterlockedCompareExchange8(obj, exchange, *expect);
    bool ret = original == *expect;
    if (!ret)
    {
        *expect = original;
    }
    return ret;
#else
    return false;
#endif
}
#endif

bool
__Atomic_cas16(ATOMIC(short) * obj,
               short* expect,
               short exchange,
               memory_order m)
{
    bool ret = false;
    short original;
    switch (m)
    {
        case memory_order_relaxed: {
            original
                = InterlockedCompareExchangeNoFence16(obj, exchange, *expect);
            break;
        }
        case memory_order_consume:
        case memory_order_acquire: {
            original
                = InterlockedCompareExchangeAcquire16(obj, exchange, *expect);
            break;
        }
        case memory_order_release: {
            original
                = InterlockedCompareExchangeRelease16(obj, exchange, *expect);
            break;
        }
        default: {
            original = InterlockedCompareExchange16(obj, exchange, *expect);
            break;
        }
    }
    ret = original == *expect;
    if (!ret)
    {
        *expect = original;
    }
    return ret;
}

bool
__Atomic_cas32(ATOMIC(int32_t) * obj,
               int32_t* expect,
               int32_t exchange,
               memory_order m)
{
    bool ret = false;
    int32_t original;
    switch (m)
    {
        case memory_order_relaxed: {
            original
                = InterlockedCompareExchangeNoFence(obj, exchange, *expect);
            break;
        }
        case memory_order_consume:
        case memory_order_acquire: {
            original
                = InterlockedCompareExchangeAcquire(obj, exchange, *expect);
            break;
        }
        case memory_order_release: {
            original
                = InterlockedCompareExchangeRelease(obj, exchange, *expect);
            break;
        }
        default: {
            original = InterlockedCompareExchange(obj, exchange, *expect);
            break;
        }
    }
    ret = original == *expect;
    if (!ret)
    {
        *expect = original;
    }
    return ret;
}

#ifdef InterlockedCompareExchange64
bool
__Atomic_cas64(ATOMIC(int64_t) * obj,
               int64_t* expect,
               int64_t exchange,
               memory_order m)
{
#ifndef InterlockedCompareExchangeNoFence64
#define InterlockedCompareExchangeNoFence64 InterlockedCompareExchange64
#endif
#ifndef InterlockedCompareExchangeAcquire64
#define InterlockedCompareExchangeAcquire64 InterlockedCompareExchange64
#endif
#ifndef InterlockedCompareExchangeRelease64
#define InterlockedCompareExchangeRelease64 InterlockedCompareExchange64
#endif
    bool ret = false;
    int64_t original;
    switch (m)
    {
        case memory_order_relaxed: {
            original
                = InterlockedCompareExchangeNoFence64(obj, exchange, *expect);
            break;
        }
        case memory_order_consume:
        case memory_order_acquire: {
            original
                = InterlockedCompareExchangeAcquire64(obj, exchange, *expect);
            break;
        }
        case memory_order_release: {
            original
                = InterlockedCompareExchangeRelease64(obj, exchange, *expect);
            break;
        }
        default: {
            original = InterlockedCompareExchange64(obj, exchange, *expect);
            break;
        }
    }
    ret = original == *expect;
    if (!ret)
    {
        *expect = original;
    }
    return ret;
}
#endif

#if ENABLE_128_CAS
bool
__Atomic_cas128(ATOMIC(__INT128__) * obj,
                __INT128__* expect,
                __INT128__ exchange,
                memory_order m)
{
    return InterlockedCompareExchange128(obj->pair,
                                         exchange.pair[1],
                                         exchange.pair[0],
                                         expect->pair);
}
#endif

#if X86
bool
__Atomic_weakCas8(ATOMIC(char) * obj,
                  char* expect,
                  char exchange,
                  memory_order m)
{
    return __Atomic_cas8(obj, expect, exchange, m);
}
#endif

bool
__Atomic_weakCas16(ATOMIC(short) * obj,
                   short* expect,
                   short exchange,
                   memory_order m)
{
    return __Atomic_cas16(obj, expect, exchange, m);
}

bool
__Atomic_weakCas32(ATOMIC(int32_t) * obj,
                   int32_t* expect,
                   int32_t exchange,
                   memory_order m)
{
    return __Atomic_cas32(obj, expect, exchange, m);
}

bool
__Atomic_weakCas64(ATOMIC(int64_t) * obj,
                   int64_t* expect,
                   int64_t exchange,
                   memory_order m)
{
    return __Atomic_cas64(obj, expect, exchange, m);
}

#if ENABLE_128_CAS
bool
__Atomic_weakCas128(ATOMIC(__INT128__) * obj,
                    __INT128__* expect,
                    __INT128__ exchange,
                    memory_order m)
{
    return __Atomic_cas128(obj, expect, exchange, m);
}
#endif
#endif
