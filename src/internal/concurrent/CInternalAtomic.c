//
// Created by xuyan on 2022/7/28.
//

#include "xcl/concurrent/CInternalAtomic.h"
#include "xcl/lang/XclDef.h"
#include <stdbool.h>
#include <stdint.h>

#if X64
typedef struct {
    int64_t pair[2];
} __Int128;
#endif

#if defined(_MSC_VER)
#include <windows.h>

static short
__Msvc_atomicInc16Relaxed(volatile short* object)
{
    return InterlockedIncrement16(object);
}

static short
__Msvc_atomicInc16Acq(volatile short* object)
{
    return InterlockedIncrement16(object);
}

static short
__Msvc_atomicInc16Rel(volatile short* object)
{
    return InterlockedIncrement16(object);
}

static short
__Msvc_atomicInc16(volatile short* object)
{
    return InterlockedIncrement16(object);
}

static short (*__MsvcAtomicInc16Funcs[])(volatile short*)
    = {__Msvc_atomicInc16Relaxed,
       __Msvc_atomicInc16Acq,
       __Msvc_atomicInc16Acq,
       __Msvc_atomicInc16Rel,
       __Msvc_atomicInc16,
       __Msvc_atomicInc16};

static LONG
__Msvc_atomicInc32Relaxed(volatile LONG* object)
{
    return InterlockedIncrement(object);
}

static LONG
__Msvc_atomicInc32Acq(volatile LONG* object)
{
    return InterlockedIncrement(object);
}

static LONG
__Msvc_atomicInc32Rel(volatile LONG* object)
{
    return InterlockedIncrement(object);
}

static LONG
__Msvc_atomicInc32(volatile LONG* object)
{
    return InterlockedIncrement(object);
}

static LONG (*__MsvcAtomicInc32Funcs[])(volatile LONG*)
    = {__Msvc_atomicInc32Relaxed,
       __Msvc_atomicInc32Acq,
       __Msvc_atomicInc32Acq,
       __Msvc_atomicInc32Rel,
       __Msvc_atomicInc32,
       __Msvc_atomicInc32};

static LONG64
__Msvc_atomicInc64Relaxed(volatile LONG64* object)
{
    return InterlockedIncrement64(object);
}

static LONG64
__Msvc_atomicInc64Acq(volatile LONG64* object)
{
    return InterlockedIncrement64(object);
}

static LONG64
__Msvc_atomicInc64Rel(volatile LONG64* object)
{
    return InterlockedIncrement64(object);
}

static LONG64
__Msvc_atomicInc64(volatile LONG64* object)
{
    return InterlockedIncrement64(object);
}

static LONG64 (*__MsvcAtomicInc64Funcs[])(volatile LONG64*)
    = {__Msvc_atomicInc64Relaxed,
       __Msvc_atomicInc64Acq,
       __Msvc_atomicInc64Acq,
       __Msvc_atomicInc64Rel,
       __Msvc_atomicInc64,
       __Msvc_atomicInc64};

bool
__XCL_atomic_inc(void* object, void* result, int size, memory_order memoryOrder)
{
    switch (size)
    {
    case 2: {
        *(short*)result = __MsvcAtomicInc16Funcs[memoryOrder](object);
        break;
    }
    case 4: {
        *(LONG*)result = __MsvcAtomicInc32Funcs[memoryOrder](object);
        break;
    }
    case 8: {
        *(LONG64*)result = __MsvcAtomicInc64Funcs[memoryOrder](object);
        break;
    }
    default:
        return false;
    }
    return true;
}

static short
__Msvc_atomicDec16Relaxed(volatile short* object)
{
    return InterlockedDecrement16(object);
}

static short
__Msvc_atomicDec16Acq(volatile short* object)
{
    return InterlockedDecrement16(object);
}

static short
__Msvc_atomicDec16Rel(volatile short* object)
{
    return InterlockedDecrement16(object);
}

static short
__Msvc_atomicDec16(volatile short* object)
{
    return InterlockedDecrement16(object);
}

