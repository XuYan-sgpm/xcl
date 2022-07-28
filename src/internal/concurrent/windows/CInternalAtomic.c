//
// Created by xuyan on 2022/7/28.
//

#include "xcl/concurrent/CInternalAtomic.h"
#include "xcl/lang/XclDef.h"
#include <windows.h>

#if defined(_MSC_VER)

#include <stdbool.h>

bool
__XCL_atomic_inc(void* object, void* result, int size, memory_order memoryOrder)
{
    switch (size)
    {
    case 2:
        *(SHORT*)result = InterlockedIncrement16((volatile SHORT*)object);
        break;
    case 4:
        *(LONG*)result = InterlockedIncrement((volatile LONG*)object);
        break;
    case 8:
        *(LONG64*)result = InterlockedIncrement64((volatile LONG64*)object);
        break;
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
    case 2:
        *(SHORT*)result = InterlockedDecrement16((volatile SHORT*)object);
        break;
    case 4:
        *(LONG*)result = InterlockedDecrement((volatile LONG*)object);
        break;
    case 8:
        *(LONG64*)result = InterlockedDecrement64((volatile LONG64*)object);
        break;
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
    case 4:
        *(LONG*)result = InterlockedAdd((volatile LONG*)object, *(LONG*)value);
        break;
    case 8:
        *(LONG64*)result
            = InterlockedAdd64((volatile LONG64*)object, *(LONG64*)value);
        break;
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
    case 1:
        *(char*)result = InterlockedOr8((volatile char*)object, 0);
        break;
    case 2:
        *(short*)result = InterlockedOr16((volatile short*)object, 0);
        break;
    case 4:
        *(LONG*)result = InterlockedOr((volatile LONG*)object, 0);
        break;
    case 8:
        *(LONG64*)result = InterlockedOr64((volatile LONG64*)object, 0);
        break;
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
    case 1:
        InterlockedExchange8((volatile char*)object, *(char*)value);
        break;
    case 2:
        InterlockedExchange16((volatile short*)object, *(short*)value);
        break;
    case 4:
        InterlockedExchange((volatile LONG*)object, *(LONG*)value);
        break;
    case 8:
        InterlockedExchange64((volatile LONG64*)object, *(LONG64*)value);
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
    case 1:
        *(char*)result
            = InterlockedExchange8((volatile char*)object, *(char*)value);
        break;
    case 2:
        *(short*)result
            = InterlockedExchange16((volatile short*)object, *(short*)value);
        break;
    case 4:
        *(LONG*)result
            = InterlockedExchange((volatile LONG*)object, *(LONG*)value);
        break;
    case 8:
        *(LONG64*)result
            = InterlockedExchange64((volatile LONG64*)object, *(LONG64*)value);
        break;
    default:
        return false;
    }
    return true;
}

bool
__XCL_atomic_compare_exchange(void* object,
                              void* expect,
                              const void* exchange,
                              int size,
                              memory_order memoryOrder)
{
    bool ret = true;
    switch (size)
    {
    case 2:
        *(short*)expect = InterlockedCompareExchange16((volatile short*)object,
                                                       *(const short*)exchange,
                                                       *(short*)expect);
        break;
    case 4:
        *(LONG*)expect = InterlockedCompareExchange((volatile LONG*)object,
                                                    *(const LONG*)exchange,
                                                    *(LONG*)expect);
        break;
    case 8:
        *(LONG64*)expect
            = InterlockedCompareExchange64((volatile LONG64*)object,
                                           *(const LONG64*)exchange,
                                           *(LONG64*)expect);
        break;
#if X64
    case 16: {
        typedef struct {
            LONG64 pair[2];
        } __Int128;
        ret = InterlockedCompareExchange128(
            ((volatile __Int128*)object)->pair,
            ((const __Int128*)exchange)->pair[1],
            ((const __Int128*)exchange)->pair[0],
            ((__Int128*)expect)->pair);
        break;
    }
#endif
    default:
        ret = false;
        break;
    }
    return ret;
}

#endif