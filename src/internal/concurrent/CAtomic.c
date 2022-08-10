#include "xcl/concurrent/CAtomic.h"

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
    __INT128__ origin = *expect;
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m);
}

bool
__Atomic_weakCas128(ATOMIC(__INT128__) * obj,
                    __INT128__* expect,
                    __INT128__ exchange,
                    memory_order m)
{
    __INT128__ origin = *expect;
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m);
}
#endif
#else
#include <windows.h>

#ifdef InterlockedOr8
char
__Atomic_load8(ATOMIC(char) * obj, memory_order m)
{
    return InterlockedOr8(obj, 0);
}
#endif

#ifdef InterlockedOr16
short
__Atomic_load16(ATOMIC(short) * obj, memory_order m)
{
    return InterlockedOr16(obj, 0);
}
#endif

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

#if X64
__INT128__
__Atomic_load128(ATOMIC(__INT128__) * obj, memory_order m)
{
#ifdef InterlockedCompareExchange128
    __INT128__ expect = *obj;
    __INT128__ exchange = expect;
    InterlockedCompareExchange128(obj->pair,
                                  exchange.pair[1],
                                  exchange.pair[0],
                                  expect.pair);
    return expect;
#else
#error "current platform doesn't support 128 bits cas"
#endif
}
#endif

#ifdef InterlockedExchange8
void
__Atomic_store8(ATOMIC(char) * obj, char val, memory_order m)
{
    InterlockedExchange8(obj, val);
}
#endif

#ifdef InterlockedExchange16
void
__Atomic_store16(ATOMIC(short) * obj, short val, memory_order m)
{
    InterlockedExchange16(obj, val);
}
#endif

void
__Atomic_store32(ATOMIC(int32_t) * obj, int32_t val, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
            InterlockedExchangeNoFence(obj, val);
            break;
        case memory_order_consume:
        case memory_order_acquire:
            InterlockedExchangeAcquire(obj, val);
            break;
        default:
            InterlockedExchange(obj, val);
            break;
    }
}

void
__Atomic_store64(ATOMIC(int64_t) * obj, int64_t val, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
            InterlockedExchangeNoFence64(obj, val);
            break;
        case memory_order_consume:
        case memory_order_acquire:
            InterlockedExchangeAcquire64(obj, val);
            break;
        default:
            InterlockedExchange64(obj, val);
            break;
    }
}

#if X64
void
__Atomic_store128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m)
{
#ifdef InterlockedCompareExchange128
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
#else
#error "current platform doesn't support 128 bits cas"
#endif
}
#endif

#ifdef InterlockedExchangeAdd8
char
__Atomic_fetchAdd8(ATOMIC(char) * obj, char delta, memory_order m)
{
    return InterlockedExchangeAdd8(obj, delta);
}
#endif

short
__Atomic_fetchAdd16(ATOMIC(short) * obj, short delta, memory_order m)
{
#ifdef InterlockedExchangeAdd16
    return InterlockedExchangeAdd16(obj, delta);
#else
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

int32_t
__Atomic_fetchAdd32(ATOMIC(int32_t) * obj, int32_t delta, memory_order m)
{
#ifdef InterlockedExchangeAdd
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
#else
    int32_t original = *obj;
    switch (m)
    {
        case memory_order_relaxed: {
            for (;;)
            {
                int32_t ret
                    = InterlockedCompareExchangeNoFence(obj,
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
                int32_t ret
                    = InterlockedCompareExchangeAcquire(obj,
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
                int32_t ret
                    = InterlockedCompareExchangeRelease(obj,
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
                int32_t ret = InterlockedCompareExchange(obj,
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

int64_t
__Atomic_fetchAdd64(ATOMIC(int64_t) * obj, int64_t delta, memory_order m)
{
#ifdef InterlockedExchangeAdd
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeAddNoFence64(obj, delta);
        case memory_order_consume:
        case memory_order_acquire:
#ifdef InterlockedExchangeAddAcquire64
            return InterlockedExchangeAddAcquire64(obj, delta);
#endif
        case memory_order_release:
#ifdef InterlockedExchangeAddRelease64
            return InterlockedExchangeAddRelease64(obj, delta);
#endif
        default:
            return InterlockedExchangeAdd64(obj, delta);
    }
#else
    int32_t original = *obj;
    switch (m)
    {
        case memory_order_relaxed: {
            for (;;)
            {
                int32_t ret
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
                int32_t ret
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
                int32_t ret
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
                int32_t ret = InterlockedCompareExchange64(obj,
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

#if X64
#include <xcl/lang/CSys.h>

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
#ifdef InterlockedCompareExchange128
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
#else
#error "current platform doesn't support 128 bits cas"
#endif
}
#endif

#ifdef InterlockedExchange8
char
__Atomic_exchange8(ATOMIC(char) * obj, char val, memory_order m)
{
    return InterlockedExchange8(obj, val);
}
#elif defined(InterlockedExchangeAdd8)
char
__Atomic_exchange8(ATOMIC(char) * obj, char val, memory_order m)
{
    return InterlockedExchangeAdd8(obj, val);
}
#endif

#ifdef InterlockedExchange16
short
__Atomic_exchange16(ATOMIC(short) * obj, short val, memory_order m)
{
    return InterlockedExchange16(obj, val);
}
#endif

int32_t
__Atomic_exchange32(ATOMIC(int32_t) * obj, int32_t val, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeNoFence(obj, val);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedExchangeAcquire(obj, val);
        default:
            return InterlockedExchange(obj, val);
    }
}

int64_t
__Atomic_exchange64(ATOMIC(int64_t) * obj, int64_t val, memory_order m)
{
    switch (m)
    {
        case memory_order_relaxed:
            return InterlockedExchangeNoFence64(obj, val);
        case memory_order_consume:
        case memory_order_acquire:
            return InterlockedExchangeAcquire64(obj, val);
        default:
            return InterlockedExchange64(obj, val);
    }
}

#if X64
__INT128__
__Atomic_exchange128(ATOMIC(__INT128__) * obj, __INT128__ val, memory_order m)
{
#ifdef InterlockedCompareExchange128
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
#else
#error "current platform doesn't support 128 bits cas"
#endif
}
#endif

#if X86
bool
__Atomic_cas8(ATOMIC(char) * obj, char* expect, char exchange, memory_order m)
{
#if defined(_InterlockedCompareExchange8)
    char original = _InterlockedCompareExchange8(obj, exchange, *expect);
    bool ret = original == *expect;
    if (!ret)
    {
        *expect = original;
    }
    return ret;
#else
#error "current platform doesn't support cas 8 bits"
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

bool
__Atomic_cas64(ATOMIC(int64_t) * obj,
               int64_t* expect,
               int64_t exchange,
               memory_order m)
{
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

#if X64
bool
__Atomic_cas128(ATOMIC(__INT128__) * obj,
                __INT128__* expect,
                __INT128__ exchange,
                memory_order m)
{
#ifdef InterlockedCompareExchange128
    return InterlockedCompareExchange128(obj->pair,
                                         exchange.pair[1],
                                         exchange.pair[0],
                                         expect->pair);
#else
#error "current platform doesn't support 128 bits cas"
#endif
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

#if X64
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