static short (*__MsvcAtomicDec16Funcs[])(volatile short*)
    = {__Msvc_atomicDec16Relaxed,
       __Msvc_atomicDec16Acq,
       __Msvc_atomicDec16Acq,
       __Msvc_atomicDec16Rel,
       __Msvc_atomicDec16,
       __Msvc_atomicDec16};

static LONG
__Msvc_atomicDec32Relaxed(volatile LONG* object)
{
    return InterlockedDecrement(object);
}

static LONG
__Msvc_atomicDec32Acq(volatile LONG* object)
{
    return InterlockedDecrement(object);
}

static LONG
__Msvc_atomicDec32Rel(volatile LONG* object)
{
    return InterlockedDecrement(object);
}

static LONG
__Msvc_atomicDec32(volatile LONG* object)
{
    return InterlockedDecrement(object);
}

static LONG (*__MsvcAtomicDec32Funcs[])(volatile LONG*)
    = {__Msvc_atomicDec32Relaxed,
       __Msvc_atomicDec32Acq,
       __Msvc_atomicDec32Acq,
       __Msvc_atomicDec32Rel,
       __Msvc_atomicDec32,
       __Msvc_atomicDec32};

static LONG64
__Msvc_atomicDec64Relaxed(volatile LONG64* object)
{
    return InterlockedDecrement64(object);
}

static LONG64
__Msvc_atomicDec64Acq(volatile LONG64* object)
{
    return InterlockedDecrement64(object);
}

static LONG64
__Msvc_atomicDec64Rel(volatile LONG64* object)
{
    return InterlockedDecrement64(object);
}

static LONG64
__Msvc_atomicDec64(volatile LONG64* object)
{
    return InterlockedDecrement64(object);
}

static LONG64 (*__MsvcAtomicDec64Funcs[])(volatile LONG64*)
    = {__Msvc_atomicDec64Relaxed,
       __Msvc_atomicDec64Acq,
       __Msvc_atomicDec64Acq,
       __Msvc_atomicDec64Rel,
       __Msvc_atomicDec64,
       __Msvc_atomicDec64};

bool
__XCL_atomic_dec(void* object, void* result, int size, memory_order memoryOrder)
{
    switch (size)
    {
    case 2:
        *(SHORT*)result = __MsvcAtomicDec16Funcs[memoryOrder](object);
        break;
    case 4:
        *(LONG*)result = __MsvcAtomicDec32Funcs[memoryOrder](object);
        break;
    case 8:
        *(LONG64*)result = __MsvcAtomicDec64Funcs[memoryOrder](object);
        break;
    default:
        return false;
    }
    return true;
}

static LONG
__Msvc_atomicAdd32Relaxed(volatile LONG* object, LONG delta)
{
    return InterlockedAdd(object, delta);
}

static LONG
__Msvc_atomicAdd32Acq(volatile LONG* object, LONG delta)
{
    return InterlockedAdd(object, delta);
}

static LONG
__Msvc_atomicAdd32Rel(volatile LONG* object, LONG delta)
{
    return InterlockedAdd(object, delta);
}

static LONG
__Msvc_atomicAdd32(volatile LONG* object, LONG delta)
{
    return InterlockedAdd(object, delta);
}

static LONG (*__MsvcAtomicAdd32Funcs[])(volatile LONG*, LONG)
    = {__Msvc_atomicAdd32Relaxed,
       __Msvc_atomicAdd32Acq,
       __Msvc_atomicAdd32Acq,
       __Msvc_atomicAdd32Rel,
       __Msvc_atomicAdd32,
       __Msvc_atomicAdd32};

static LONG64
__Msvc_atomicAdd64Relaxed(volatile LONG64* object, LONG64 delta)
{
    return InterlockedAdd64(object, delta);
}

static LONG64
__Msvc_atomicAdd64Acq(volatile LONG64* object, LONG64 delta)
{
    return InterlockedAdd64(object, delta);
}

static LONG64
__Msvc_atomicAdd64Rel(volatile LONG64* object, LONG64 delta)
{
    return InterlockedAdd64(object, delta);
}

