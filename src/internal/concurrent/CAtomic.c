#include "xcl/concurrent/CAtomic.h"

#if CLANG || GNUC
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
__int128_t
__Atomic_load128(ATOMIC(__int128_t) * obj, memory_order m)
{
    return atomic_load_explicit(obj, m);
}

void
__Atomic_store128(ATOMIC(__int128_t) * obj, __int128_t val, memory_order m)
{
    atomic_store_explicit(obj, val, m);
}

__int128_t
__Atomic_fetchAdd128(ATOMIC(__int128_t) * obj, __int128_t delta, memory_order m)
{
    return atomic_fetch_add_explicit(obj, delta, m);
}

__int128_t
__Atomic_exchange128(ATOMIC(__int128_t) * obj, __int128_t val, memory_order m)
{
    return atomic_exchange_explicit(obj, val, m);
}

bool
__Atomic_cas128(ATOMIC(__int128_t) * obj,
                __int128_t* expect,
                __int128_t exchange,
                memory_order m)
{
    __int128_t origin = *expect;
    return atomic_compare_exchange_strong_explicit(obj, expect, exchange, m, m)
           == origin;
}

bool
__Atomic_weakCas128(ATOMIC(__int128_t) * obj,
                    __int128_t* expect,
                    __int128_t exchange,
                    memory_order m)
{
    __int128_t origin = *expect;
    return atomic_compare_exchange_weak_explicit(obj, expect, exchange, m, m)
           == origin;
}
#endif
#else
#endif