static LONG64
__Msvc_atomicAdd64(volatile LONG64* object, LONG64 delta)
{
    return InterlockedAdd64(object, delta);
}

static LONG64 (*__MsvcAtomicAdd64Funcs[])(volatile LONG64*, LONG64)
    = {__Msvc_atomicAdd64Relaxed,
       __Msvc_atomicAdd64Acq,
       __Msvc_atomicAdd64Acq,
       __Msvc_atomicAdd64Rel,
       __Msvc_atomicAdd64,
       __Msvc_atomicAdd64};

bool
__XCL_atomic_add(void* object,
                 const void* value,
                 void* result,
                 int size,
                 memory_order memoryOrder)
{
    switch (size)
    {
    case 4:
        *(LONG*)result
            = __MsvcAtomicAdd32Funcs[memoryOrder](object, *(const LONG*)value);
        break;
    case 8:
        *(LONG64*)result
            = __MsvcAtomicAdd64Funcs[memoryOrder](object,
                                                  *(const LONG64*)value);
        break;
    default:
        return false;
    }
    return true;
}

#ifdef _MSC_VER
static char
__Msvc_atomicLoad8Relaxed(volatile char* object)
{
    return InterlockedOr8(object, 0);
}

static char
__Msvc_atomicLoad8Acq(volatile char* object)
{
    return InterlockedOr8(object, 0);
}

static char
__Msvc_atomicLoad8Rel(volatile char* object)
{
    return InterlockedOr8(object, 0);
}

static char
__Msvc_atomicLoad8(volatile char* object)
{
    return InterlockedOr8(object, 0);
}

static char (*__MsvcAtomicLoad8Funcs[])(volatile char*)
    = {__Msvc_atomicLoad8Relaxed,
       __Msvc_atomicLoad8Acq,
       __Msvc_atomicLoad8Acq,
       __Msvc_atomicLoad8Rel,
       __Msvc_atomicLoad8,
       __Msvc_atomicLoad8};

static short
__Msvc_atomicLoad16Relaxed(volatile short* object)
{
    return InterlockedOr16(object, 0);
}

static short
__Msvc_atomicLoad16Acq(volatile short* object)
{
    return InterlockedOr16(object, 0);
}

static short
__Msvc_atomicLoad16Rel(volatile short* object)
{
    return InterlockedOr16(object, 0);
}

static short
__Msvc_atomicLoad16(volatile short* object)
{
    return InterlockedOr16(object, 0);
}

static short (*__MsvcAtomicLoad16Funcs[])(volatile short*)
    = {__Msvc_atomicLoad16Relaxed,
       __Msvc_atomicLoad16Acq,
       __Msvc_atomicLoad16Acq,
       __Msvc_atomicLoad16Rel,
       __Msvc_atomicLoad16,
       __Msvc_atomicLoad16};
#endif

static LONG
__Msvc_atomicLoad32Relaxed(volatile LONG* object)
{
    return InterlockedOr(object, 0);
}

static LONG
__Msvc_atomicLoad32Acq(volatile LONG* object)
{
    return InterlockedOr(object, 0);
}

static LONG
__Msvc_atomicLoad32Rel(volatile LONG* object)
{
    return InterlockedOr(object, 0);
}

static LONG
__Msvc_atomicLoad32(volatile LONG* object)
{
    return InterlockedOr(object, 0);
}

static LONG (*__MsvcAtomicLoad32Funcs[])(volatile LONG*)
    = {__Msvc_atomicLoad32Relaxed,
       __Msvc_atomicLoad32Acq,
       __Msvc_atomicLoad32Acq,
       __Msvc_atomicLoad32Rel,
       __Msvc_atomicLoad32,
       __Msvc_atomicLoad32};

static LONG64
__Msvc_atomicLoad64Relaxed(volatile LONG64* object)
{
    return InterlockedOr64(object, 0);
}

static LONG64
__Msvc_atomicLoad64Acq(volatile LONG64* object)
{
    return InterlockedOr64(object, 0);
}

static LONG64
__Msvc_atomicLoad64Rel(volatile LONG64* object)
{
    return InterlockedOr64(object, 0);
}

static LONG64
__Msvc_atomicLoad64(volatile LONG64* object)
{
    return InterlockedOr64(object, 0);
}

static LONG64 (*__MsvcAtomicLoad64Funcs[])(volatile LONG64*)
    = {__Msvc_atomicLoad64Relaxed,
       __Msvc_atomicLoad64Acq,
       __Msvc_atomicLoad64Acq,
       __Msvc_atomicLoad64Rel,
       __Msvc_atomicLoad64,
       __Msvc_atomicLoad64};

bool
__XCL_atomic_load(void* object,
                  void* result,
                  int size,
                  memory_order memoryOrder)
{
    switch (size)
    {
#ifdef _MSC_VER
    case 1:
        *(char*)result = __MsvcAtomicLoad8Funcs[memoryOrder](object);
        break;
    case 2:
        *(short*)result = __MsvcAtomicLoad16Funcs[memoryOrder](object);
        break;
#endif
    case 4:
        *(LONG*)result = __MsvcAtomicLoad32Funcs[memoryOrder](object);
        break;
    case 8:
        *(LONG64*)result = __MsvcAtomicLoad64Funcs[memoryOrder](object);
        break;
    default:
        return false;
    }
    return true;
}

#if defined(_MSC_VER)
static char
__Msvc_atomicExchange8Relaxed(volatile char* object, char val)
{
    return InterlockedExchange8(object, val);
}

static char
__Msvc_atomicExchange8Acq(volatile char* object, char val)
{
    return InterlockedExchange8(object, val);
}

static char
__Msvc_atomicExchange8Rel(volatile char* object, char val)
{
    return InterlockedExchange8(object, val);
}

static char
__Msvc_atomicExchange8(volatile char* object, char val)
{
    return InterlockedExchange8(object, val);
}

static char (*__MsvcAtomicExchange8Funcs[])(volatile char*, char)
    = {__Msvc_atomicExchange8Relaxed,
       __Msvc_atomicExchange8Acq,
       __Msvc_atomicExchange8Acq,
       __Msvc_atomicExchange8Rel,
       __Msvc_atomicExchange8,
       __Msvc_atomicExchange8};

static short
__Msvc_atomicExchange16Relaxed(volatile short* object, short val)
{
    return InterlockedExchange16(object, val);
}

static short
__Msvc_atomicExchange16Acq(volatile short* object, short val)
{
    return InterlockedExchange16(object, val);
}

static short
__Msvc_atomicExchange16Rel(volatile short* object, short val)
{
    return InterlockedExchange16(object, val);
}

static short
__Msvc_atomicExchange16(volatile short* object, short val)
{
    return InterlockedExchange16(object, val);
}

static short (*__MsvcAtomicExchange16Funcs[])(volatile short*, short)
    = {__Msvc_atomicExchange16Relaxed,
       __Msvc_atomicExchange16Acq,
       __Msvc_atomicExchange16Acq,
       __Msvc_atomicExchange16Rel,
       __Msvc_atomicExchange16,
       __Msvc_atomicExchange16};
#endif

static LONG
__Msvc_atomicExchange32Relaxed(volatile LONG* object, LONG val)
{
    return InterlockedExchange(object, val);
}

static LONG
__Msvc_atomicExchange32Acq(volatile LONG* object, LONG val)
{
    return InterlockedExchange(object, val);
}

static LONG
__Msvc_atomicExchange32Rel(volatile LONG* object, LONG val)
{
    return InterlockedExchange(object, val);
}

static LONG
__Msvc_atomicExchange32(volatile LONG* object, LONG val)
{
    return InterlockedExchange(object, val);
}

static LONG (*__MsvcAtomicExchange32Funcs[])(volatile LONG*, LONG)
    = {__Msvc_atomicExchange32Relaxed,
       __Msvc_atomicExchange32Acq,
       __Msvc_atomicExchange32Acq,
       __Msvc_atomicExchange32Rel,
       __Msvc_atomicExchange32,
       __Msvc_atomicExchange32};

static LONG64
__Msvc_atomicExchange64Relaxed(volatile LONG64* object, LONG64 val)
{
    return InterlockedExchange64(object, val);
}

static LONG64
__Msvc_atomicExchange64Acq(volatile LONG64* object, LONG64 val)
{
    return InterlockedExchange64(object, val);
}

static LONG64
__Msvc_atomicExchange64Rel(volatile LONG64* object, LONG64 val)
{
    return InterlockedExchange64(object, val);
}

static LONG64
__Msvc_atomicExchange64(volatile LONG64* object, LONG64 val)
{
    return InterlockedExchange64(object, val);
}

static LONG64 (*__MsvcAtomicExchange64Funcs[])(volatile LONG64*, LONG64)
    = {__Msvc_atomicExchange64Relaxed,
       __Msvc_atomicExchange64Acq,
       __Msvc_atomicExchange64Acq,
       __Msvc_atomicExchange64Rel,
       __Msvc_atomicExchange64,
       __Msvc_atomicExchange64};

bool
__XCL_atomic_store(void* object,
                   const void* value,
                   int size,
                   memory_order memoryOrder)
{
    switch (size)
    {
#ifdef _MSC_VER
    case 1:
        __MsvcAtomicExchange8Funcs[memoryOrder](object, *(char*)value);
        break;
    case 2:
        __MsvcAtomicExchange16Funcs[memoryOrder](object, *(short*)value);
        break;
#endif
    case 4:
        __MsvcAtomicExchange32Funcs[memoryOrder](object, *(LONG*)value);
        break;
    case 8:
        __MsvcAtomicExchange64Funcs[memoryOrder](object, *(LONG64*)value);
        break;
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_exchange(void* object,
                      const void* value,
                      void* result,
                      int size,
                      memory_order memoryOrder)
{
    switch (size)
    {
#ifdef _MSC_VER
    case 1:
        *(char*)result
            = __MsvcAtomicExchange8Funcs[memoryOrder](object, *(char*)value);
        break;
    case 2:
        *(short*)result
            = __MsvcAtomicExchange16Funcs[memoryOrder](object, *(short*)value);
        break;
#endif
    case 4:
        *(LONG*)result
            = __MsvcAtomicExchange32Funcs[memoryOrder](object, *(LONG*)value);
        break;
    case 8:
        *(LONG64*)result
            = __MsvcAtomicExchange64Funcs[memoryOrder](object, *(LONG64*)value);
        break;
    default:
        return false;
    }
    return true;
}

static short
__Msvc_cas16Relaxed(volatile short* object,
                    const short expect,
                    const short exchange)
{
    return InterlockedCompareExchange16(object, exchange, expect);
}

static short
__Msvc_cas16Acq(volatile short* object,
                const short expect,
                const short exchange)
{
    return InterlockedCompareExchange16(object, exchange, expect);
}

static short
__Msvc_cas16Rel(volatile short* object,
                const short expect,
                const short exchange)
{
    return InterlockedCompareExchange16(object, exchange, expect);
}

static short
__Msvc_cas16(volatile short* object, const short expect, const short exchange)
{
    return InterlockedCompareExchange16(object, exchange, expect);
}

static short (*__MsvcCas16Funcs[])(volatile short*, const short, const short)
    = {__Msvc_cas16Relaxed,
       __Msvc_cas16Acq,
       __Msvc_cas16Acq,
       __Msvc_cas16Rel,
       __Msvc_cas16,
       __Msvc_cas16};

static LONG
__Msvc_cas32Relaxed(volatile LONG* object,
                    const LONG expect,
                    const LONG exchange)
{
    return InterlockedCompareExchange(object, exchange, expect);
}

static LONG
__Msvc_cas32Acq(volatile LONG* object, const LONG expect, const LONG exchange)
{
    return InterlockedCompareExchange(object, exchange, expect);
}

static LONG
__Msvc_cas32Rel(volatile LONG* object, const LONG expect, const LONG exchange)
{
    return InterlockedCompareExchange(object, exchange, expect);
}

static LONG
__Msvc_cas32(volatile LONG* object, const LONG expect, const LONG exchange)
{
    return InterlockedCompareExchange(object, exchange, expect);
}

static LONG (*__MsvcCas32Funcs[])(volatile LONG*, const LONG, const LONG)
    = {__Msvc_cas32Relaxed,
       __Msvc_cas32Acq,
       __Msvc_cas32Acq,
       __Msvc_cas32Rel,
       __Msvc_cas32,
       __Msvc_cas32};

static LONG64
__Msvc_cas64Relaxed(volatile LONG64* object,
                    const LONG64 expect,
                    const LONG64 exchange)
{
    return InterlockedCompareExchange64(object, exchange, expect);
}

static LONG64
__Msvc_cas64Acq(volatile LONG64* object,
                const LONG64 expect,
                const LONG64 exchange)
{
    return InterlockedCompareExchange64(object, exchange, expect);
}

static LONG64
__Msvc_cas64Rel(volatile LONG64* object,
                const LONG64 expect,
                const LONG64 exchange)
{
    return InterlockedCompareExchange64(object, exchange, expect);
}

static LONG64
__Msvc_cas64(volatile LONG64* object,
             const LONG64 expect,
             const LONG64 exchange)
{
    return InterlockedCompareExchange64(object, exchange, expect);
}

static LONG64 (*__MsvcCas64Funcs[])(volatile LONG64*,
                                    const LONG64,
                                    const LONG64)
    = {__Msvc_cas64Relaxed,
       __Msvc_cas64Acq,
       __Msvc_cas64Acq,
       __Msvc_cas64Rel,
       __Msvc_cas64,
       __Msvc_cas64};

#if defined(_MSC_VER) && X64
static __Int128
__Msvc_cas128(volatile __Int128* object, __Int128 expect, __Int128 exchange);

static __Int128
__Msvc_cas128Relaxed(volatile __Int128* object,
                     const __Int128 expect,
                     const __Int128 exchange)
{
    return __Msvc_cas128(object, expect, exchange);
}

static __Int128
__Msvc_cas128Acq(volatile __Int128* object,
                 const __Int128 expect,
                 const __Int128 exchange)
{
    return __Msvc_cas128(object, expect, exchange);
}

static __Int128
__Msvc_cas128Rel(volatile __Int128* object,
                 const __Int128 expect,
                 const __Int128 exchange)
{
    return __Msvc_cas128(object, expect, exchange);
}

static __Int128
__Msvc_cas128(volatile __Int128* object,
              const __Int128 expect,
              const __Int128 exchange)
{
    __Int128 compare = expect;
    InterlockedCompareExchange128(object->pair,
                                  exchange.pair[1],
                                  exchange.pair[0],
                                  compare.pair);
    return compare;
}

static __Int128 (*__MsvcCas128Funcs[])(volatile __Int128*,
                                       const __Int128,
                                       const __Int128)
    = {__Msvc_cas128Relaxed,
       __Msvc_cas128Acq,
       __Msvc_cas128Acq,
       __Msvc_cas128Rel,
       __Msvc_cas128,
       __Msvc_cas128};
#endif

bool
__XCL_atomic_cas(void* object,
                 void* expect,
                 const void* exchange,
                 int size,
                 memory_order memoryOrder)
{
    bool ret = true;
    switch (size)
    {
    case 2:
        *(short*)expect
            = __MsvcCas16Funcs[memoryOrder](object,
                                            *(short*)expect,
                                            *(const short*)exchange);
        break;
    case 4:
        *(LONG*)expect = __MsvcCas32Funcs[memoryOrder](object,
                                                       *(LONG*)expect,
                                                       *(const LONG*)exchange);
        break;
    case 8:
        *(LONG64*)expect
            = __MsvcCas64Funcs[memoryOrder](object,
                                            *(LONG64*)expect,
                                            *(const LONG64*)exchange);
        break;
#if X64 && defined(_MSC_VER)
    case 16: {
        *(__Int128*)expect
            = __MsvcCas128Funcs[memoryOrder](object,
                                             *(__Int128*)expect,
                                             *(const __Int128*)exchange);
        break;
    }
#endif
    default:
        ret = false;
        break;
    }
    return ret;
}

bool
__XCL_atomic_casWeak(void* object,
                     void* expect,
                     const void* exchange,
                     int size,
                     memory_order memoryOrder)
{
    return __XCL_atomic_cas(object, expect, exchange, size, memoryOrder);
}
#else
bool
__XCL_atomic_inc(void* object, void* result, int size, memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)result
            = atomic_fetch_add_explicit((ATOMIC(char)*)object, 1, memoryOrder);
        break;
    }
    case 2: {
        *(short*)result
            = atomic_fetch_add_explicit((ATOMIC(short)*)object, 1, memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)result = atomic_fetch_add_explicit((ATOMIC(int32_t)*)object,
                                                      1,
                                                      memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)result = atomic_fetch_add_explicit((ATOMIC(int64_t)*)object,
                                                      1,
                                                      memoryOrder);
        break;
    }
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_dec(void* object, void* result, int size, memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)result
            = atomic_fetch_add_explicit((ATOMIC(char)*)object, -1, memoryOrder);
        break;
    }
    case 2: {
        *(short*)result = atomic_fetch_add_explicit((ATOMIC(short)*)object,
                                                    -1,
                                                    memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)result = atomic_fetch_add_explicit((ATOMIC(int32_t)*)object,
                                                      -1,
                                                      memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)result = atomic_fetch_add_explicit((ATOMIC(int64_t)*)object,
                                                      -1,
                                                      memoryOrder);
        break;
    }
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_add(void* object,
                 const void* value,
                 void* result,
                 int size,
                 memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)result = atomic_fetch_add_explicit((ATOMIC(char)*)object,
                                                   *(const char*)value,
                                                   memoryOrder);
        break;
    }
    case 2: {
        *(short*)result = atomic_fetch_add_explicit((ATOMIC(short)*)object,
                                                    *(const short*)value,
                                                    memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)result = atomic_fetch_add_explicit((ATOMIC(int32_t)*)object,
                                                      *(const int32_t*)value,
                                                      memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)result = atomic_fetch_add_explicit((ATOMIC(int64_t)*)object,
                                                      *(const int64_t*)value,
                                                      memoryOrder);
        break;
    }
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_load(void* object,
                  void* result,
                  int size,
                  memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)result
            = atomic_load_explicit((ATOMIC(char)*)object, memoryOrder);
        break;
    }
    case 2: {
        *(short*)result
            = atomic_load_explicit((ATOMIC(short)*)object, memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)result
            = atomic_load_explicit((ATOMIC(int32_t)*)object, memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)result
            = atomic_load_explicit((ATOMIC(int64_t)*)object, memoryOrder);
        break;
    }
#if X64
    case 16: {
        *(__Int128*)result
            = atomic_load_explicit((ATOMIC(__Int128)*)object, memoryOrder);
        break;
    }
#endif
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_store(void* object,
                   const void* value,
                   int size,
                   memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        atomic_store_explicit((ATOMIC(char)*)object,
                              *(const char*)value,
                              memoryOrder);
        break;
    }
    case 2: {
        atomic_store_explicit((ATOMIC(short)*)object,
                              *(const short*)value,
                              memoryOrder);
        break;
    }
    case 4: {
        atomic_store_explicit((ATOMIC(int32_t)*)object,
                              *(const int32_t*)value,
                              memoryOrder);
        break;
    }
    case 8: {
        atomic_store_explicit((ATOMIC(int64_t)*)object,
                              *(const int64_t*)value,
                              memoryOrder);
        break;
    }
#if X64
    case 16: {
        atomic_store_explicit((ATOMIC(__Int128)*)object,
                              *(const __Int128*)value,
                              memoryOrder);
        break;
    }
#endif
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_exchange(void* object,
                      const void* value,
                      void* result,
                      int size,
                      memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)result = atomic_exchange_explicit((ATOMIC(char)*)object,
                                                  *(const char*)value,
                                                  memoryOrder);
        break;
    }
    case 2: {
        *(short*)result = atomic_exchange_explicit((ATOMIC(short)*)object,
                                                   *(const short*)value,
                                                   memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)result = atomic_exchange_explicit((ATOMIC(int32_t)*)object,
                                                     *(const int32_t*)value,
                                                     memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)result = atomic_exchange_explicit((ATOMIC(int64_t)*)object,
                                                     *(const int64_t*)value,
                                                     memoryOrder);
        break;
    }
#if X64
    case 16: {
        *(__Int128*)result = atomic_exchange_explicit((ATOMIC(__Int128)*)object,
                                                      *(const __Int128*)value,
                                                      memoryOrder);
        break;
    }
#endif
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_cas(void* object,
                 void* expect,
                 const void* exchange,
                 int size,
                 memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)expect
            = atomic_compare_exchange_strong_explicit((ATOMIC(char)*)object,
                                                      (char*)expect,
                                                      *(const char*)exchange,
                                                      memoryOrder,
                                                      memoryOrder);
        break;
    }
    case 2: {
        *(short*)expect
            = atomic_compare_exchange_strong_explicit((ATOMIC(short)*)object,
                                                      (short*)expect,
                                                      *(const short*)exchange,
                                                      memoryOrder,
                                                      memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)expect
            = atomic_compare_exchange_strong_explicit((ATOMIC(int32_t)*)object,
                                                      (int32_t*)expect,
                                                      *(const int32_t*)exchange,
                                                      memoryOrder,
                                                      memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)expect
            = atomic_compare_exchange_strong_explicit((ATOMIC(int64_t)*)object,
                                                      (int64_t*)expect,
                                                      *(const int64_t*)exchange,
                                                      memoryOrder,
                                                      memoryOrder);
        break;
    }
#if X64
    case 16: {
        atomic_compare_exchange_strong_explicit((ATOMIC(__Int128)*)object,
                                                (__Int128*)expect,
                                                *(const __Int128*)exchange,
                                                memoryOrder,
                                                memoryOrder);
        break;
    }
#endif
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_casWeak(void* object,
                     void* expect,
                     const void* exchange,
                     int size,
                     memory_order memoryOrder)
{
    switch (size)
    {
    case 1: {
        *(char*)expect
            = atomic_compare_exchange_weak_explicit((ATOMIC(char)*)object,
                                                    (char*)expect,
                                                    *(const char*)exchange,
                                                    memoryOrder,
                                                    memoryOrder);
        break;
    }
    case 2: {
        *(short*)expect
            = atomic_compare_exchange_weak_explicit((ATOMIC(short)*)object,
                                                    (short*)expect,
                                                    *(const short*)exchange,
                                                    memoryOrder,
                                                    memoryOrder);
        break;
    }
    case 4: {
        *(int32_t*)expect
            = atomic_compare_exchange_weak_explicit((ATOMIC(int32_t)*)object,
                                                    (int32_t*)expect,
                                                    *(const int32_t*)exchange,
                                                    memoryOrder,
                                                    memoryOrder);
        break;
    }
    case 8: {
        *(int64_t*)expect
            = atomic_compare_exchange_weak_explicit((ATOMIC(int64_t)*)object,
                                                    (int64_t*)expect,
                                                    *(const int64_t*)exchange,
                                                    memoryOrder,
                                                    memoryOrder);
        break;
    }
#if X64
    case 16: {
        atomic_compare_exchange_weak_explicit((ATOMIC(__Int128)*)object,
                                              (__Int128*)expect,
                                              *(const __Int128*)exchange,
                                              memoryOrder,
                                              memoryOrder);
        break;
    }
#endif
    default:
        return false;
    }
    return true;
}
#endif
